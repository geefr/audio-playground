
# Locate system libraries
find_package( OpenAL REQUIRED )

include_directories(
 ${OPENAL_INCLUDE_DIR}
 ${PROJECT_SOURCE_DIR}
)

add_library( openalengine STATIC
  openalengine.h
  openalengine.cpp
  )

target_link_libraries( openalengine PUBLIC ${OPENAL_LIBRARY} )

