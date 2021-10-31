
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

install( TARGETS opengl-visualiser 
         RUNTIME DESTINATION bin )
install( FILES README.md DESTINATION . )
if( NOT MSVC )
  install( FILES opengl-visualiser.sh DESTINATION .
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE )
endif()
