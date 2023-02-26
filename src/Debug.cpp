#include <gl/glew.h>
#include <cstdio>
#include "Debug.h"
#include "Shader.h"

void Debug::log(const char* format, ...)
{
    va_list argList;
    va_start(argList, format);
    logV(0, format, argList);
    va_end(argList);
}

void Debug::log(int verbosity, const char* format, ...)
{
    va_list argList;
    va_start(argList, format);
    logV(verbosity, format, argList);
    va_end(argList);
}

void Debug::logV(const char* format, va_list args)
{
    logV(0, format, args);
}

void Debug::logV(int verbosity, const char* format, va_list args)
{
    if (Debug::Verbosity >= verbosity)
    {
        const int MAX_CHARS = 1024;
        static char buffer[MAX_CHARS];
        
        char buf[1024]; sprintf_s(buf, "%s\n", format);
        vsnprintf(buffer, MAX_CHARS, buf, args);
        printf(buffer);
    }
}

static unsigned int s_vao;
static Shader* s_debugShader;

void Debug::drawLine(glm::vec3 start, glm::vec3 end, glm::mat4 vp, Color color)
{
    if (s_debugShader == nullptr)
    {
        s_debugShader = new Shader{ "./assets/Debug.vert", "./assets/Debug.frag" };
        
        glGenVertexArrays(1, &s_vao);
        glBindVertexArray(s_vao);

        unsigned int vbo;
        glGenBuffers(1, &vbo);
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6, nullptr, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    }

    float vertices[] = 
    { 
        start.x, start.y, start.z,
        end.x, end.y, end.z
    };

    s_debugShader->use();
    s_debugShader->setVec3("color", color);
    s_debugShader->setMat4("transform", vp);

    glBindVertexArray(s_vao);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 6, vertices);
    glDrawArrays(GL_LINES, 0, 2);
}

void Debug::drawRay(glm::vec3 start, glm::vec3 direction, glm::mat4 vp, Color color)
{
    drawLine(start, start + direction, vp, color);
}
