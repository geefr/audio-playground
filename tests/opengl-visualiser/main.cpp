
#include <iostream>
#include <memory>
#include <chrono>
#include <random>

#include "ui.h"

#include "openalengine/openalengine.h"
#include "audioprocessing/audio.h"
#include "shadertoy/shadertoyengine.h"

// TODO: These are placeholder shaders - should be talking to the shadertoy rest api to fetch these
#include "shadertoy/shadertoyshaders.h"

ShaderToyEngine engine;

/// Select a random song to play
/// TODO: Encapsulate in a better place, typedef the map, do this in the background, etc
std::vector<std::string>::iterator shuffleMusic(std::vector<std::string>& music) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(0, music.size() - 1);

  auto it = music.begin();
  std::advance(it, distrib(gen));
  return it;
}

int main(int argc, char** argv) {
try
{
    std::string filename;
    if( argc >= 2 ) {
      filename = argv[1];
    } else {
      std::cout << "Loading all music files, please wait..";
    }

    // Audio playback engine - OpenAL is overkill for what's needed, but simple to use
    std::unique_ptr<OpenALEngine> audioEngine(new OpenALEngine());

    UI ui(engine);

    // Setup the rendering engine
    std::string shaderDir = "../shaders";
    auto shaderEnv = std::getenv("SHADER_DIR");
    if( shaderEnv ) {
      shaderDir = shaderEnv;
    }
    engine.init(shaderDir);

    // Load audio
    std::string musicDir = "../music";
    auto musicEnv = std::getenv("MUSIC_DIR");
    if( musicEnv ) {
      musicDir = musicEnv;
    }

    std::vector<std::string> audioFiles;
    if( !filename.empty() ) {
        audioFiles.push_back(filename);
    } else {
      if( !Audio::FindSupportedFiles(musicDir, audioFiles) ) {
          std::cout << "Failed to find audio files in " << musicDir << std::endl;
          return EXIT_FAILURE;
        }
    }

    // Initialise shader input channels (audio textures)
    // Note: Creation requires a bound context - contructor will perform texture allocations
    std::shared_ptr<ShaderAudioTexture> audioTex0(new ShaderAudioTexture());

    engine.audioTextures().push_back(audioTex0);

    // The time when the audio source started playing
    auto audioStartTime = std::chrono::steady_clock::now();
    auto shaderStartTime = audioStartTime;
    auto shaderSwitchTime = 10.f;

    // Resources for playing the audio
    std::shared_ptr<OpenALEngine::Source> audioSrc;
    std::shared_ptr<OpenALEngine::Buffer> audioBuf;
    std::shared_ptr<Audio> audio;
    auto trackIndex = 0u;

    while(!ui.shouldClose())
    {
        // Handle input events, ui actions
        ui.pollEvents();

        auto currentTime = std::chrono::steady_clock::now();

        // Update any graphics simulation/timing data
        engine.update();

        // Ensure the sound is playing, loop if it's not
        if( !audioEngine->isSourcePlaying(audioSrc) || ui.mSkipTrack ) {
            // TODO: Should run OpenAL in another thread if possible - It's fairly high cpu usage, and at the moment the graphics will stall when changing tracks
            // TODO: This section really needs some work - openAL is a poor choice for just playing music, and this is pretty ugly here
            // auto audioIt = shuffleMusic(audioFiles);
            // audio = Audio::open(*audioIt);
            trackIndex++;
            if( trackIndex == audioFiles.size() ) trackIndex = 0;
            audio = Audio::open(audioFiles[trackIndex]);
            if( !audio ) {
                std::cout << "Failed to open audio file: " << audioFiles[trackIndex] << std::endl;
                return EXIT_FAILURE;
              }

            if( audioSrc ) {
                audioEngine->stopSource(audioSrc);
                audioEngine->deleteSource(audioSrc);
                audioEngine->deleteBuffer(audioBuf);
              }

            // Prepare a playback source
            audioSrc = audioEngine->createSource();
            audioBuf = audioEngine->createBuffer(*audio);
            audioEngine->bindBufferToSource(audioSrc, audioBuf);

            audioEngine->playSource(audioSrc);
            audioStartTime = std::chrono::steady_clock::now();
            ui.mSkipTrack = false;
          }

        auto audioOffsetSeconds = audioEngine->sourcePlaybackOffset(audioSrc);
        auto audioWindow = engine.updateDelta();

        // Update audio input to the renderer
        audioTex0->setAudio( *audio, audioOffsetSeconds, audioOffsetSeconds + audioWindow );

        // Switch the visualisation on a regular interval
        // TODO: Should be more sophisticated here, Only switch on a beat, etc
        if( ((currentTime - shaderStartTime) / std::chrono::milliseconds(1)) / 1000.f > shaderSwitchTime ) {
          engine.nextShader();
          shaderStartTime = currentTime;
        }

        // Render to the screen
        ui.render();
    }

}
catch(const std::exception& e)
{
    std::cerr << e.what() << std::endl;
    return 1;
}
catch(...)
{
    std::cerr << "Unknown exception" << std::endl;
    return 1;
}

return 0;
}
