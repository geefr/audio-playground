#ifndef AUDIODRMP3_H
#define AUDIODRMP3_H

#include "audio.h"

/**
 * A sound buffer/audio track, supporting MP3 files
 */
class AudioDrMp3 final : public Audio {
public:
  AudioDrMp3() = delete;
  AudioDrMp3(std::string filename);
  virtual ~AudioDrMp3();

  /**
   * Read a single sample
   * @note Method will be slow if called repeatedly, consider querying a range instead
   * @param channel The channel to read
   * @param t The time at which to read, must be < lengthSeconds()
   * @return The requested sample, zero on error
   */
  int16_t sample( uint32_t channel, float t ) override;

  /**
   * Read samples from a single channel
   * @param channel The channel to read
   * @param startT The start time to read from
   * @param endT The end time to read to
   * @return The requested range, or unique_ptr() on error
   */
  std::unique_ptr<int16_t[]> sample( uint32_t channel, float startT, float endT, uint32_t& numSamples ) override;

  std::unique_ptr<int16_t[]> sample( uint32_t channel, uint64_t sampleStart, uint64_t sampleEnd, uint32_t& numSamples ) override;

  /// Direct access to audio buffer
  int16_t* data() const override;

  /// Size of the audio buffer, in bytes
  uint64_t dataSize() const override;

protected:
  int16_t* mData = nullptr;
  uint64_t mDataSize = 0;
  uint64_t mTotalPCMFrames = 0;
};

#endif
