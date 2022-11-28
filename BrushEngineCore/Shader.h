#pragma once

#include <gl/glew.h>
#include <glm/glm.hpp>
#include "Color.h"

class Shader 
{
public:
    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader();

    void use();

    void setFloat(const char *name, float value);
    void setInt(const char *name, int value);
    void setBool(const char *name, bool value);
    void setMat4(const char *name, glm::mat4 value);
    void setVec3(const char *name, glm::vec3 value);
    void setVec3(const char *name, Color value);

private:
    static unsigned int s_currentHandle;
    unsigned int m_handle;

    unsigned int createShader(const char *path, GLenum type);
};