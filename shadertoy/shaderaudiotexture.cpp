
#include "shaderaudiotexture.h"

#include "audioloaders/audio.h"

#include <iostream>

ShaderAudioTexture::ShaderAudioTexture() {
  glGenTextures(1, &mId);
  bind(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  mBuffer.reset(new float[mWidth * mHeight]);
}

ShaderAudioTexture::~ShaderAudioTexture() {}

GLuint ShaderAudioTexture::id() const { return mId; }

void ShaderAudioTexture::bind( GLenum target ) const {
  glBindTexture(target, mId);
}

void ShaderAudioTexture::setAudio( Audio& audio, float startT, float endT ) {
  mPlaybackTime = startT;

  // Sample the first channel
  uint32_t numSamples = 0;
  auto samples = audio.sample(0, startT, endT, numSamples);

  // If the audio has more channels then combined with an average
  for( auto i = 1u; i < audio.numChannels(); ++i ) {
    uint32_t numSamplesI = 0;
    auto samplesI = audio.sample(i, startT, endT, numSamplesI);
    numSamplesI = std::min(numSamples, numSamplesI);
    for( auto x = 0u; x < numSamplesI; ++x) {
        samples[x] += samplesI[x];
    }
  }
  for( auto x = 0u; x < numSamples; ++x ) {
    samples[x] /= static_cast<float>(audio.numChannels());
  }


  // Fill the FFT component of the texture (y == 0)
//  for( auto x = 0u; x < mWidth; ++x ) {
//    auto sampleI = static_cast<float>(x / mWidth);
//    mBuffer[x] = samples[ static_cast<uint32_t>(sampleI * static_cast<float>(numSamples))];
//  }

  // Fill the amplitude component of the texture (y == 1)
  for( auto x = 0u; x < mWidth; ++x ) {
    auto sampleI = static_cast<float>(x) / static_cast<float>(mWidth);
    auto sample = samples[ static_cast<uint32_t>(sampleI * static_cast<float>(numSamples))];

    // TODO: This looks close to what shadertoy displays, but there's something not quite right
    // Might be down to differing sample rates/etc? Or maybe shadertoy does some interpolation instead
    // of nearest neighbour on the samples as theirs looks 'smoother'
    mBuffer[(1 * mWidth) + x] = 0.5 + ((static_cast<float>(sample)) / static_cast<float>(audio.maxAmplitude()) / 2.0);
  }

  bind(GL_TEXTURE_2D);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, mWidth, mHeight, 0, GL_RED, GL_FLOAT, mBuffer.get());

}

uint32_t ShaderAudioTexture::width() const { return mWidth; }
uint32_t ShaderAudioTexture::height() const { return mHeight; }
float ShaderAudioTexture::playbackTime() const { return mPlaybackTime; }
