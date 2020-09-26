cmake_minimum_required(VERSION 3.10)

project(audio-playground)

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

add_executable( ${PROJECT_NAME}
  main.cpp
  openalengine.h
  openalengine.cpp
  audio/audio.h
  audio/audio.cpp
  audio/audiodrlibs.h
  audio/audiodrlibs.cpp
  )

target_link_libraries( ${PROJECT_NAME}
  ${OPENAL_LIBRARY}
)
