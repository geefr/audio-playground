
#include "shaderaudiotexture.h"

#include "audioloaders/audio.h"


// TODO: May not be the fastest option, but should work for what's needed
#include "kissfft/kiss_fft.h"

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

  // Fill the amplitude component of the texture (y == 1)
  for( auto x = 0u; x < mWidth; ++x ) {
    auto sampleI = static_cast<float>(x) / static_cast<float>(mWidth);
    auto sample = samples[ static_cast<uint32_t>(sampleI * static_cast<float>(numSamples))];

    // TODO: This looks close to what shadertoy displays, but there's something not quite right
    // Might be down to differing sample rates/etc? Or maybe shadertoy does some interpolation instead
    // of nearest neighbour on the samples as theirs looks 'smoother'
    mBuffer[(1 * mWidth) + x] = 0.5 + ((static_cast<float>(sample)) / static_cast<float>(audio.maxAmplitude()) / 2.0);
  }

  setAudioFFT( audio, startT, endT );

  bind(GL_TEXTURE_2D);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, mWidth, mHeight, 0, GL_RED, GL_FLOAT, mBuffer.get());
}

void ShaderAudioTexture::setAudioFFT( Audio& audio, float startT, float endT ) {
  // TODO: This shouldn't be the width - should be a sensible window around centerT
  auto fftWindowSize = mWidth;

  auto centerTime = (startT + endT) / 2.0f;
  auto centralSample = static_cast<uint64_t>(centerTime * static_cast<float>(audio.sampleRate()));
  auto sampleStart = centralSample - fftWindowSize / 2;
  auto sampleEnd = sampleStart + fftWindowSize;
  auto numSamples = 0u;

  auto samples = audio.sample(0, sampleStart, sampleEnd, numSamples);
  // If the audio has more channels then combined with an average
  for( auto i = 1u; i < audio.numChannels(); ++i ) {
    uint32_t numSamplesI = 0;
    auto samplesI = audio.sample(i, sampleStart, sampleEnd, numSamplesI);
    numSamplesI = std::min(numSamples, numSamplesI);
    for( auto x = 0u; x < numSamplesI; ++x) {
        samples[x] += samplesI[x];
    }
  }
  for( auto x = 0u; x < numSamples; ++x ) {
    samples[x] /= static_cast<float>(audio.numChannels());
  }

  // Fill the FFT component of the texture (y == 0)
  kiss_fft_cpx fftIn[numSamples], fftOut[fftWindowSize];
  for( auto s = 0u; s < numSamples; ++s ) {
    fftIn[s].r = samples[s];
    fftIn[s].i = 0;
  }

  kiss_fft_cfg fftCfg;
  if( (fftCfg = kiss_fft_alloc(mWidth, 0, nullptr, nullptr)) != nullptr) {
    kiss_fft(fftCfg, fftIn, fftOut);
    std::free(fftCfg);

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
        auto r = fftOut[fftX].r;
        auto i = fftOut[fftX].i;
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

        mFFTData[x] = mag;
        mBuffer[x] = mag;
    }
    //std::cerr << "FFT Max: " << fftMax << std::endl;

    // fftMin = std::max(fftMin, 0.1f * mFFtScale);

    for( auto x = 0u; x < mWidth; ++x ) {
      mBuffer[x] = (mBuffer[x]);
      fftMax = std::max(fftMax, mBuffer[x]);
      mBuffer[x] /= mFFtScale;
    }

    if( fftMax > mFFtScale ) {
        mFFtScale = fftMax;
    } else {
        mFFtScale = mFFtScale * (1.0 - (scaleDecayConstant * (endT - startT)));
    }
    //std::cerr << "FFT scale: " << mFFtScale << std::endl;

  }

}

uint32_t ShaderAudioTexture::width() const { return mWidth; }
uint32_t ShaderAudioTexture::height() const { return mHeight; }
float ShaderAudioTexture::playbackTime() const { return mPlaybackTime; }
