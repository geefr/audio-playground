
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

  mDataSize = mTotalPCMFrames * mNumChannels * sizeof(drmp3_int16);
  mLengthSeconds = static_cast<float>(mDataSize / mNumChannels) / mSampleRate;
}

AudioDrMp3::~AudioDrMp3() {
  if( mData ) {
    drmp3_free(mData, nullptr);
  }
}

int16_t AudioDrMp3::sample( uint32_t channel, float t ) {
  if( t < 0.f || t > mLengthSeconds ) return 0;

  // data is one sample per channel, for each 1/mSampleRate interval
  auto sampleI = static_cast<uint64_t>(t / mSampleRate);

  return mData[ (sampleI * mNumChannels) + channel ];
}

std::unique_ptr<int16_t[]> AudioDrMp3::sample( uint32_t channel, float startT, float endT ) {
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

int16_t* AudioDrMp3::data() const { return mData; }

uint64_t AudioDrMp3::dataSize() const { return mDataSize; }
