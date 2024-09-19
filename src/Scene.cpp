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

    // JSON chunk length
    f.read(buf, sizeof (i));
    if (f.fail())
    {
      throw ReadException("Missing first chunk.");
    }
    unsigned json_len = LilE(i);

    // JSON chunk test
    f.read(buf, sizeof (i));
    if (f.fail() || strncmp("JSON", buf, sizeof(buf)))
    {
      throw ReadException("JSON chunk not first.");
    }

    // Parse the JSON string real quick and store it in an object
    char* json_str = new char[json_len];
    
    f.read(json_str, json_len);
    if (f.fail())
    {
      throw ReadException("JSON reading failed.");
    }

    JSON json(json_str);
    auto& jsonr = json.root();

    delete [] json_str;

    // Required so no need to PointNode.
    if (jsonr["asset"]["version"].string() != "2.0")
    {
      throw ReadException("JSON version doesn't match header version.");
    }

    unsigned scene = 0;
    if (jsonr.PointNode("scene") != nullptr)
    {
      scene = jsonr["scene"].number();
    }

    name_ = jsonr["scenes"][scene]["name"].string();

    for (auto& n: jsonr["scenes"][scene]["nodes"])
    {
      unsigned index = n.number();
      
    }
  }

  Scene::~Scene()
  {
    
  }
}
