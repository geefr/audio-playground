
#include <iostream>
#include <memory>

#include "engine.h"

#define DR_WAV_IMPLEMENTATION
#include "extern/dr_libs/dr_wav.h"

int main(void) {
  std::unique_ptr<Engine> e(new Engine());

  // Load the noise
  drwav wav;
  unsigned int channels;
  unsigned int sampleRate;
  drwav_uint64 totalPCMFrameCount;
  drwav_int16* soundData = drwav_open_file_and_read_pcm_frames_s16("/home/gareth/Music/iamtheprotectorofthissystem.wav",
    &channels, &sampleRate, &totalPCMFrameCount, NULL);
  if( !soundData ) {
    throw std::runtime_error("Failed to load test wav");
  }

  // Upload sound data to OpenAL
  auto buf = e->createBuffer(channels, sampleRate, soundData, totalPCMFrameCount * channels * sizeof(drwav_int16));

  drwav_free(soundData, nullptr);
  drwav_uninit(&wav);

  // Prepare a playback source
  auto src = e->createSource();
  e->bindBufferToSource(src, buf);

  // Play until stop
  e->playSourceAndWait(src);

  return EXIT_SUCCESS;
}
