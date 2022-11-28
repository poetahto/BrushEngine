#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <assert.h>
#include <sstream>
#include <fstream>
#include <vector>

#include "Shader.h"
#include "Debug.h"

using namespace glm;

// todo: wrap shader attachment / linking errors with logging
Shader::Shader(const char *vertexPath, const char *fragmentPath)
{
    // Create the vertex and fragment shaders from files on the disk.

    unsigned int vertexShader = createShader(vertexPath, GL_VERTEX_SHADER);
    unsigned int fragmentShader = createShader(fragmentPath, GL_FRAGMENT_SHADER);

    // Link the loaded vertex and fragment shaders into a usable program.

    m_handle = glCreateProgram();
    glAttachShader(m_handle, vertexShader);
    glAttachShader(m_handle, fragmentShader);
    glLinkProgram(m_handle);

    // Free the shaders from memory - we only need the program now after linking.

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::~Shader()
{
    glDeleteProgram(m_handle);
}

// todo: wrap file read errors with logging
// todo: wrap shader compilation errors with logging
unsigned int Shader::createShader(const char *path, GLenum type)
{
    std::ifstream fileStream;
    std::stringstream stringStream;

    // Read the raw text data from a file on the disk.

    fileStream.open(path);
    stringStream << fileStream.rdbuf();
    fileStream.close();

    std::string source = stringStream.str();
    const char* sourcePtr = source.c_str();

    // Feed the text data to OpenGL to compile.

    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &sourcePtr, nullptr);
    glCompileShader(shader);

    GLint isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);

    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

        debug::log("Failure to compile shader %s: %s", path, errorLog.begin());
    }

    else debug::log("Successfully compiled shader %s.", path);

    return shader;
}

void Shader::use()
{
    glUseProgram(m_handle);
    s_currentHandle = m_handle;
}

void Shader::setFloat(const char *name, float value)
{
    assert(s_currentHandle == m_handle && "Shader must be active before changing values.");
    glUniform1f(glGetUniformLocation(m_handle, name), value);
}

void Shader::setInt(const char *name, int value)
{
    assert(s_currentHandle == m_handle && "Shader must be active before changing values.");
    glUniform1i(glGetUniformLocation(m_handle, name), value);
}

void Shader::setBool(const char *name, bool value)
{
    assert(s_currentHandle == m_handle && "Shader must be active before changing values.");
    glUniform1i(glGetUniformLocation(m_handle, name), static_cast<int>(value));
}

void Shader::setMat4(const char *name, mat4 value)
{
    assert(s_currentHandle == m_handle && "Shader must be active before changing values.");
    glUniformMatrix4fv(glGetUniformLocation(m_handle, name), 1, GL_FALSE, value_ptr(value));
}

void Shader::setVec3(const char *name, glm::vec3 value)
{
    assert(s_currentHandle == m_handle && "Shader must be active before changing values.");
    glUniform3f(glGetUniformLocation(m_handle, name), value.x, value.y, value.z);
}

void Shader::setVec3(const char *name, Color value)
{
    assert(s_currentHandle == m_handle && "Shader must be active before changing values.");
    glUniform3f(glGetUniformLocation(m_handle, name), value.r, value.g, value.b);
}

unsigned int Shader::s_currentHandle{ 0 };