cmake_minimum_required(VERSION 3.10)

project(openal-test)

# Compiler/Tool requirements
set( CMAKE_CXX_STANDARD 20 )

find_package( OpenAL REQUIRED )

if( MSVC )
else()
  set( CMAKE_CXX_FLAGS "-Werror=all -Wno-error=old-style-cast -Wno-error=unused-variable -Wno-error=unused-but-set-variable" )
endif()

include_directories( 
 ${OPENAL_INCLUDE_DIR}
)
link_directories( 
)

add_executable( openal-test
  main.cpp
  engine.cpp
  )

target_link_libraries( openal-test
  ${OPENAL_LIBRARY}
)
