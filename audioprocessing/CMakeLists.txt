
include_directories( 
 ${PROJECT_SOURCE_DIR}/extern
)

add_library( audioprocessing STATIC
  audioprocessing.h
  audioprocessing.cpp
  audio.h
  audio.cpp
  audiodrwav.h
  audiodrwav.cpp
  audiodrmp3.h
  audiodrmp3.cpp
  )

target_link_libraries( audioprocessing PUBLIC kissfft )

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  target_link_libraries( audioprocessing PUBLIC stdc++fs )
endif()
