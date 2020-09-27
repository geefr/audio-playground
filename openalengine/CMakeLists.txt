
include_directories(
 ${PROJECT_SOURCE_DIR}
)

add_library( openalengine STATIC
  openalengine.h
  openalengine.cpp
  )

target_include_directories( openalengine PUBLIC ${OPENAL_INCLUDE_DIR} )

target_link_libraries( openalengine PUBLIC ${OPENAL_LIBRARY} )

