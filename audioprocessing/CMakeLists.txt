
include_directories( 
 ${PROJECT_SOURCE_DIR}/extern
)

add_library( audioprocessing STATIC
  audio.h
  audio.cpp
  audiodrwav.h
  audiodrwav.cpp
  audiodrmp3.h
  audiodrmp3.cpp
  )

