
#include <iostream>
#include <memory>
#include <chrono>

#include "openalengine/openalengine.h"
#include "audioprocessing/audio.h"
#include "shadertoy/shadertoyengine.h"

// TODO: These are placeholder shaders - should be talking to the shadertoy rest api to fetch these
#include "shadertoy/shadertoyshaders.h"

#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

[[noreturn]] void quit(std::string msg)
{
    throw std::runtime_error(msg);
}

void errorCallback(int error, const char* description)
{
    std::cerr << "GLFW Error: " << error << ": " << description << std::endl;
}

void keyCallback(GLFWwindow* window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int modes)
{
    if( action == GLFW_PRESS )
    {
        switch(key)
        {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GLFW_TRUE);
                break;
            // case GLFW_KEY_1:
            //     viewRotDelta[0] = M_PI / 360.0f;
            //     break;
        }
    }
    else if( action == GLFW_RELEASE )
    {
        switch(key)
        {
            // case GLFW_KEY_1:
            // case GLFW_KEY_2:
            //     viewRotDelta[0] = 0.0f;
            //     break;
        }
    }
}

void mouseButtonCallback(GLFWwindow* window, int button, int action,[[maybe_unused]]  int mods)
{
    if( action != GLFW_PRESS ) return;

    double cx, cy;
    glfwGetCursorPos(window, &cx, &cy);
/*
    float x = static_cast<float>(cx) / engine.windowSize[0];
    float y = static_cast<float>(cy) / engine.windowSize[1];

    x = engine.viewExtent[0] + (x * (engine.viewExtent[1] - engine.viewExtent[0]));
    y = engine.viewExtent[2] + (y * (engine.viewExtent[3] - engine.viewExtent[2]));

    switch(button)
    {
        case GLFW_MOUSE_BUTTON_1:
            engine.viewCenter[0] += x;
            engine.viewCenter[1] += y;
            break;
    }
*/
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    yoffset *= -1.0f;
    if( yoffset < 0.0 ) yoffset = 1.0 - (-yoffset / 20.0);
    else yoffset = 1.0 + (yoffset / 20.0);

    /*
    engine.viewExtent[0] *= static_cast<float>(yoffset);
    engine.viewExtent[1] *= static_cast<float>(yoffset);
    engine.viewExtent[2] *= static_cast<float>(yoffset);
    engine.viewExtent[3] *= static_cast<float>(yoffset);
    */
}

int main(int argc, char** argv) {
try
{
    if( argc < 2 ) {
      std::cout << "USAGE: " << argv[0] << " <Audio file>" << std::endl;
      return EXIT_FAILURE;
    }
    std::string filename(argv[1]);

    // Audio playback engine - OpenAL is overkill for what's needed, but simple to use
    std::unique_ptr<OpenALEngine> audioEngine(new OpenALEngine());

    glfwSetErrorCallback(errorCallback);
    if( !glfwInit() ) quit("Failed to init glfw");

    // Let's try for GL 3.3, should be fine on almost everything now (it is 2020 after all)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_SAMPLES, 8);
    auto window = glfwCreateWindow(800, 600, "OpenGL-Visualiser", nullptr, nullptr);
    if( !window ) quit("Failed to init window");

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetScrollCallback(window, scrollCallback);

    glfwSwapInterval(1);

    // Setup the rendering engine
    ShaderToyEngine engine;
    engine.init(ShaderToyShaders::instance.ShaderToyBodyGeefrGlitchBlobs);

    // Load audio
    std::cerr << "Loading...: " << filename << std::endl;
    auto audio = Audio::open(filename);
    if( !audio || audio->data() == nullptr ) {
      std::cout << "Failed to open audio file: " << filename << std::endl;
      return EXIT_FAILURE;
    }
    std::cerr << "Audio loaded, playing through OpenAL" << std::endl;

    // Initialise shader input channels (audio textures)
    // Note: Creation requires a bound context - contructor will perform texture allocations
    std::shared_ptr<ShaderAudioTexture> audioTex0(new ShaderAudioTexture());

    engine.audioTextures().push_back(audioTex0);

    // TODO: Should run OpenAL in another thread if possible - It's fairly high cpu usage
    // Upload sound data to OpenAL
    // Note: Whole-buffer uploads only appropriate for short sounds etc - should stream the buffer for music
    auto audioBuf = audioEngine->createBuffer(*audio);

    // Prepare a playback source
    auto audioSrc = audioEngine->createSource();
    audioEngine->bindBufferToSource(audioSrc, audioBuf);

    // The time when the audio source started playing
    auto audioStartTime = std::chrono::steady_clock::now();

    auto width = 0;
    auto height = 0;
    while(!glfwWindowShouldClose(window))
    {
        // Pet the event doggie so it barks at our callbacks
        glfwPollEvents();

        auto currentTime = std::chrono::steady_clock::now();

        // Update any graphics simulation/timing data
        engine.update();

        // Ensure the sound is playing, loop if it's not
        if( !audioEngine->isSourcePlaying(audioSrc) ) {
          audioEngine->playSource(audioSrc);
          audioStartTime = currentTime;
        }

        auto audioOffsetSeconds = audioEngine->sourcePlaybackOffset(audioSrc);
        auto audioWindow = engine.updateDelta();
        // auto audioWindow = 1.0f;

        // Update audio input to the renderer
        audioTex0->setAudio( *audio, audioOffsetSeconds, audioOffsetSeconds + audioWindow );

        // Hack, should use framebuffersizecallback ;)
        glfwGetFramebufferSize(window, &width, &height);

        // Render to screen
        engine.render(width, height);

        // TODO: We need to deal with synchronisation as we're uploading the texture each frame
        // The correct way to do this is with a ringbuffer of textures, to let us be 1/2 frames
        // ahead of where the gpu is, and avoid changing texture contents during a draw
        // To get this working quickly however we'll just wait for the pipeline to be idle
        glFlush();
        glFinish();

        glfwSwapBuffers(window);
    }

    // TODO: Being naughty here and not deleting the OpenGL resources :/

    glfwDestroyWindow(window);
    glfwTerminate();
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
