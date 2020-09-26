#ifndef AUDIOMP3_H
#define AUDIOMP3_H

#error "AudioMp3 implementation incomplete - do not use"

#include "audio.h"

/**
 * A sound buffer/audio track, loaded from MP3
 */
class AudioMp3 final : public Audio {
public:
  virtual ~AudioMp3();

  /**
   * Factory method - Load an mp3 file
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
   * @param endT The end time to read from
   * @return The requested range, or unique_ptr() on error
   */
  std::unique_ptr<int16_t[]> sample( uint32_t channel, float startT, float endT ) override;

  /// Direct access to audio buffer
  int16_t* data() const override;

  /// Size of the audio buffer, in bytes
  uint64_t dataSize() const override;

protected:
  AudioMp3();
};

#endif
