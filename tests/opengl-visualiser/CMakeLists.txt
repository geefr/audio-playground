
include_directories( 
  ${PROJECT_SOURCE_DIR}
)

add_executable( opengl-visualiser
  main.cpp
)

target_link_libraries( opengl-visualiser
  audioloaders
  openalengine
  shadertoy
  glfw
)
