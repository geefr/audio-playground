
#include "shadertoyshaders.h"

#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

ShaderToyShaders ShaderToyShaders::instance;

std::map<std::string, std::string> ShaderToyShaders::loadShaders( std::string shaderDir ) {
  if( !fs::exists(shaderDir) ) return {};

  std::map<std::string, std::string> shaders;
  for( auto& entry : fs::directory_iterator(shaderDir) ) {
      auto path = entry.path();
      if( path.extension() != ".frag" ) continue;

      std::ifstream f(path, std::ios::in | std::ios::binary);
      auto size = fs::file_size(path);
      std::string fragSrc(size, 0x00);
      f.read(fragSrc.data(), size);
      if( fragSrc.empty() ) continue;

      shaders[path.stem()] = fragSrc;
  }

  return shaders;
}
