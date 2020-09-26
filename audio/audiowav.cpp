
#include "audiowav.h"

#include <algorithm>
#include <filesystem>
namespace fs = std::filesystem;

#include <iostream>
#include <memory>
#define DR_WAV_IMPLEMENTATION
#include "dr_libs/dr_wav.h"

AudioWav::AudioWav(std::string filename) {
  if( !fs::exists(fs::path(filename)) ) {
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

  mDataSize = mTotalPCMFrames * mNumChannels * sizeof(drwav_int16);
  mLengthSeconds = static_cast<float>(mDataSize / mNumChannels) / mSampleRate;
}

AudioWav::~AudioWav() {
  if( mData ) {
    drwav_free(mData, nullptr);
  }
}

int16_t AudioWav::sample( uint32_t channel, float t ) {
  if( t < 0.f || t > mLengthSeconds ) return 0;

  // data is one sample per channel, for each 1/mSampleRate interval
  auto sampleI = static_cast<uint64_t>(t / mSampleRate);

  return mData[ (sampleI * mNumChannels) + channel ];
}

std::unique_ptr<int16_t[]> AudioWav::sample( uint32_t channel, float startT, float endT ) {
  startT = std::clamp(startT, 0.f, mLengthSeconds);
  endT = std::clamp(endT, 0.f, mLengthSeconds);

  // data is one sample per channel, for each 1/mSampleRate interval
  auto sampleStart = static_cast<uint64_t>(startT / mSampleRate);
  auto sampleEnd = static_cast<uint64_t>(endT / mSampleRate);

  // Must have at least 1 sample
  if( sampleEnd < sampleStart ) return {};

  std::unique_ptr<int16_t[]> res(new int16_t[sampleEnd - sampleStart + 1]);
  for( auto i = sampleStart; i < sampleEnd; ++i ) {
    res[i] = mData[ (i * mNumChannels) + channel ];
  }

  return res;
}

int16_t* AudioWav::data() const { return mData; }

uint64_t AudioWav::dataSize() const { return mDataSize; }
