#include "audio.h"

#include "audiodrwav.h"
#include "audiodrmp3.h"

#include <cstring>
#include <filesystem>
namespace fs = std::filesystem;

std::unique_ptr<Audio> Audio::open( std::string filename ) {
  // Helper method for creating from file
  // TODO: Could have child classes auto-register against a file type
  auto path = fs::path(filename);
  auto ext = path.extension();

  if( ext == ".wav" ) {
    return std::unique_ptr<Audio>(new AudioDrWav(filename));
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

int16_t Audio::sample( uint32_t channel, float t ) {
  if( t < 0.f || t > mLengthSeconds ) return 0;

  // data is one sample per channel, for each 1/mSampleRate interval
  auto sampleI = static_cast<uint64_t>(t * static_cast<float>(mSampleRate));
  if( sampleI >= totalSamples() ) return 0;

  return data()[ (sampleI * mNumChannels) + channel ];
}

std::unique_ptr<int16_t[]> Audio::sample( uint32_t channel, float startT, float endT, uint32_t& numSamples ) {
  // data is one sample per channel, for each 1/mSampleRate interval
  auto sampleStart = static_cast<uint64_t>(startT * static_cast<float>(mSampleRate));
  auto sampleEnd = static_cast<uint64_t>(endT * static_cast<float>(mSampleRate));

  // TODO: Better way to handle the edge case?
  std::clamp(sampleStart, 0ul, totalSamples() - 1);
  std::clamp(sampleEnd, 0ul, totalSamples() - 1);

  return sample(channel, sampleStart, sampleEnd, numSamples);
}

std::unique_ptr<int16_t[]> Audio::sample( uint32_t channel, uint64_t sampleStart, uint64_t sampleEnd, uint32_t& numSamples ) {
  // Must have at least 1 sample
  numSamples = 0;
  if( sampleEnd < sampleStart ) return {};

  numSamples = sampleEnd - sampleStart + 1;
  std::unique_ptr<int16_t[]> res(new int16_t[numSamples * mNumChannels]);
  for( auto i = 0u; i < numSamples; ++i ) {
    auto sampleOffset = ((sampleStart + i) * mNumChannels) + channel;
    res[i] = data()[sampleOffset];
  }

  return res;
}

std::unique_ptr<int16_t[]> Audio::sampleAllChannels( float startT, float endT, uint32_t& numSamples ) {
  // data is one sample per channel, for each 1/mSampleRate interval
  auto sampleStart = static_cast<uint64_t>(startT * static_cast<float>(mSampleRate));
  auto sampleEnd = static_cast<uint64_t>(endT * static_cast<float>(mSampleRate));

  // TODO: Better way to handle the edge case?
  std::clamp(sampleStart, 0ul, totalSamples() - 1);
  std::clamp(sampleEnd, 0ul, totalSamples() - 1);

  return sampleAllChannels(sampleStart, sampleEnd, numSamples);
}

std::unique_ptr<int16_t[]> Audio::sampleAllChannels( uint64_t sampleStart, uint64_t sampleEnd, uint32_t& numSamples ) {
  // Must have at least 1 sample
  numSamples = 0;
  if( sampleEnd < sampleStart ) return {};

  numSamples = sampleEnd - sampleStart + 1;
  std::unique_ptr<int16_t[]> res(new int16_t[numSamples * mNumChannels]);
  std::memset(res.get(), 0x00, numSamples * mNumChannels * sizeof(int16_t));

  for( auto i = 0u; i < numSamples; ++i ) {
    for( auto s = 0u; s < mNumChannels; ++s ) {
      auto sampleOffset = ((sampleStart + i) * mNumChannels) + s;
      // TODO: May need to use floats here to avoid discarding small signals, should be fine for now
      res[i] += data()[sampleOffset] / mNumChannels;
    }
  }

  return res;
}
