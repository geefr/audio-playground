
include_directories( 
  ${PROJECT_SOURCE_DIR}
)

add_executable( openal-play
  main.cpp
)

target_link_libraries( openal-play
  audioprocessing
  openalengine
)

install( TARGETS openal-play 
         RUNTIME DESTINATION bin )
