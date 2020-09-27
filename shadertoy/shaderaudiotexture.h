#ifndef SHADERAUDIOTEXTURE_H
#define SHADERAUDIOTEXTURE_H

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

#include <memory>

class Audio;

/**
 * Texture for audio channel input, following the shadertoy spec (As close as I can, it's not clearly documented anywhere?)
 * Texture is a 2D texture, 2 pixels high containing:
 * - x : Amplitude channel. Row at y == 0.75 is signal ampYou can litude at time interval (roughly time since last frame), in red channel
 * - y : FFT channel. Row at y == 0.25 is FFT amplitude for bucket, in red channel. Buckets scaled over 0 <= x <= 1
 * - Texture reads should be linear interpolated, so reading values other than 0.25/0.75 result in smooth interpolation
 * - Texture should be wrapped at edges
 * - blue/green/alpha channels are 0.0f
 */
class ShaderAudioTexture
{
public:
  /**
   * Constructor
   * When constructed the texture will immediately allocate OpenGL resources,
   */
  ShaderAudioTexture();
  ~ShaderAudioTexture();

  /// The texture ID
  GLuint id() const;

  /**
   * Bind the texture to the specified texture unit
   */
  void bind( GLenum target ) const;

  /**
   * Update the texture contents to match the provided audio
   * @param audio The audio data to upload
   * @param startT Start time of the frame
   * @param endT End time of the frame
   */
  void setAudio( Audio& audio, float startT, float endT );

  // TODO: cleanups
  void setAudioFFT( Audio& audio, float startT, float endT );

  uint32_t width() const;
  uint32_t height() const;
  float playbackTime() const;

private:
  GLuint mId = 0;
  std::unique_ptr<float[]> mBuffer;
  const uint32_t mWidth = 512;
  const uint32_t mHeight = 2;
  float mPlaybackTime = 0.f;
  float mFFtScale = 0.f;

  std::unique_ptr<float[]> mFFTData;
  std::unique_ptr<float[]> mFFTDataLast;
};

#endif
