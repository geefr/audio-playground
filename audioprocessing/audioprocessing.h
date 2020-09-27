#ifndef AUDIOPROCESSING_H
#define AUDIOPROCESSING_H

#include "audio.h"

// TODO: May not be the fastest option, but should work for what's needed
#include "kissfft/kiss_fft.h"

#include <memory>

/**
 * Utility class containing audio processing algorithms
 */
class AudioProcessing {
public:
  AudioProcessing() = delete;

  /**
   * Calculate an FFT around time t
   * @param audio The audio to sample
   * @param t The center of the fft window
   * @param windowSize The window size, in samples
   * @return The calculated FFT
   */
  static std::unique_ptr<kiss_fft_cpx> FFT( Audio& audio, float t, uint32_t windowSize );
};

#endif
