#pragma once

#include <gl/glew.h>

struct VertexAttributeData
{
public:
    size_t Size;
    size_t Count;
};

class Mesh
{
    typedef GLuint GLHandle;
public:
    Mesh(float vertices[], size_t vertexCount, VertexAttributeData vertexAttributes[], size_t attributeCount);
    void draw();

private:
    GLHandle m_vertexBuffer;
    GLHandle m_vertexArray;
    float *m_vertices;
    size_t m_vertexCount;
};

