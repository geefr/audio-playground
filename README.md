# Audio Playground

A little area to play with sound processing, OpenAL, and such

Executables can be found under tests, should be fairly self explanatory

# Building
Should be a fairly straightfoward CMake build. Repo uses submodules under extern/ so fetch those before building.

The following dependencies are needed:
* Bundled deps - 'git submodule update --init --recursive' to fetch
* OpenGL
* glm
* glfw3 3.3+
* GLEW (When building on Windows)

Linux
* Targetting GCC 8 or above, C++20 where available
* glm and glfw are linked from system directories

Windows
* Targetting Visual Studio 2019, C++20
* glm, glfw, glew are linked from external directories, CMake project should walk you through the setup

# Licence
* Unless specified otherwise in folder/file everything is covered by the BSD licence
* extern/dr_libs: Public domain
* extern/kissfft: BSD/Unilicence
* extern/openal-soft: LGPL
* extern/imgui: MIT
* shadertoy/shaders: Shaders taken from shadertoy.com, mostly creative commons, or BSD. See header of files.
* music: Various licences, see Readme.md in each folder

Many thanks to the authors of these dependencies, just let me know if there's any usage issues.

