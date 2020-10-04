#ifndef SHADERTOYENGINE_H
#define SHADERTOYENGINE_H

#include <iostream>
#include <exception>
#include <stdexcept>
#include <fstream>
#include <vector>
#include <chrono>
#include <map>
#include <memory>

#ifdef WIN32
# include <GL/glew.h>
#else
# define GL_GLEXT_PROTOTYPES
# include <GL/gl.h>
# include <GL/glext.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include "shaderprogram.h"
#include "framebuffer.h"
#include "defaultframebuffer.h"
#include "shaderaudiotexture.h"

/**
 * Graphics engine providing ability to display ShaderToy shaders (https://www.shadertoy.com/)
 */
class ShaderToyEngine
{
public:
    /**
     * Engine constructor
     */
    ShaderToyEngine();

    /**
     * Initialise the engine
     * @param shaderDir Directory containing shaders (Shader toy compatible, containing a mainImage function)
     */
    void init(std::string shaderDir);

    /// Generic vertex definition for graphical shaders
    struct VertexDef
    {
        vec3 coord;
        vec2 texCoord;

        bool operator == ( const VertexDef& other ) { return coord == other.coord &&
                                                            texCoord == other.texCoord; }
    };

    /// Data storage for a render pass
    struct RenderPass
    {
        GLuint vao;
        GLuint vbo;
        GLuint numVerts = 0;
    };

    /**
     * Update the engine/scene graph state
     *
     * Must be called once per application loop
     * Must be called before render (assuming you want to update positions every render)
     */
    void update();

    /// The time delta used in the last update() call
    float updateDelta() const;

    /**
     * Render the scene
     *
     * If no framebuffer is provided will render to the default framebuffer (The screen)
     */
    void render(float width, float height, const FrameBuffer* framebuffer = nullptr);
    void render(const FrameBuffer* framebuffer);

    /**
     * Render a single line
     * @note This method is slow, intended for debugging/a few lines here and there
     */
    void renderLineImmediate( vec3 start, vec3 end, vec4 colour, int thicknessPx, bool depthTestEnabled = true );

    [[noreturn]] static void quit(std::string msg);

    /// MSAA enable
    void MSAA(bool enable);
    /// Enable/Disable depth testing
    void depthTest(bool enable);

    /// Enable/Disable alpha blending
    /// Default enabled, src alpha/1 - src alpha
    /// TODO: Allow different blend functions here, but we probably won't ever use them
    void alphaBlending(bool enable);

    /// Clear colour
    /// Default black
    void clearColor(glm::vec4 c);

    /// The time since the engine was started
    float secondsSinceInit() const;

    /// Width/Height of the engine's default framebuffer
    float width() const;
    float height() const;

    /// The shaders available to the engine
    std::map<std::string, std::shared_ptr<ShaderProgram>> mShaders;

    /**
     * Change the active shader
     * @param toy One of the shaders from mShaders
     */
    void activeShaderToy( std::shared_ptr<ShaderProgram> toy );
    void activateShaderToy( std::string shaderName );

    /// Switch to the next shader/effect
    void nextShader();

    /**
     * The input channels/textures to the shader
     * If more than 4 are added the extras will be ignored
     * TODO: For now these are hardcoded to all be audio, image/other inputs not possible yet
     */
    std::vector<std::shared_ptr<ShaderAudioTexture>>& audioTextures();
private:
    void checkGlError();
    GLuint loadTexture( std::string fileName );
    void setVertexAttribPointers(GLint vertCoordAttrib, GLint texCoordAttrib, GLint vertColorAttrib);
    void printMat4x4(mat4x4 mat, std::string name);

    void initRenderPasses();
    void renderPassDisplay();

    bool mEnableDepthTest = true;
    bool mEnableAlpha = true;
    vec4 mClearColor = {0.f,0.f,0.f,1.f};
    bool mMSAA = true;
    float mWidth = 0.f;
    float mHeight = 0.f;

    std::chrono::time_point<std::chrono::high_resolution_clock> mTimeStart;
    std::chrono::time_point<std::chrono::high_resolution_clock> mTimeCurrent;
    float mTimeDelta = 0.f;
    uint32_t mFrame = 0;

    vec4 mOrthoSpace = { -10.f,10.f,-10.f,10.f };

    DefaultFrameBuffer mDefaultFramebuffer;

    mat4 mProjectionMatrix = mat4(1.0);

    RenderPass mRenderPassDisplay;
    GLuint vao;

    std::vector<std::shared_ptr<ShaderAudioTexture>> mAudioTextures;

    std::shared_ptr<ShaderProgram> mActiveShaderToy;
};

#endif
