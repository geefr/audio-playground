
#include <iostream>
#include <memory>

#include "openalengine/openalengine.h"
#include "audioloaders/audio.h"

int main(int argc, char** argv) {
  if( argc < 2 ) {
    std::cout << "USAGE: " << argv[0] << " <Audio file>" << std::endl;
    return EXIT_FAILURE;
  }
  std::string filename(argv[1]);

  std::unique_ptr<OpenALEngine> e(new OpenALEngine());

  // Load the noise
  std::cerr << "Loading...: " << filename << std::endl;
  auto audio = Audio::open(filename);
  if( !audio || audio->data() == nullptr ) {
    std::cout << "Failed to open audio file: " << filename << std::endl;
    return EXIT_FAILURE;
  }
  std::cerr << "Audio loaded, playing through OpenAL" << std::endl;

  // Upload sound data to OpenAL
  // Note: Whole-buffer uploads only appropriate for short sounds etc - should stream the buffer for music
  // auto buf = e->createBuffer(channels, sampleRate, soundData, totalPCMFrameCount * channels * sizeof(drwav_int16));
  auto buf = e->createBuffer(*audio);

  // Prepare a playback source
  auto src = e->createSource();
  e->bindBufferToSource(src, buf);

  // Play until stop
  e->playSourceAndWait(src);

  return EXIT_SUCCESS;
}
