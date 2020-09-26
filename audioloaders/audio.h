#ifndef AUDIO_H
#define AUDIO_H

#include <string>
#include <memory>

/**
 * A sound buffer/audio track
 * @note For now this is hardcoded to use signed 16-bit samples
 */
class Audio {
public:
  virtual ~Audio() = default;

  /**
   * Factory method - Load an audio file into the appropriate object
   * @param filename File to load
   * @return Loaded audio, nullptr if file not supported
   */
  static std::unique_ptr<Audio> open( std::string filename );

  /// The file the audio was loaded from (if any)
  std::string filename() const;

  /// Number of audio channels
  uint32_t numChannels() const;

  /// Sample rate in Hz
  uint32_t sampleRate() const;

  /// Length of the audio in seconds
  float lengthSeconds() const;

  /**
   * Read a single sample
   * @note Method will be slow if called repeatedly, consider querying a range instead
   * @param channel The channel to read
   * @param t The time at which to read, must be < lengthSeconds()
   * @return The requested sample, zero on error
   */
  virtual int16_t sample( uint32_t channel, float t ) = 0;

  /**
   * Read samples from a single channel
   * @param channel The channel to read
   * @param startT The start time to read from
   * @param endT The end time to read from
   * @param numSamples Will be set to size of returned array (TODO: This isn't nice, return a struct)
   * @return The requested range, or unique_ptr() on error
   */
  virtual std::unique_ptr<int16_t[]> sample( uint32_t channel, float startT, float endT, uint32_t& numSamples ) = 0;

  /// Direct access to audio buffer
  virtual int16_t* data() const = 0;

  /// Size of the audio buffer, in bytes
  virtual uint64_t dataSize() const = 0;

  /// Min/Max amplitude
  int16_t minAmplitude() const;
  int16_t maxAmplitude() const;

protected:
  Audio() = default;

  std::string mFilename;
  uint32_t mNumChannels = 0;
  uint32_t mSampleRate = 0;
  float mLengthSeconds = 0.f;
  int16_t mAmplitudeMax = 0;
};

#endif
