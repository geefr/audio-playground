
#include "ui.h"

#include <imgui.h>
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>

#include <stdexcept>

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

  // Setup ImGui
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  auto imIO = ImGui::GetIO();
  ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
  ImGui_ImplOpenGL3_Init(nullptr);
  ImGui::StyleColorsDark();
  auto& s = ImGui::GetStyle();
  s.Alpha = 0.8f;
  s.WindowRounding = 0.f;
  s.WindowMinSize = ImVec2(1.f,1.f);
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

  renderUI();

  // TODO: We need to deal with synchronisation as we're uploading the texture each frame
  // The correct way to do this is with a ringbuffer of textures, to let us be 1/2 frames
  // ahead of where the gpu is, and avoid changing texture contents during a draw
  // To get this working quickly however we'll just wait for the pipeline to be idle
  glFlush();
  glFinish();

  glfwSwapBuffers(mWindow);
}

void UI::renderUI() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  renderUITopBar();
  renderUIBottomBar();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UI::renderUITopBar() {
  // Main menu bar with useful elements
  ImGui::Begin("header", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar );
  ImGui::SetWindowPos( ImVec2(0.0, 0.0) );
  ImGui::SetWindowSize( ImVec2(mWidth, 36) );

  renderComboShader();

  ImGui::SameLine();

  ImGui::End();
}

void UI::renderComboShader() {
  // Shader selection Combo Box
  mComboShaderData.clear();
  for( auto& s : mEngine.mShaders ) mComboShaderData.emplace_back(s.first);

  ImGui::PushItemWidth(200.f);
  const char* currentItem = mComboShaderData[mComboShaderDataSelectedIndex].c_str();
  if( ImGui::BeginCombo("", currentItem, ImGuiComboFlags_HeightLargest ) ) {
      auto shaders = mEngine.mShaders;
      for( auto i = 0u; i < mComboShaderData.size(); ++i ) {
        auto selected = i == mComboShaderDataSelectedIndex;
        if( ImGui::Selectable(mComboShaderData[i].c_str(), selected) ) {
          currentItem = mComboShaderData[i].c_str();
          mComboShaderDataSelectedIndex = i;
        }
        if( selected ) {
          ImGui::SetItemDefaultFocus();
        }
      }
      ImGui::EndCombo();
  }
  ImGui::PopItemWidth();

  mEngine.activateShaderToy(mComboShaderData[mComboShaderDataSelectedIndex]);
}

void UI::renderUIBottomBar() {
  ImGui::Begin("footer", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar );
  ImGui::SetWindowPos( ImVec2(0.0, mHeight - 32) );
  ImGui::SetWindowSize( ImVec2(mWidth, 32) );

  ImGui::Text("X: Next Shader S: Next song F11: Fullscreen ESC: Exit");

  ImGui::End();
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
                mComboShaderDataSelectedIndex++;
                if( mComboShaderDataSelectedIndex >= mComboShaderData.size() ) mComboShaderDataSelectedIndex = 0;
                mEngine.activateShaderToy(mComboShaderData[mComboShaderDataSelectedIndex]);
                break;
        }
    }
}

void UI::mouseButtonCallback(GLFWwindow* window, int button, int action,[[maybe_unused]]  int mods) {}
void UI::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {}

bool UI::shouldClose() const { return glfwWindowShouldClose(mWindow); }
