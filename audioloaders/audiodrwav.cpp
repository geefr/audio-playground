
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

int16_t AudioDrWav::sample( uint32_t channel, float t ) {
  if( t < 0.f || t > mLengthSeconds ) return 0;

  // data is one sample per channel, for each 1/mSampleRate interval
  auto sampleI = static_cast<uint64_t>(t * static_cast<float>(mSampleRate));
  if( sampleI >= mTotalPCMFrames ) return 0;

  return mData[ (sampleI * mNumChannels) + channel ];
}

std::unique_ptr<int16_t[]> AudioDrWav::sample( uint32_t channel, float startT, float endT, uint32_t& numSamples ) {
  // data is one sample per channel, for each 1/mSampleRate interval
  auto sampleStart = static_cast<uint64_t>(startT * static_cast<float>(mSampleRate));
  auto sampleEnd = static_cast<uint64_t>(endT * static_cast<float>(mSampleRate));

  // TODO: Better way to handle the edge case?
  std::clamp(sampleStart, 0ul, mTotalPCMFrames - 1);
  std::clamp(sampleEnd, 0ul, mTotalPCMFrames - 1);

  // Must have at least 1 sample
  numSamples = 0;
  if( sampleEnd < sampleStart ) return {};

  numSamples = sampleEnd - sampleStart + 1;
  std::unique_ptr<int16_t[]> res(new int16_t[numSamples * mNumChannels]);
  for( auto i = 0; i < numSamples; ++i ) {
    auto sampleOffset = ((sampleStart + i) * mNumChannels) + channel;
    res[i] = mData[sampleOffset];
  }

  return res;
}

int16_t* AudioDrWav::data() const { return mData; }

uint64_t AudioDrWav::dataSize() const { return mDataSize; }
