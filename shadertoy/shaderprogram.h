#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#ifdef WIN32
# include <GL/glew.h>
#else
# define GL_GLEXT_PROTOTYPES
# include <GL/gl.h>
# include <GL/glext.h>
#endif

#include <vector>
#include <string>
#include <map>

class ShaderProgram
{
public:
struct Shader
{
    GLenum type;
    std::string fileName;
    GLuint shader;
};

struct Attribute
{
    GLint index;
    std::string name;
};

struct Uniform
{
    GLint index;
    std::string name;
};

ShaderProgram();
~ShaderProgram();

void addShader( GLenum shaderType, std::string shaderFileName );
void addShaderFromSrc( GLenum shaderType, std::string shaderSource, std::string shaderName );

/// Compile/link the shader
void init() const;

/// Compile/link if needed, and return the shader program
GLuint id() const;

/// Perform attribute lookup
void regAttribute(std::string attributeName, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* ptr);

/// Perform uniform lookup
void regUniform(std::string uniformName);

GLint attribute(std::string attributeName);
GLint uniform(std::string uniformName);

private:
std::vector<Shader> m_shaders;
mutable GLuint m_program = 0;
std::map<std::string, Attribute> m_attributes;
std::map<std::string, Uniform> m_uniforms;
};
#endif // SHADERPROGRAM_H
