
add_library( imgui STATIC
  imgui/examples/imgui_impl_opengl3.h
  imgui/examples/imgui_impl_opengl3.cpp
  imgui/examples/imgui_impl_glfw.h
  imgui/examples/imgui_impl_glfw.cpp
)
target_include_directories( imgui PUBLIC ${PROJECT_SOURCE_DIR}/extern/imgui )

