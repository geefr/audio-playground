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
