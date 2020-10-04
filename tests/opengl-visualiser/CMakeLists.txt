
include_directories( 
  ${PROJECT_SOURCE_DIR}
)

add_executable( opengl-visualiser
  main.cpp
  ui.h
  ui.cpp
)

target_link_libraries( opengl-visualiser
  audioprocessing
  openalengine
  shadertoy
  glfw
  imgui
)

install( TARGETS opengl-visualiser )
install( FILES README.md DESTINATION . )
if( NOT MSVC )
  install( FILES opengl-visualiser.sh DESTINATION . )
endif()

