
#include "ui.h"

#include <stdexcept>
#include <functional>
using namespace std::placeholders;

[[noreturn]] void quit(std::string msg) { throw std::runtime_error(msg); }

void errorCallback(int error, const char* description) {
  // Not much we can do, just drop to log in case it's helpful
  std::cerr << "GLFW Error: " << error << ": " << description << std::endl;
}

// Bind glfw callbacks to UI instance
void gKeyCallback(GLFWwindow* window, int key,  int scancode, int action, int modes) {
  reinterpret_cast<UI*>(glfwGetWindowUserPointer(window))->keyCallback(window, key, scancode, action, modes); }
void gMouseButtonCallback(GLFWwindow* window, int button, int action,  int mods) {
  reinterpret_cast<UI*>(glfwGetWindowUserPointer(window))->mouseButtonCallback(window, button, action, mods); }
void gScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
  reinterpret_cast<UI*>(glfwGetWindowUserPointer(window))->scrollCallback(window, xoffset, yoffset); }

UI::UI( ShaderToyEngine& engine )
  : mEngine(engine)
{
  glfwSetErrorCallback(errorCallback);
  if( !glfwInit() ) quit("Failed to init glfw");

  // Let's try for GL 3.3, should be fine on almost everything now (it is 2020 after all)
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_SAMPLES, 8);
  mWindow = glfwCreateWindow(800, 600, "OpenGL-Visualiser", nullptr, nullptr);
  if( !mWindow ) quit("Failed to init window");

  glfwSetWindowUserPointer(mWindow, this);
  glfwMakeContextCurrent(mWindow);
  glfwSetKeyCallback(mWindow, gKeyCallback);
  glfwSetMouseButtonCallback(mWindow, gMouseButtonCallback);
  glfwSetScrollCallback(mWindow, gScrollCallback);

  glfwSwapInterval(1);
}

UI::~UI() {
  glfwDestroyWindow(mWindow);
  glfwTerminate();
}

void UI::pollEvents() { glfwPollEvents(); }

void UI::render() {
  // Hack, should really use framebuffersizecallback
  glfwGetFramebufferSize(mWindow, &mWidth, &mHeight);

  // Render to screen
  mEngine.render(mWidth, mHeight);

  // TODO: We need to deal with synchronisation as we're uploading the texture each frame
  // The correct way to do this is with a ringbuffer of textures, to let us be 1/2 frames
  // ahead of where the gpu is, and avoid changing texture contents during a draw
  // To get this working quickly however we'll just wait for the pipeline to be idle
  glFlush();
  glFinish();

  glfwSwapBuffers(mWindow);
}

void UI::toggleFullscreen( GLFWwindow* window ) {
  auto currentMon = glfwGetWindowMonitor(window);

  auto primaryMon = glfwGetPrimaryMonitor();
  int x = 0, y = 0, w = 800, h = 600;

  if( currentMon ) {
    // Disable fullscreen
    if( primaryMon ) {
      int mx = 0, my = 0;
      glfwGetMonitorPos(primaryMon, &mx, &my);
      glfwGetMonitorWorkarea(primaryMon, &x, &y, &w, &h);
      x = mx + (w / 2) - 400;
      y = my + (h / 2) - 300;
      w = 800;
      h = 600;
    }
    glfwSetWindowMonitor(window, nullptr, x, y, w, h, GLFW_DONT_CARE);
  } else {
    // Enable fullscreen
    if( primaryMon ) {
      glfwGetMonitorWorkarea(primaryMon, &x, &y, &w, &h);
      glfwSetWindowMonitor(window, primaryMon, x, y, w, h, GLFW_DONT_CARE);
    }
  }
}

void UI::keyCallback(GLFWwindow* window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int modes)
{
    if( action == GLFW_PRESS )
    {
        switch(key)
        {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GLFW_TRUE);
                break;
            case GLFW_KEY_F11:
                toggleFullscreen(window);
                break;
            case GLFW_KEY_S:
                mSkipTrack = true;
                break;
            case GLFW_KEY_X:
                mEngine.nextShader();
                break;
        }
    }
}

void UI::mouseButtonCallback(GLFWwindow* window, int button, int action,[[maybe_unused]]  int mods) {}
void UI::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {}

bool UI::shouldClose() const { return glfwWindowShouldClose(mWindow); }
