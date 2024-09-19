#include "Scene.hpp"
#include "Logger.hpp"
#include "endian.hpp"

#include <fstream>

namespace nogl
{
  Scene::Scene(const char* path)
  {
    std::ifstream f(path);
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
    f.seekg(4, std::ios_base::seekdir::_S_cur);

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
    f.read(buf, sizeof (i));
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
    for (auto& n: jsonr["meshes"])
    {
      // Primitive[0]
      if (n["primitives"].children_n() > 1)
      {
        throw ReadException("Multiple primitives not supported yet, too complex for now :(");
      }
      auto& primitive0 = n["primitives"][0];

      // Mode
      auto* mode = primitive0.PointNode("mode");
      if (mode != nullptr && mode->number() != 4)
      {
        throw ReadException("Primitive can only have mode 4(Triangles as primitive).");
      }
      
      meshes_.push_back(Mesh());
      Mesh& mesh = meshes_.back();
      
      // Name
      mesh.name_ = n["name"].string();

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
        const char* desired_type = "VEC3";
        unsigned desired_component_type = 5126;
        
        // Choose VOV from the mesh based on the primitive
        VOV4& vov = mesh.vertices_;
        if (attrib.key() == "NORMAL")
        {
          vov = mesh.normals_;
        }
        else if (attrib.key() == "TANGENTS")
        {
          vov = mesh.tangents_;
        }
        else if (attrib.key() == "TEXCOORD_0")
        {
          vov = mesh.texcoords_;
          desired_type = "VEC2";
        }
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
        vov.Reallocate(accessor["count"].number());

        auto& buffer_view = jsonr["bufferViews"][accessor["bufferView"].number()];

      
      }
    }
    // Node parsing
    for (auto& n: jsonr["scenes"][scene]["nodes"])
    {
      unsigned index = n.number();

      // XXX: May be slower if there are multiple instances of the same node.
      nodes_.push_back(Node());
      Node& node = nodes_.back();

      node.name_ = jsonr["nodes"][index]["name"].string();
    }

    delete [] json_chunk;
    delete [] bin_chunk;
  }

  Scene::~Scene()
  {
    
  }
}
