#ifndef AUDIOWAV_H
#define AUDIOWAV_H

#include "audio.h"

/**
 * A sound buffer/audio track, loaded from WAV
 */
class AudioWav final : public Audio {
public:
  AudioWav() = delete;
  AudioWav(std::string filename);
  virtual ~AudioWav();

  /**
   * Factory method - Load a wav file
   * @param filename File to load
   * @return Loaded audio, nullptr if file not supported
   */
  static std::unique_ptr<Audio> open( std::string filename );

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
  std::unique_ptr<int16_t[]> sample( uint32_t channel, float startT, float endT ) override;

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
