
# Locate system libraries
find_package( OpenGL REQUIRED )

include_directories(
 ${PROJECT_SOURCE_DIR}
)

add_library( shadertoy STATIC
  shadertoyengine.h
  shadertoyengine.cpp
  shaderprogram.h
  shaderprogram.cpp
  framebuffer.h
  defaultframebuffer.h
  defaultframebuffer.cpp
  #framebufferrendertotexture.h
  #framebufferrendertotexture.cpp
  )

target_link_libraries( shadertoy PUBLIC OpenGL::GL )

