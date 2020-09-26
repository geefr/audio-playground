
include_directories( 
  ${PROJECT_SOURCE_DIR}
)

add_executable( audio-stats
  main.cpp
)

target_link_libraries( audio-stats
  audioloaders
)

