#include "openalengine.h"
#include <stdexcept>
#include <iostream>

OpenALEngine::OpenALEngine() {
  openDevice();
}

OpenALEngine::~OpenALEngine() {
  closeDevice();
}

std::shared_ptr<OpenALEngine::Buffer> OpenALEngine::createBuffer( uint8_t numChannels, uint32_t sampleRate, const int16_t* data, size_t dataSize ) {
  ALenum format;

  if( numChannels == 1 ) format = AL_FORMAT_MONO16;
  else if( numChannels == 2 ) format = AL_FORMAT_STEREO16;
  //if( numChannels == 1 && bitsPerSample == 8 ) format = AL_FORMAT_MONO8;
  //else if( numChannels == 1 && bitsPerSample == 16) format = AL_FORMAT_MONO16;
  //else if( numChannels == 2 && bitsPerSample == 8) format = AL_FORMAT_STEREO8;
  //else if( numChannels == 2 && bitsPerSample == 16) format = AL_FORMAT_STEREO16;
  else throw std::runtime_error("Unable to create buffer");

  std::shared_ptr<Buffer> b(new Buffer());
  alGenBuffers(1, &b->id);
  checkError("Gen Buffers");

  alBufferData(b->id, format, data, dataSize, sampleRate);
  checkError("Buffer Data");

  mBuffers.emplace_back(b);
  return b;
}

std::shared_ptr<OpenALEngine::Buffer> OpenALEngine::createBuffer( Audio& audio ) {
  return createBuffer(
        audio.numChannels(),
        audio.sampleRate(),
        audio.data(),
        audio.dataSize()
  );
}

std::shared_ptr<OpenALEngine::Source> OpenALEngine::createSource() {
  std::shared_ptr<Source> s(new Source());

  // Allocate
  alGenSources(1, &s->id);
  checkError("Create source");

  // Source parameters
  alSourcef(s->id, AL_PITCH, 1);
  alSourcef(s->id, AL_GAIN, 1.f);
  alSource3f(s->id, AL_POSITION, 0, 0, 0);
  alSource3f(s->id, AL_VELOCITY, 0, 0, 0);
  alSourcei(s->id, AL_LOOPING, AL_FALSE);

  mSources.emplace_back(s);
  return s;
}

void OpenALEngine::bindBufferToSource(std::shared_ptr<Source>& source, std::shared_ptr<Buffer>& buffer) {
  alSourcei(source->id, AL_BUFFER, buffer->id);
  checkError("Bind buffer to source");
}

void OpenALEngine::playSourceAndWait(std::shared_ptr<Source>& source) {
  alSourcePlay(source->id);
  checkError("Play source and wait");
  
  ALint state = AL_PLAYING;
  while(state == AL_PLAYING) {
    alGetSourcei(source->id, AL_SOURCE_STATE, &state);
    checkError("Check source state");
  }
}

void OpenALEngine::playSource(std::shared_ptr<Source>& source) {
  alSourcePlay(source->id);
  checkError("Play source");
}

bool OpenALEngine::isSourcePlaying(std::shared_ptr<Source>& source) {
  ALint state = AL_PLAYING;
  alGetSourcei(source->id, AL_SOURCE_STATE, &state);
  checkError("hasSourceFinished");
  return state == AL_PLAYING;
}

float OpenALEngine::sourcePlaybackOffset(std::shared_ptr<Source>& source) {
  float res = 0.0f;
  alGetSourcef(source->id, AL_SEC_OFFSET, &res);
  return res;
}

void OpenALEngine::openDevice() {
  mDevice = alcOpenDevice(nullptr);
  if( !mDevice ) {
    throw std::runtime_error("Failed to create OpenAL device");
  }

  mContext = alcCreateContext(mDevice, nullptr);
  if( !mContext ) {
    throw std::runtime_error("Failed to create OpenAL context");
  }

  alcMakeContextCurrent(mContext);
  checkError("Make Context Current");
}

void OpenALEngine::closeDevice() {
  if( !mDevice || !mContext ) return;

  // Cleanup resources
  for( auto& s : mSources ) {
    alSourcei(s->id, AL_BUFFER, 0);
    alDeleteSources(1, &s->id);
  }
  for( auto& b : mBuffers ) alDeleteBuffers(1, &b->id);

  // Cleanup context
  alcMakeContextCurrent(nullptr);
  alcDestroyContext(mContext);

  // Deinitialise
  if( !alcCloseDevice(mDevice) ) {
    std::cerr << "WARNING: Failed to close OpenAL device" << std::endl;
  }
}

void OpenALEngine::checkError(std::string msg) {
  auto err = alGetError();
  switch(err) {
    case AL_NO_ERROR:
      break;
    case AL_INVALID_NAME:
      throw std::runtime_error("AL_INVALID_NAME: " + msg);
    case AL_INVALID_ENUM:
      throw std::runtime_error("AL_INVALID_ENUM: " + msg);
    case AL_INVALID_VALUE:
      throw std::runtime_error("AL_INVALID_VALUE: " + msg);
    case AL_INVALID_OPERATION:
      throw std::runtime_error("AL_INVALID_OPERATION: " + msg);
    case AL_OUT_OF_MEMORY:
      throw std::runtime_error("AL_OUT_OF_MEMORY: " + msg);
    default:
      throw std::runtime_error("AL: Unknown Error: " + msg);
  }
}
