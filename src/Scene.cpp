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
    // Length
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
      // XXX: May be slower if there are multiple instances of the same node.
      meshes_.push_back(Mesh());
      Mesh& mesh = meshes_.back();

      mesh.name_ = n["name"].string();
      
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
