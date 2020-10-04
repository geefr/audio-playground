#ifndef UI_H
#define UI_H

#include "shadertoy/shadertoyengine.h"

#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

/**
 * UI and window functions for the visualiser
 * Uses GLFW for window management
 * Uses imgui for ui elements
 */
class UI {
public:
  UI( ShaderToyEngine& engine );
  ~UI();

  /// Poll for events, act on the outcomes
  /// Should be called before render
  void pollEvents();

  /// Render a frame
  void render();

  void keyCallback(GLFWwindow* window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int modes);
  void mouseButtonCallback(GLFWwindow* window, int button, int action,[[maybe_unused]]  int mods);
  void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

  bool shouldClose() const;

  // TODO: Shouldn't really be here, used to notify main that it needs to skip to next track
  bool mSkipTrack = false;
private:
  void toggleFullscreen( GLFWwindow* window );
  void renderUI();

  ShaderToyEngine& mEngine;
  GLFWwindow* mWindow = nullptr;
  int mWidth = 800, mHeight = 600;
};

#endif
