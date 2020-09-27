
#include "audioprocessing.h"

std::unique_ptr<kiss_fft_cpx> AudioProcessing::FFT( Audio& audio, float t, uint32_t windowSize ) {
  auto tSample = static_cast<uint64_t>(t * static_cast<float>(audio.sampleRate()));
  auto sampleStart = tSample - windowSize / 2;
  auto sampleEnd = sampleStart + windowSize;

  auto numSamples = 0u;
  auto samples = audio.sampleAllChannels(sampleStart, sampleEnd, numSamples);

  if( !samples ) return {};

  // Populate input buffer
  kiss_fft_cpx fftIn[numSamples];
  std::unique_ptr<kiss_fft_cpx> fftOut(new kiss_fft_cpx[windowSize]);
  for( auto s = 0u; s < numSamples; ++s ) {
      fftIn[s].r = samples[s];
      fftIn[s].i = 0;
    }

  // Perform the FFT
  kiss_fft_cfg fftCfg;
  if( (fftCfg = kiss_fft_alloc(windowSize, 0, nullptr, nullptr)) != nullptr) {
      kiss_fft(fftCfg, fftIn, fftOut.get());
      std::free(fftCfg);
      return fftOut;
    }
  else {
      return {};
    }
}
