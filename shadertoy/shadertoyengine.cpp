#include "shadertoyengine.h"
#include <functional>

void GLAPIENTRY
GLErrorCallback( [[maybe_unused]] GLenum source,
                  GLenum type,
                  [[maybe_unused]] GLuint id,
                  GLenum severity,
                  [[maybe_unused]] GLsizei length,
                  const GLchar* message,
                  [[maybe_unused]] const void* userParam )
{
  fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
  if( severity == GL_DEBUG_SEVERITY_HIGH )
  {
      ShaderToyEngine::quit("High severity error raised, terminating");
  }
}

//l/r/b/t
[[noreturn]] void ShaderToyEngine::quit(std::string msg)
{
    throw std::runtime_error(msg);
}

void ShaderToyEngine::checkGlError()
{
    auto error = glGetError();
    switch(error)
    {
        case GL_NO_ERROR: return;
        case GL_INVALID_ENUM: quit("GL_INVALID_ENUM");
        case GL_INVALID_VALUE: quit("GL_INVALID_VALUE");
        case GL_INVALID_OPERATION: quit("GL_INVALID_OPERATION");
        case GL_INVALID_FRAMEBUFFER_OPERATION: quit("GL_INVALID_FRAMEBUFFER_OPERATION");
        case GL_OUT_OF_MEMORY: quit("GL_OUT_OF_MEMORY");
        case GL_STACK_UNDERFLOW: quit("GL_STACK_UNDERFLOW");
        case GL_STACK_OVERFLOW: quit("GL_STACK_OVERFLOW");
    }
}

// GLuint Engine::loadTexture( std::string fileName )
// {
//     GLuint tex;
//     glGenTextures(1, &tex);
//     glActiveTexture(GL_TEXTURE0);
//     glBindTexture(GL_TEXTURE_2D, tex);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

//     // Load the image
//     raster::PNG loader(fileName);

//     GLenum pixFormat;
//     switch( loader.depth() )
//     {
//         case 24: pixFormat = GL_RGB; break;
//         case 32: pixFormat = GL_RGBA; break;
//         default: quit("Unsupported image depth: " + std::to_string(loader.depth()));
//     }

//     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<GLsizei>(loader.width()), static_cast<GLsizei>(loader.height()), 0, pixFormat, GL_UNSIGNED_BYTE, loader.data() );

//     // Generate mipmaps (After texture upload)
//     glGenerateMipmap(GL_TEXTURE_2D);
//     return tex;
// }

void ShaderToyEngine::setVertexAttribPointers(GLint vertCoordAttrib, GLint texCoordAttrib, GLint vertColorAttrib)
{
    glEnableVertexAttribArray(static_cast<GLuint>(vertCoordAttrib));
    glVertexAttribPointer(static_cast<GLuint>(vertCoordAttrib), 3, GL_FLOAT, GL_FALSE, sizeof(VertexDef), reinterpret_cast<void*>(offsetof(VertexDef,coord)));
    glEnableVertexAttribArray(static_cast<GLuint>(texCoordAttrib));
    glVertexAttribPointer(static_cast<GLuint>(texCoordAttrib), 2, GL_FLOAT, GL_FALSE, sizeof(VertexDef), reinterpret_cast<void*>(offsetof(VertexDef,texCoord)));
}

void ShaderToyEngine::printMat4x4(mat4x4 mat, std::string name)
{
    std::cerr << name << ":\n";
    for(auto col = 0; col < 3; ++col)
    {
        std::cerr << col << ": ";
        for(auto row = 0; row < 3; ++row)
        {
            std::cerr << "[" << mat[col][row] << "]";
        }
        std::cerr << std::endl;
    }
}

ShaderToyEngine::ShaderToyEngine( std::string shaderfile )
{
  // TODO: Load shadertoy src, inject into frag shader
}

void ShaderToyEngine::init()
{
#ifdef _WIN32
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
      throw std::runtime_error("Failed to initialised GLEW");
    }
