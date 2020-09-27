
#include "shaderaudiotexture.h"

#include "audioprocessing/audio.h"
#include "audioprocessing/audioprocessing.h"

#include <iostream>
#include <cstring>

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
  startT = std::max(startT, 0.0f); // First frame will ask for a negative timestamp

  mPlaybackTime = startT;

  // Sample the first channel
  uint32_t numSamples = 0;
  auto samples = audio.sampleAllChannels(startT, endT, numSamples);

  // Fill the amplitude component of the texture (y == 1)
  for( auto x = 0u; x < mWidth; ++x ) {
    auto sampleI = static_cast<float>(x) / static_cast<float>(mWidth);
    auto sample = samples[ static_cast<uint32_t>(sampleI * static_cast<float>(numSamples))];

    // TODO: This looks close to what shadertoy displays, but there's something not quite right
    // Might be down to differing sample rates/etc? Or maybe shadertoy does some interpolation instead
    // of nearest neighbour on the samples as theirs looks 'smoother'
    mBuffer[(1 * mWidth) + x] = 0.5 + ((static_cast<float>(sample)) / static_cast<float>(audio.maxAmplitude()) / 2.0);
  }

  // Fill the fft component of the texture (y == 0)
  setAudioFFT( audio, startT, endT );

  bind(GL_TEXTURE_2D);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, mWidth, mHeight, 0, GL_RED, GL_FLOAT, mBuffer.get());
}

void ShaderAudioTexture::setAudioFFT( Audio& audio, float startT, float endT ) {
  // TODO: This shouldn't be the width - should be a sensible window around centerT
  auto fftWindowSize = mWidth;

  auto fft = AudioProcessing::FFT(audio, (startT + endT) / 2.0f, fftWindowSize );
  if( !fft ) return;

  // Shift last frame's data over - used for peak decay
  mFFTDataLast = std::move(mFFTData);
  mFFTData.reset(new float[mWidth]);

  // Map the fft into the texture
  // Shadertoy texture here has a decay rate, and adds the frame's data to the graph.
  // Graph never goes above 1, so likely normalises each update
  // This behaviour is again close to shadertoy but not exact
  // - Lossy peak around 50%/s
  // - Scale decay around 10%/s
  // TODO: The scale/log nature of the values seems to be very off. Test case: Sandstorm, in the calm sections shadertoy's bars drop to zero. Maybe min and max norm? or just differences in mag
  // TODO: The top 10% or so of the fft appears to just be solid, but shadertoy preserves the detail up there. Is that a limitation of kissfft or maybe iterating wrong/etc below?
  // TODO: To test would need to use the same audio buffer for both - so either need 8-bit + big update windows here, or download the sample music from shadertoy to play locally aswell
  auto fftMax = 0.f;
  auto decayConstant = 0.9f;
  auto scaleDecayConstant = 0.1f;
  for( auto x = 0u; x < mWidth; ++x ) {
      float xPercent = static_cast<float>(x) / static_cast<float>(mWidth);
      auto fftX = static_cast<uint32_t>(((xPercent / 2.0) * fftWindowSize));

      // Magnitude of FFT bucket
      auto r = fft.get()[fftX].r;
      auto i = fft.get()[fftX].i;
      auto mag2 =  (r*r) + (i*i);
      // Convert to dB
      auto mag = 10.0f * std::log10(mag2 / 2.0f);

      // Lossy peak detector
      if( mFFTDataLast ) {
        auto oldMag = mFFTDataLast[x];
        if( mag < oldMag ) {
          mag = oldMag * (1.0 - (decayConstant * (endT - startT)));
        }
      }

      // Track peaks for next frame
      fftMax = std::max(fftMax, mag);
      mFFTData[x] = mag;

      // Data for the texture
      mag /= mFFtScale;
      mBuffer[x] = mag;
  }

  // Apply decay rate to the graph scale
  if( fftMax > mFFtScale ) {
      mFFtScale = fftMax;
  } else {
      mFFtScale = mFFtScale * (1.0 - (scaleDecayConstant * (endT - startT)));
  }
}

uint32_t ShaderAudioTexture::width() const { return mWidth; }
uint32_t ShaderAudioTexture::height() const { return mHeight; }
float ShaderAudioTexture::playbackTime() const { return mPlaybackTime; }
