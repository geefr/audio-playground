
#include "audiomp3.h"

#include <filesystem>
namespace fs = std::filesystem;

AudioMp3::AudioMp3() {

}

AudioMp3::~AudioMp3() {

}

std::unique_ptr<Audio> AudioMp3::open( std::string filename ) {

  // check file exists
  // error handling
  //


  auto path = fs::path(filename);
  if( !fs::exists(path) ) {
      return {};
  }

  return {};
}

int16_t AudioMp3::sample( uint32_t channel, float t ) {
 return 0;
}

std::unique_ptr<int16_t[]> AudioMp3::sample( uint32_t channel, float startT, float endT ) {
 return {};
}

int16_t* AudioMp3::data() const {
 return nullptr;
}

uint64_t AudioMp3::dataSize() const {
 return 0;
}
