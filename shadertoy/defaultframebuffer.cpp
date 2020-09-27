
#include "defaultframebuffer.h"

#ifdef WIN32
# include <GL/glew.h>
#else
# define GL_GLEXT_PROTOTYPES
# include <GL/gl.h>
# include <GL/glext.h>
#endif


DefaultFrameBuffer::DefaultFrameBuffer() {}
DefaultFrameBuffer::~DefaultFrameBuffer() {}
void DefaultFrameBuffer::activate() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
void DefaultFrameBuffer::resolve() const {}
