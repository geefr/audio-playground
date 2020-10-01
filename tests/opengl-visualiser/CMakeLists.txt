
include_directories( 
  ${PROJECT_SOURCE_DIR}
)

add_executable( opengl-visualiser
  main.cpp
)

target_link_libraries( opengl-visualiser
  audioprocessing
  openalengine
  shadertoy
  glfw
)

install( TARGETS opengl-visualiser )
install( FILES README.md DESTINATION . )

