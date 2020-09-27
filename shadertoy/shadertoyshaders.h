#ifndef SHADERTOYSHADERS_H
#define SHADERTOYSHADERS_H

#include <string>
#include <map>

/**
 * This struct contains the various boilerplate shaders needed for shadertoy setup
 * Also contains some hardcoded body shaders, to be used unless/until shadertoy REST API support is added
 */
class ShaderToyShaders {
public:
  // Lazy singleton
  static ShaderToyShaders instance;

  /**
   * Load shadertoy shaders from disk
   * @param shaderDir Path to a folder containing one or more shaders
   * @return The shaders, in the form <Shader (file) Name, Fragment shader source>
   */
  std::map<std::string, std::string> loadShaders( std::string shaderDir );

  const std::string ShaderToyBoilerPlateVert = R"(#version 330
  in vec3 vertCoord;
  in vec2 vertTexCoord;

  out vec3 fragPos;
  out vec2 vUV;

  void main(void)
  {
      fragPos = vertCoord;
      vUV = vertTexCoord;

      gl_Position = vec4(vertCoord, 1.0);
  }
  )";

 const std::string ShaderToyBoilerPlateFragHeader = R"(#version 330

   in vec3 fragPos;
   in vec2 vUV;

   // Shadertoy uniforms
   uniform vec3      iResolution;           // viewport resolution (in pixels)
   uniform float     iTime;                 // shader playback time (in seconds)
   uniform float     iTimeDelta;            // render time (in seconds)
   uniform int       iFrame;                // shader playback frame
   uniform float     iChannelTime[4];       // channel playback time (in seconds)
   uniform vec4      iMouse;                // mouse pixel coords. xy: current (if MLB down), zw: click
   uniform vec4      iDate;                 // (year, month, day, time in seconds)
   uniform float     iSampleRate;           // sound sample rate (i.e., 44100)
   uniform vec3      iChannelResolution[4]; // Resolution of input channels
   uniform sampler2D iChannel0;             // Input channels (For audio in this case)

   out vec4 fragColor;
 )";

 const std::string ShaderToyBoilerPlateFragFooter = R"(
   void main(void) {
     mainImage(fragColor, vUV.st * iResolution.xy);
   }
 )";
};

#endif
