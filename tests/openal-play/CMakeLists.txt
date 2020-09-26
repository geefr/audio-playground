
include_directories( 
  ${PROJECT_SOURCE_DIR}
)

add_executable( openal-play
  main.cpp
)

target_link_libraries( openal-play
  audioloaders
  openalengine
)