#endif

    GLint majVer=0;
    GLint minVer=0;
    glGetIntegerv(GL_MAJOR_VERSION, &majVer);
    glGetIntegerv(GL_MINOR_VERSION, &minVer);

    if( majVer < 3 || (majVer == 3 && minVer < 3) )
    {
      throw std::runtime_error("ERROR: minimum of OpenGL 3.3 required");
    }

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(GLErrorCallback, nullptr);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    depthTest(true);
    alphaBlending(mEnableAlpha);
    clearColor(mClearColor);
    MSAA(mMSAA);

    ////////////////////////////////////////////////////////////

    // Create and bind a VAO
    // This one is used for global stuff, immediate renders, etc. Don't expect anything to stay bound here
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Find where the shaders and such are

    // Let's load some shaders
    // The shader used to display the final scene
    // TODO: need to load shaders here and stuff
    /*
    std::shared_ptr<ShaderProgram> display(new ShaderProgram());
    display->addShader(GL_VERTEX_SHADER, mDataDir + "shaders/display.vert");
    display->addShader(GL_FRAGMENT_SHADER, mDataDir + "shaders/display.frag");
    mShaders["display"] = display;
    display->init();

    std::shared_ptr<ShaderProgram> flat(new ShaderProgram());
    flat->addShader(GL_VERTEX_SHADER, mDataDir + "shaders/flatshading.vert");
    flat->addShader(GL_FRAGMENT_SHADER, mDataDir + "shaders/flatshading.frag");
    mShaders["flatshading"] = flat;
    flat->init();
*/
    initRenderPasses();

    mTimeStart = std::chrono::high_resolution_clock::now();
    mTimeCurrent = mTimeStart;
}

void ShaderToyEngine::update()
{
  std::chrono::time_point<std::chrono::high_resolution_clock> current = std::chrono::high_resolution_clock::now();
  mTimeDelta = ((double)(current - mTimeCurrent).count()) / 1.0e9;
  mTimeCurrent = current;
  // mScene->update(*this, current, delta);
}

void ShaderToyEngine::render(float width, float height, const FrameBuffer* framebuffer)
{
  if (framebuffer)
  {
    framebuffer->activate();
  }
  else
  {
    mWidth = width;
    mHeight = height;

    mDefaultFramebuffer.activate();

    // TODO: Hack, should move to defaultframebuffer, could remove this block then
    // TODO: Should be handling framebuffersizecallback for default framebuffer adjustments ;)
    glViewport(0, 0, width, height);
  }

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);


  // TODO: Do simulation/draw passes to update rendering data

  // Display the results to the display
  renderPassDisplay();

  /*
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, catTexture);
  glUniform1i(mShader.uniform("texture0"), 0);
  glUniform1i(mShader.uniform("enableTexture0"), false);
  */
  // mScene->node()->render(mViewMatrix, mProjectionMatrix);

  if (framebuffer) framebuffer->resolve();
}

void ShaderToyEngine::render(const FrameBuffer* framebuffer)
{
  render(framebuffer->width(), framebuffer->height(), framebuffer);
}

void ShaderToyEngine::renderLineImmediate( vec3 start, vec3 end, vec4 colour, int thicknessPx, bool depthTestEnabled ) {
  std::vector<VertexDef> v;
  v.push_back({start, {0.0, 0.0}});
  v.push_back({end, {0.0, 0.0}});
  auto numVerts = v.size();

  glBindVertexArray(vao);
  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(v.size() * sizeof(VertexDef)), v.data(), GL_STATIC_DRAW);

  auto shader = mShaders["flatshading"];
  glUseProgram(shader->id());

  shader->regAttribute("vertCoord", 3, GL_FLOAT, GL_FALSE, sizeof(VertexDef), reinterpret_cast<const void*>(offsetof(VertexDef,coord)));
  // If pointer != null then GL_ARRAY_BUFFER has to be bound
  shader->regAttribute("vertTexCoord", 2, GL_FLOAT, GL_FALSE, sizeof(VertexDef), reinterpret_cast<const void*>(offsetof(VertexDef,texCoord)));
  shader->regUniform("projectionMatrix");

  glUniformMatrix4fv(shader->uniform("projectionMatrix"), 1, GL_FALSE, value_ptr(mProjectionMatrix));

  if( !depthTestEnabled ) {
    glDisable(GL_DEPTH_TEST);
  }

  auto oldWidth = 1;
  glGetIntegerv(GL_LINE_WIDTH, &oldWidth);
  glLineWidth( thicknessPx );

  glDrawArrays(GL_LINES, 0, numVerts);

  glLineWidth(oldWidth);

  if( !depthTestEnabled ) {
    glEnable(GL_DEPTH_TEST);
  }

  glDeleteBuffers(1, &vbo);
}

