cmake_minimum_required(VERSION 3.10)

project(audio-playground)

# Compiler/Tool requirements
set( CMAKE_CXX_STANDARD 20 )
set( OpenGL_GL_PREFERENCE GLVND )

# Locate system libraries
find_package( OpenGL REQUIRED )
find_package( OpenAL REQUIRED )
find_package( glm REQUIRED )

if( MSVC )
	find_package( glew REQUIRED )

	# TODO: Hack - The binary glfw releases don't include a cmake config and windows cmake doesn't have a FindGLFW.cmake
	# For now just include it as source
	if(NOT GLFW_SOURCE_PATH )
	message( FATAL_ERROR "You're building on windows, set GLFW_SOURCE_PATH to the location of glfw3's source")
	endif()
	set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
	set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
	set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
	add_subdirectory( ${GLFW_SOURCE_PATH} ${CMAKE_CURRENT_BINARY_DIR}/glfw3 )
else()
	find_package(glfw3 REQUIRED)
	# TODO: Assuming GCC, will have flag mismatch with clang
	set( CMAKE_CXX_FLAGS "-Werror=all -Wno-error=old-style-cast -Wno-error=unused-variable -Wno-error=unused-but-set-variable" )
endif()

add_subdirectory( extern/kissfft )

add_subdirectory( audioprocessing )
add_subdirectory( openalengine )
add_subdirectory( shadertoy )
add_subdirectory( tests )

