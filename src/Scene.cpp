#include "Scene.hpp"
#include "Logger.hpp"
#include "endian.hpp"

#include <fstream>

namespace nogl
{
  Scene::Scene(const char* path)
  {
    std::ifstream f(path, std::ios::in | std::ios::binary);
    if (!f.is_open())
    {
      throw OpenException("Opening glTF file.");
    }

    union
    {
      char buf[4];
      uint32_t i = 0;
    };
    
    // Magic
    f.read(buf, sizeof (buf));
    if (f.fail() || strncmp("glTF", buf, sizeof(buf)))
    {
      throw ReadException("Invalid glTF.");
    }

    // Version must be 2
    f.read(buf, sizeof (i));
    if (f.fail() || LilE(i) != 2)
    {
      throw ReadException("Version must be 2.");
    }

    // Skip length
    f.read(buf, sizeof (i));

    // JSON chunk
    char* json_chunk;
    unsigned json_chunk_len;
    // Length
    f.read(buf, sizeof (i));
    if (f.fail())
    {
      throw ReadException("Missing first chunk.");
    }
    json_chunk_len = LilE(i);
    // Test if it's JSON, because has to be first
    f.read(buf, sizeof (buf));
    if (f.fail() || strncmp("JSON", buf, sizeof(buf)))
    {
      throw ReadException("JSON chunk not first.");
    }
    // Allocate the chunk in memory and read it
    json_chunk = new char[json_chunk_len];
    f.read(json_chunk, json_chunk_len);
    if (f.fail())
    {
      throw ReadException("JSON reading failed.");
    }

    // BIN chunk
    // FIXME: bin_chunk access is usually unsafe, because relying on the gltf file, so gotta wrap it in std::vector some time and use .at() instead of []
    char* bin_chunk = nullptr;
    unsigned bin_chunk_len = 0;
    f.read(buf, sizeof (i));
    if (!f.fail())
    {
      bin_chunk_len = LilE(i);
      // Test if it's BIN, because has to be first
      f.read(buf, sizeof (i));
      if (f.fail() || strncmp("BIN\0", buf, sizeof (buf)))
      {
        throw ReadException("Not a valid BIN chunk.");
      }
      // Allocate the chunk in memory and read it
      bin_chunk = new (std::align_val_t(4)) char[bin_chunk_len];
      f.read(bin_chunk, bin_chunk_len);
      if (f.fail())
      {
        throw ReadException("JSON reading failed.");
      }
    }
    if (!bin_chunk_len)
    {
      throw ReadException("BIN chunk required as of now.");
    }

    JSON json(json_chunk);
    auto& jsonr = json.root();

    // Required so no need to PointNode.
    if (jsonr["asset"]["version"].string() != "2.0")
    {
      throw ReadException("JSON version doesn't match header version.");
    }

    // Scene stuff
    unsigned scene = 0;
    if (jsonr.PointNode("scene") != nullptr)
    {
      scene = jsonr["scene"].number();
    }
    name_ = jsonr["scenes"][scene]["name"].string();

    // Mesh parsing
    for (auto& json_mesh: jsonr["meshes"])
    {
      // Primitive[0]
      if (json_mesh["primitives"].children_n() > 1)
      {
        throw ReadException("Multiple primitives not supported yet, too complex for now :(");
      }
      auto& primitive0 = json_mesh["primitives"][0];

      // Mode
      auto* mode = primitive0.PointNode("mode");
      if (mode != nullptr && mode->number() != 4)
      {
        throw ReadException("Primitive can only have mode 4(Triangles as primitive).");
      }
      
      meshes_.push_back(Mesh());
      Mesh& mesh = meshes_.back();
      
      // Name
      mesh.name_ = json_mesh["name"].string();

      // Indices
      {
        auto& indices = primitive0["indices"];
        auto& accessor = jsonr["accessors"][indices.number()];

        unsigned component_type = accessor["componentType"].number();
        if (
          accessor["type"].string() != "SCALAR"
          || (component_type != 5123 && component_type != 5125)
        )
        {
          throw ReadException("Bad indices accessor type/componentType.");
        }

        mesh.indices_.reset(new unsigned[3 * static_cast<unsigned>(accessor["count"].number())]);

        auto& buffer_view = jsonr["bufferViews"][accessor["bufferView"].number()];

        // Used for conversion and determining default byte_stride
        unsigned component_size = component_type == 5123 ? sizeof (uint16_t) : sizeof (uint32_t);

        unsigned byte_offset = buffer_view.PointNode("byteOffset") != nullptr ? buffer_view["byteOffset"].number() : 0;
        unsigned byte_stride;
        if (buffer_view.PointNode("byteStride") != nullptr)
        {
          byte_stride = buffer_view["byteStride"].number();
        }
        else
        {
          byte_stride = component_size * 3;
        }
        unsigned byte_length = buffer_view["byteLength"].number();

        // Now for the copying
        for (unsigned byte = byte_offset, comp = 0; byte < byte_offset + byte_length; byte += byte_stride, comp += 3)
        {
          switch (component_size)
          {
            // Just straight copy
            case sizeof (uint32_t):
            memcpy(mesh.indices_.get() + comp, bin_chunk + byte, component_size * 3);
            break;
            
            // Need conversion
            case sizeof (uint16_t):
            uint16_t* first_comp = reinterpret_cast<uint16_t*>(bin_chunk + byte);
            mesh.indices_[comp + 0] = first_comp[0];
            mesh.indices_[comp + 1] = first_comp[1];
            mesh.indices_[comp + 2] = first_comp[2];
            break;
          }
        }
      }

      // Attributes
      for (auto& attrib : primitive0["attributes"])
      {
        // ALL VALUES BELOW ASSUME THAT THE ATTRIBUTE IS POSITION, CHECK IFS BELOW.
        const char* desired_type = "VEC3";
        unsigned desired_component_type = 5126;
        unsigned components_n = 3; // How many components per vector in the glTF format, not the VOV
        float f[4] = {0,0,0,1}; // A buffer for copying into the vov easily. Value of the 4th component is set up below, not supposed to be 1 for non positional vectors.
        // Choose VOV from the mesh based on the primitive
        VOV4* vov = &mesh.vertices_;

        if (attrib.key() == "NORMAL")
        {
          vov = &mesh.normals_;
          f[3] = 0;
        }
        else if (attrib.key() == "TANGENTS")
        {
          vov = &mesh.tangents_;
          f[3] = 0;
        }
        // else if (attrib.key() == "TEXCOORD_0")
        // {
        //   vov = mesh.texcoords_;
        //   desired_type = "VEC2";
        //   components_n = 2;
        // }
        else if (attrib.key() != "POSITION")
        {
          Logger::Begin() << name_ << ": Skipping unsupported attribute key: " << attrib.key() << '.' << Logger::End();
          continue;
        }

        auto& accessor = jsonr["accessors"][attrib.number()];

        if (
          accessor["type"].string() != desired_type
          || accessor["componentType"].number() != desired_component_type
        )
        {
          throw ReadException("Bad accessor type/componentType.");
        }
        vov->Reallocate(accessor["count"].number());

        auto& buffer_view = jsonr["bufferViews"][accessor["bufferView"].number()];

        unsigned byte_offset = buffer_view.PointNode("byteOffset") != nullptr ? buffer_view["byteOffset"].number() : 0;
        unsigned byte_stride;
        if (buffer_view.PointNode("byteStride") != nullptr)
        {
          byte_stride = buffer_view["byteStride"].number();
        }
        else
        {
          byte_stride = sizeof (float) * components_n;
        }
        unsigned byte_length = buffer_view["byteLength"].number();

        for (unsigned byte = byte_offset, vec = 0; byte < byte_offset + byte_length; byte += byte_stride, ++vec)
        {
          float* first_comp = reinterpret_cast<float*>(bin_chunk + byte);
          switch (components_n)
          {
            case 2:
            break;

            case 3:
            f[0] = first_comp[0];
            f[1] = first_comp[1];
            f[2] = first_comp[2];
            (*vov)[vec] = f;
            break;
          }
        }
      }

      // After ALL THAT, we for sure have vertices_, at very least n()=0 so...
      mesh.vertices_projected_.Reallocate(mesh.vertices_.n());
    }

    // Node parsing
    for (auto& json_node: jsonr["scenes"][scene]["nodes"])
    {
      unsigned index = json_node.number();

      nodes_.push_back(Node());
      Node& node = nodes_.back();

      node.name_ = jsonr["nodes"][index]["name"].string();

      auto* mesh = jsonr["nodes"][index].PointNode("mesh");
      if (mesh != nullptr)
      {
        node.data_ = &meshes_.at(mesh->number());
        continue;
      }
    }

    // TODO: Cameras
    
    if (cameras_.empty())
    {
      cameras_.push_back(Camera());
      auto& camera = cameras_.back();
      camera.zfar_ = 1000.0f;
      camera.znear_ = 0.01f;
      camera.yfov_ = 1.39626f; // 80 degrees
      camera.aspect_ratio_ = 4.0f / 3.0f;
      camera.RecalculateMatrix();

      nodes_.push_back(Node());
      auto& node = nodes_.back();
      node.data() = &camera;

      main_camera_node_ = &node;
    }

    delete [] json_chunk;
    delete [] bin_chunk;
  }

  Scene::~Scene()
  {
    
  }
}
