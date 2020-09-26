#include "audio.h"

#include "audiodrwav.h"
#include "audiodrmp3.h"

#include <filesystem>
namespace fs = std::filesystem;

std::unique_ptr<Audio> Audio::open( std::string filename ) {
  // Helper method for creating from file
  // TODO: Could have child classes auto-register against a file type
  auto path = fs::path(filename);
  auto ext = path.extension();

  if( ext == ".wav" ) {
    // return std::unique_ptr<Audio>(new AudioDrWav(filename));
  }
  else if(ext == ".mp3") {
    return std::unique_ptr<Audio>(new AudioDrMp3(filename));
  }

  return {};
}

std::string Audio::filename() const { return mFilename; }

uint32_t Audio::numChannels() const { return mNumChannels; }

uint32_t Audio::sampleRate() const { return mSampleRate; }

float Audio::lengthSeconds() const { return mLengthSeconds; }

int16_t Audio::maxAmplitude() const { return mAmplitudeMax; }
