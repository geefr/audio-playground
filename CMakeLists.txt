cmake_minimum_required(VERSION 3.10)

project(audio-stats)

# Compiler/Tool requirements
set( CMAKE_CXX_STANDARD 20 )

if( MSVC )
else()
  # TODO: Assuming GCC, will have flag mismatch with clang
  set( CMAKE_CXX_FLAGS "-Werror=all -Wno-error=old-style-cast -Wno-error=unused-variable -Wno-error=unused-but-set-variable" )
endif()

# Locate system libraries
find_package( OpenAL REQUIRED )

include_directories( 
 ${OPENAL_INCLUDE_DIR}
 ${PROJECT_SOURCE_DIR}/extern
)

#link_directories( 
#)

add_executable( audio-stats
  main.cpp
  openalengine.h
  openalengine.cpp
  audio/audio.h
  audio/audio.cpp
  audio/audiowav.h
  audio/audiowav.cpp
  #audio/audiomp3.h
  #audio/audiomp3.cpp
  )

target_link_libraries( audio-stats
  ${OPENAL_LIBRARY}
)
