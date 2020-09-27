
#include "defaultframebuffer.h"

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>


DefaultFrameBuffer::DefaultFrameBuffer() {}
DefaultFrameBuffer::~DefaultFrameBuffer() {}
void DefaultFrameBuffer::activate() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
void DefaultFrameBuffer::resolve() const {}
