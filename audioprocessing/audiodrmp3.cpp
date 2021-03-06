
#include "audiodrmp3.h"

#include <algorithm>
#include <filesystem>
namespace fs = std::filesystem;

#include <iostream>
#include <memory>
#define DR_MP3_IMPLEMENTATION
#include "dr_libs/dr_mp3.h"

AudioDrMp3::AudioDrMp3(std::string filename) {
  auto path = fs::path(filename);
  if( !fs::exists(path) ) {
    return;
  }

  mFilename = filename;
  // TODO: Just loading the whole file here, simple approach may break down for large files
  drmp3_uint64 totalPCMframes;
  drmp3_config mp3Config;
  mData = drmp3_open_file_and_read_pcm_frames_s16(
        mFilename.c_str(),
        &mp3Config,
        &totalPCMframes,
        nullptr
  );
  // TODO: dr uses 'unsigned long long' for uint64_t
  mTotalPCMFrames = totalPCMframes;
  mNumChannels = mp3Config.channels;
  mSampleRate = mp3Config.sampleRate;

  mDataSize = (mTotalPCMFrames * mNumChannels) * sizeof(drmp3_int16);
  mLengthSeconds = static_cast<float>(mTotalPCMFrames) / mSampleRate;

  // TODO: Efficiency on this? It's needed for normalisation later at least
  for( auto i = 0u; i < mTotalPCMFrames; ++i ) {
      for( auto j = 0u; j < mNumChannels; ++j ) {
        auto a = static_cast<int16_t>(std::abs(mData[(i * mNumChannels) + j]));
        mAmplitudeMax = std::max(mAmplitudeMax, a);
      }
  }
}

AudioDrMp3::~AudioDrMp3() {
  if( mData ) {
    drmp3_free(mData, nullptr);
  }
}

int16_t* AudioDrMp3::data() const { return mData; }

uint64_t AudioDrMp3::dataSize() const { return mDataSize; }

uint64_t AudioDrMp3::totalSamples() const { return mTotalPCMFrames; }
