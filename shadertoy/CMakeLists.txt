
include_directories(
 ${PROJECT_SOURCE_DIR}
 ${PROJECT_SOURCE_DIR}/extern
)

add_library( shadertoy STATIC
  shadertoyengine.h
  shadertoyengine.cpp
  shadertoyshaders.h
  shadertoyshaders.cpp
  shaderprogram.h
  shaderprogram.cpp
  framebuffer.h
  defaultframebuffer.h
  defaultframebuffer.cpp
  #framebufferrendertotexture.h
  #framebufferrendertotexture.cpp
  shaderaudiotexture.h
  shaderaudiotexture.cpp
  )

target_link_libraries( shadertoy PUBLIC OpenGL::GL audioprocessing glfw )
if( MSVC )
  target_include_directories( shadertoy PUBLIC 
    ${GLEW_INCLUDE_DIR}
	${glm_DIR}/../../ # TODO
  )
  target_link_libraries( shadertoy PUBLIC ${GLEW_LIBRARIES} )  
endif()

install( DIRECTORY shaders DESTINATION ./ )
if( MSVC )
  install( FILES "${GLEW_INCLUDE_DIR}/../bin/Release/x64/glew32.dll" DESTINATION bin )
endif()

