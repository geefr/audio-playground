
#include "shaderaudiotexture.h"

#include "audioloaders/audio.h"

#include <cstring> // memset

ShaderAudioTexture::ShaderAudioTexture() {
  glGenTextures(1, &mId);
  bind(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  mBuffer.reset(new float[mWidth * mHeight]);
}

ShaderAudioTexture::~ShaderAudioTexture() {}

GLuint ShaderAudioTexture::id() const { return mId; }

void ShaderAudioTexture::bind( GLenum target ) const {
  glBindTexture(target, mId);
}

void ShaderAudioTexture::setAudio( Audio& audio, float startT, float endT ) {

  // TODO: Fill mBuffer with data here - need amplitude + fft in mWidth buckets
  for( auto y = 0u; y < mHeight; ++y )
    for( auto x = 0u; x < mWidth; ++x ) {
      mBuffer.get()[(y * mWidth) + x] = 0.5f;
    }

  bind(GL_TEXTURE_2D);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, mWidth, mHeight, 0, GL_RED, GL_FLOAT, mBuffer.get());

}

uint32_t ShaderAudioTexture::width() const { return mWidth; }
uint32_t ShaderAudioTexture::height() const { return mHeight; }
