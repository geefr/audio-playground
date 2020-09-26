cmake_minimum_required(VERSION 3.10)

project(audio-playground)

# Compiler/Tool requirements
set( CMAKE_CXX_STANDARD 20 )

if( MSVC )
else()
  # TODO: Assuming GCC, will have flag mismatch with clang
  set( CMAKE_CXX_FLAGS "-Werror=all -Wno-error=old-style-cast -Wno-error=unused-variable -Wno-error=unused-but-set-variable" )
endif()

add_subdirectory( extern/kissfft )

add_subdirectory( audioloaders )
add_subdirectory( openalengine )
add_subdirectory( shadertoy )
add_subdirectory( tests )

