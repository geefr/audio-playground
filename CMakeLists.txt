cmake_minimum_required(VERSION 3.10)

project(audio-playground)

if( NOT CMAKE_BUILD_TYPE AND NOT MSVC )
  set( CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE )
  set_property( CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Release" "Debug" "MinSizeRel" "RelWithDebInfo" )
endif()

# Compiler/Tool requirements
set( CMAKE_CXX_STANDARD 20 )
set( OpenGL_GL_PREFERENCE GLVND )

# Locate system libraries
find_package( OpenGL REQUIRED )
find_package( glm REQUIRED )

# Honestly could use binaries but it's just easier for deployment doing this
set(ALSOFT_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ALSOFT_UTILS OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_AMBDEC_PRESETS OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_CONFIG OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_HRTF_DATA OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_UTILS OFF CACHE BOOL "" FORCE)
set(ALSOFT_EMBED_HRTF_DATA OFF CACHE BOOL "" FORCE)
set(ALSOFT_BUILD_ROUTER ON CACHE BOOL "" FORCE)
add_subdirectory( ${PROJECT_SOURCE_DIR}/extern/openal-soft ${CMAKE_CURRENT_BINARY_DIR}/openal-soft )	
set(OPENAL_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/extern/openal-soft/include")
set(OPENAL_LIBRARY OpenAL)

if( MSVC )
	find_package( glew REQUIRED )
	if(NOT GLEW_INCLUDE_DIR OR
	   NOT GLEW_SHARED_LIBRARY_DEBUG OR
	   NOT GLEW_SHARED_LIBRARY_RELEASE )
	   message( FATAL_ERROR "GLEW Not found, please set GLEW_INCLUDE_DIR, GLEW_SHARED_LIBRARY_DEBUG, and GLEW_SHARED_LIBRARY_RELEASE")
	endif()

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

add_compile_definitions( IMGUI_IMPL_OPENGL_LOADER_CUSTOM="imgui_gl_loader.h" )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/extern )
add_subdirectory( extern )

add_subdirectory( audioprocessing )
add_subdirectory( openalengine )
add_subdirectory( shadertoy )
add_subdirectory( tests )

install( DIRECTORY music DESTINATION . )
install( FILES LICENSE DESTINATION . )