void ShaderToyEngine::initRenderPasses() {
  // TODO: Simulation/update passes

  // Display pass
  glGenVertexArrays(1, &mRenderPassDisplay.vao);
  glBindVertexArray(mRenderPassDisplay.vao);
  glGenBuffers(1, &mRenderPassDisplay.vbo);
  auto displayShader = mShaders["display"];
  displayShader->id(); // Ensure shader has cached uniform IDs and otherwise initialised

  auto left = -1.f;
  auto right = 1.f;
  auto bottom = -1.f;
  auto top = 1.f;

  std::vector<VertexDef> displayVerts;
  displayVerts.push_back({{left, bottom, 0.0}, {0.0, 0.0}});
  displayVerts.push_back({{right, bottom, 0.0}, {1.0, 0.0}});
  displayVerts.push_back({{right, top, 0.0}, {1.0, 1.0}});

  displayVerts.push_back({{right, top, 0.0}, {1.0, 1.0}});
  displayVerts.push_back({{left, top, 0.0}, {0.0, 1.0}});
  displayVerts.push_back({{left, bottom, 0.0}, {0.0, 0.0}});
  mRenderPassDisplay.numVerts = displayVerts.size();

  glBindVertexArray(mRenderPassDisplay.vao);
  glBindBuffer(GL_ARRAY_BUFFER, mRenderPassDisplay.vbo);
  glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(displayVerts.size() * sizeof(VertexDef)), displayVerts.data(), GL_STATIC_DRAW );

  glUseProgram(displayShader->id());
  displayShader->regAttribute("vertCoord", 3, GL_FLOAT, GL_FALSE, sizeof(VertexDef), reinterpret_cast<const void*>(offsetof(VertexDef,coord)));
  // If pointer != null then GL_ARRAY_BUFFER has to be bound
  displayShader->regAttribute("vertTexCoord", 2, GL_FLOAT, GL_FALSE, sizeof(VertexDef), reinterpret_cast<const void*>(offsetof(VertexDef,texCoord)));

  displayShader->regUniform("projectionMatrix");
  displayShader->regUniform("numSpheres");
}

void ShaderToyEngine::renderPassDisplay() {
  auto shader = mShaders["display"];

  glBindVertexArray(mRenderPassDisplay.vao);
  glUseProgram(shader->id());
  // TODO: Bind shader resources (if needed, should be setup in vao already)

  glUniformMatrix4fv(shader->uniform("projectionMatrix"), 1, GL_FALSE, value_ptr(mProjectionMatrix));

//  glUniform1f(shader->uniform("numSpheres"), static_cast<float>(mGeometry.size()));
//  for( auto i = 0; i < mGeometry.size(); ++i ) {
//    auto& s = mGeometry[i];
//    glUniform4f(shader->uniform("spheres[" + std::to_string(i) + "]"),
//      s.origin.x,
//      s.origin.y,
//      s.origin.z,
//      s.radius);
//    glUniform4fv(shader->uniform("sphereDiffuseColour[" + std::to_string(i) + "]"), 1, value_ptr(s.diffuseColour));
//  }

  glDrawArrays(GL_TRIANGLES, 0, mRenderPassDisplay.numVerts);
}

// void Engine::changeScene( std::shared_ptr<Scene> scene ) {
//   mScene = scene;
// }

// std::shared_ptr<Scene> Engine::scene() const { return mScene; }

void ShaderToyEngine::depthTest(bool enable)
{
    mEnableDepthTest = enable;
    mEnableDepthTest ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
}

void ShaderToyEngine::alphaBlending(bool enable)
{
    mEnableAlpha = enable;
    if( mEnableAlpha )
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else
    {
        glDisable(GL_BLEND);
    }
}

void ShaderToyEngine::clearColor(vec4 c)
{
    mClearColor = c;
    glClearColor(c[0],c[1],c[2],c[3]);
}

void ShaderToyEngine::MSAA(bool enable)
{
    mMSAA = enable;
    if( mMSAA ) glEnable(GL_MULTISAMPLE);
    else glDisable(GL_MULTISAMPLE);
}

float ShaderToyEngine::secondsSinceInit() const
{
    auto since = mTimeCurrent - mTimeStart;
    float msSince = since / std::chrono::milliseconds(1);
    return msSince / 1000.0f;
}

float ShaderToyEngine::width() const { return mWidth; }
float ShaderToyEngine::height() const { return mHeight; }
float ShaderToyEngine::updateDelta() const { return mTimeDelta; }
