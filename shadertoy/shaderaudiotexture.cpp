
#include "shaderaudiotexture.h"

#include "audioloaders/audio.h"

// TODO: May not be the fastest option, but should work for what's needed
#include "kissfft/kiss_fft.h"

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

  // TODO: Refactor this bit, it's ugly
  {
    // Fill the FFT component of the texture (y == 0)
    kiss_fft_cpx fftIn[numSamples], fftOut[mWidth];
    for( auto s = 0u; s < numSamples; ++s ) {
      fftIn[s].r = samples[s];
      fftIn[s].i = 0;
    }
    kiss_fft_cfg fftCfg;
    if( (fftCfg = kiss_fft_alloc(mWidth, 0, nullptr, nullptr)) != nullptr) {
      kiss_fft(fftCfg, fftIn, fftOut);
      std::free(fftCfg);

      // Map the fft into the texture
      // Shadertoy texture here has a decay rate, and adds the frame's data to the graph.
      // Graph never goes above 1, so likely normalises each update
      auto fftMax = 0.f;
      auto smoothing = 0.2f;
      auto decayConstant = 1.0f;
      auto scalingConstant = 20.0f;
      for( auto x = 0u; x < mWidth; ++x ) {
          // Magnitude of FFT bucket
          auto r = fftOut[x / 2].r;
          auto i = fftOut[x / 2].i;
          auto mag2 =  (r*r) + (i*i);
          // Convert to dB
          auto mag = 10 * std::log10(mag2);

          // Lossy peak detector
          auto oldMag = mBuffer[x];
          if( mag > oldMag ) {
              mBuffer[x] = mag;
          } else {
              mBuffer[x] = oldMag * decayConstant * (endT - startT);
          }
          mBuffer[x] = mag;

          fftMax = std::max(fftMax, mag);

          // std::cerr << "FFT [" << x << "] : " << mag << "\n";

          // mBuffer[x] += mag;
          // mBuffer[x] *= 0.99;

//         auto r = fftOut[x].r / numSamples;
//         mBuffer[(0) + x] += r ;
//         mBuffer[(0) + x] *= 0.9;

      }
      std::cerr << "FFT Max: " << fftMax << std::endl;

      if( fftMax > mFFtScale ) {
          mFFtScale = fftMax;
      } else{
          mFFtScale *= (decayConstant * (endT - startT));
      }

      std::cerr << "FFT scale: " << mFFtScale << std::endl;

      for( auto x = 0u; x < mWidth; ++x ) {
        mBuffer[x] /= fftMax;
      }
    }

    // Normalise
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

  bind(GL_TEXTURE_2D);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, mWidth, mHeight, 0, GL_RED, GL_FLOAT, mBuffer.get());

}

uint32_t ShaderAudioTexture::width() const { return mWidth; }
uint32_t ShaderAudioTexture::height() const { return mHeight; }
float ShaderAudioTexture::playbackTime() const { return mPlaybackTime; }
