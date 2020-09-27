#ifndef AUDIODRWAV_H
#define AUDIODRWAV_H

#include "audio.h"

/**
 * A sound buffer/audio track, supporting WAV files
 */
class AudioDrWav final : public Audio {
public:
  AudioDrWav() = delete;
  AudioDrWav(std::string filename);
  virtual ~AudioDrWav();

  /// Direct access to audio buffer
  int16_t* data() const override;

  /// Size of the audio buffer, in bytes
  uint64_t dataSize() const override;

  /// Number of samples in the audio buffer
  uint64_t totalSamples() const override;

protected:
  int16_t* mData = nullptr;
  uint64_t mDataSize = 0;
  uint64_t mTotalPCMFrames = 0;
};

#endif
