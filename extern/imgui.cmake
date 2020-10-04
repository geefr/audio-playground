
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

add_library( imgui STATIC
  imgui/imgui.h
  imgui/imgui.cpp
  imgui/imgui_draw.cpp
  imgui/imgui_widgets.cpp

  imgui/examples/imgui_impl_opengl3.h
  imgui/examples/imgui_impl_opengl3.cpp
  imgui/examples/imgui_impl_glfw.h
  imgui/examples/imgui_impl_glfw.cpp
)
target_include_directories( imgui PUBLIC ${PROJECT_SOURCE_DIR}/extern/imgui )
target_link_libraries( imgui glfw )

if( MSVC )
  target_include_directories( imgui PUBLIC ${GLEW_INCLUDE_DIR})  
endif()
