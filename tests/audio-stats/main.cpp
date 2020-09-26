
#include <iostream>
#include <memory>

#include "audioloaders/audio.h"

int main(int argc, char** argv) {
  if( argc < 2 ) {
    std::cout << "USAGE: " << argv[0] << " <Audio file>" << std::endl;
    return EXIT_FAILURE;
  }
  std::string filename(argv[1]);

  // Load the noise
  auto audio = Audio::open(filename);
  if( !audio || audio->data() == nullptr ) {
    std::cout << "Failed to open audio file: " << filename << std::endl;
    return EXIT_FAILURE;
  }

  // Log stats about the audio to stdout
  std::cout << 
  "File : " << audio->filename() << "\n" <<
  "Length : " << audio->lengthSeconds() << " seconds\n" <<
  "Channels: " << audio->numChannels() << "\n" <<
  "Sample Rate: " << audio->sampleRate() << "Hz\n";

  return EXIT_SUCCESS;
}
