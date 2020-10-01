#ifndef AUDIO_H
#define AUDIO_H

#include <string>
#include <memory>
#include <vector>

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
  static std::shared_ptr<Audio> open( std::string filename );

  /**
   * Factory method - Load all audio files from a directory
   * @param directory The directory to load files from
   * @param audioFiles Any loaded files will be appended to this vector
   * @return true if one or more files were loaded, false otherwise
   */
  static bool openRecursive( std::string directory, std::vector<std::shared_ptr<Audio>>& audioFiles );

  static bool FindSupportedFiles( std::string directory, std::vector<std::string>& audioFiles );

  /// The file the audio was loaded from (if any)
  std::string filename() const;

  /// Number of audio channels
  uint32_t numChannels() const;

  /// Sample rate in Hz
  uint32_t sampleRate() const;

  /// Length of the audio in seconds
  float lengthSeconds() const;

  /// Min/Max amplitude
  int16_t minAmplitude() const;
  int16_t maxAmplitude() const;

  /**
   * Read a single sample
   * @note Method will be slow if called repeatedly, consider querying a range instead
   * @param channel The channel to read
   * @param t The time at which to read, must be < lengthSeconds()
   * @return The requested sample, zero on error
   */
  int16_t sample( uint32_t channel, float t );

  /**
   * Read samples from a single channel
   * @param channel The channel to read
   * @param startSample The sample to start reading from
   * @param endSample The sample to read to
   * @param numSamples Will be set to the size of the returned array (TODO: Improve style)
   * @return
   */
  std::unique_ptr<int16_t[]> sample( uint32_t channel, uint64_t startSample, uint64_t endSample, uint64_t& numSamples );

  /**
   * Read samples from a single channel, between 2 timestamps
   * @param channel The channel to read
   * @param startT The start time to read from
   * @param endT The end time to read from
   * @param numSamples Will be set to size of returned array (TODO: Improve style)
   * @return The requested range, or unique_ptr() on error
   */
  std::unique_ptr<int16_t[]> sample( uint32_t channel, float startT, float endT, uint64_t& numSamples );

  /**
   * Read samples from all channels, return the average
   * @param startSample The sample to start reading from
   * @param endSample The sample to read to
   * @param numSamples Will be set to the size of the returned array (TODO: Improve style)
   * @return
   */
  std::unique_ptr<int16_t[]> sampleAllChannels( uint64_t startSample, uint64_t endSample, uint64_t& numSamples );

  /**
   * Read samples from all channels, return the average, between 2 timestamps
   * @param startT The start time to read from
   * @param endT The end time to read from
   * @param numSamples Will be set to size of returned array (TODO: Improve style)
   * @return The requested range, or unique_ptr() on error
   */
  std::unique_ptr<int16_t[]> sampleAllChannels( float startT, float endT, uint64_t& numSamples );

  /// Direct access to audio buffer
  virtual int16_t* data() const = 0;

  /// Size of the audio buffer, in bytes
  virtual uint64_t dataSize() const = 0;

  /// Number of samples in the audio buffer
  virtual uint64_t totalSamples() const = 0;

protected:
  Audio() = default;

  std::string mFilename;
  uint32_t mNumChannels = 0;
  uint32_t mSampleRate = 0;
  float mLengthSeconds = 0.f;
  int16_t mAmplitudeMax = 0;
};

#endif
