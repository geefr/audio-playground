#ifndef FRAMEBUFFERRENDERTOTEXTURE_H
#define FRAMEBUFFERRENDERTOTEXTURE_H

#include "framebuffer.h"

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

/**
 * A framebuffer/render target for Engine
 */
class FrameBufferRenderToTexture final : public FrameBuffer
{
  public:
    /**
     * Initialise the framebuffer/texture
     * TODO: Hardcoded texture format here, rgba32
     * TODO: Hardcoded filtering here/etc
     *
     * @param width width of framebuffer
     * @param height height of framebuffer
     * @throws std::runtime_exception if initialisation fails
     */
    FrameBufferRenderToTexture(uint32_t width, uint32_t height);
    virtual ~FrameBufferRenderToTexture() final override;

    GLuint textureID() const;

    /// Set the render target to this framebuffer
    virtual void activate() const override final;
    virtual void resolve() const override final;
    virtual float width() const override final;
    virtual float height() const override final;

  private:
    GLuint mFramebuffer;
    GLuint mResolveFramebuffer;
    GLuint mTexture;
    GLuint mResolveTexture;
    GLuint mDepthBuffer;
    GLuint mResolveDepthBuffer;
    GLint mWidth;
    GLint mHeight;
};

#endif
