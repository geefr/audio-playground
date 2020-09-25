#ifndef ENGINE_H
#define ENGINE_H

#include <AL/al.h>
#include <AL/alc.h>

#include <string>
#include <vector>
#include <memory>
#include <list>

/// The sound engine
class Engine {

  // Wrapper types around OpenAL
  // For now these are just handles/not required, may be expanded on later
  struct Buffer {
    ALuint id = 0;
  };
  struct Source {
    ALuint id = 0;
  };

  public:
    Engine();
    ~Engine();

    /// Allocate a buffer and upload the provided data
    std::shared_ptr<Buffer> createBuffer( uint8_t numChannels, uint32_t sampleRate, const int16_t* data, size_t dataSize );

    /// Create a source (Which can play data from a buffer)
    std::shared_ptr<Source> createSource();

    /// Bind a buffer to a source
    void bindBufferToSource(std::shared_ptr<Source>& source, std::shared_ptr<Buffer>& buffer);

    /// Play a source, and wait until it finishes playing
    /// @note Calling this for looping sources is not recommended
    void playSourceAndWait(std::shared_ptr<Source>& source);

  private:
    /// Open the (default) device, initialise OpenAL
    void openDevice();

    /// Close the device, shutdown OpenAL
    void closeDevice();
    
    /// Check for OpenAL errors
    /// @throws std::runtime_error on error
    void checkError(std::string msg);

    ALCdevice* mDevice = nullptr;
    ALCcontext* mContext = nullptr;

    std::list<std::shared_ptr<Buffer>> mBuffers;
    std::list<std::shared_ptr<Source>> mSources;
};

#endif
