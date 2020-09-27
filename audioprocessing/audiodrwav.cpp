
#include "audiodrwav.h"

#include <algorithm>
#include <filesystem>
namespace fs = std::filesystem;

#include <iostream>
#include <memory>
#define DR_WAV_IMPLEMENTATION
#include "dr_libs/dr_wav.h"

AudioDrWav::AudioDrWav(std::string filename) {
  auto path = fs::path(filename);
  if( !fs::exists(path) ) {
    return;
  }

  mFilename = filename;
  // TODO: Just loading the whole file here, simple approach may break down for large files
  drwav_uint64 totalPCMframes;
  mData = drwav_open_file_and_read_pcm_frames_s16(
    mFilename.c_str(),
    &mNumChannels,
    &mSampleRate,
    &totalPCMframes,
    nullptr
  );
  // TODO: dr uses 'unsigned long long' for uint64_t
  mTotalPCMFrames = totalPCMframes;

  mDataSize = (mTotalPCMFrames * mNumChannels) * sizeof(drwav_int16);
  mLengthSeconds = static_cast<float>(mTotalPCMFrames) / mSampleRate;

  // TODO: Efficiency on this? It's needed for normalisation later at least
  for( auto i = 0u; i < mTotalPCMFrames; ++i ) {
      for( auto j = 0u; j < mNumChannels; ++j ) {
        auto a = static_cast<int16_t>(std::abs(mData[(i * mNumChannels) + j]));
        mAmplitudeMax = std::max(mAmplitudeMax, a);
      }
  }
}

AudioDrWav::~AudioDrWav() {
  if( mData ) {
    drwav_free(mData, nullptr);
  }
}

int16_t* AudioDrWav::data() const { return mData; }

uint64_t AudioDrWav::dataSize() const { return mDataSize; }

uint64_t AudioDrWav::totalSamples() const { return mTotalPCMFrames; }
