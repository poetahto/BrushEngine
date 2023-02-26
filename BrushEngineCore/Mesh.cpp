#include "Mesh.h"
#include "Debug.h"

Mesh::Mesh(float vertices[], size_t vertexCount, VertexAttributeData vertexAttributes[], size_t attributeCount) :
    m_vertices {vertices},
    m_vertexCount {vertexCount},
    m_vertexBuffer {},
    m_vertexArray {}
{
    // Count the number of floats-per-vertex
    size_t floatsPerVertex { 0 };

    for (size_t i = 0; i < attributeCount; i++)
        floatsPerVertex += vertexAttributes[i].Count;

    // Generate the buffers that will store mesh data on the GPU.
    glGenBuffers(1, &m_vertexBuffer);
    glGenVertexArrays(1, &m_vertexArray);

    // Activate the vertex array so we can cache all this setup and easily reapply.
    glBindVertexArray(m_vertexArray);

    // Upload the vertex data into the buffer.
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (floatsPerVertex * vertexCount), m_vertices, GL_STATIC_DRAW);

    size_t offset { 0 };

    for (int i = 0; i < attributeCount; i++)
    {
        VertexAttributeData current = vertexAttributes[i];
        glVertexAttribPointer(i, static_cast<int>(current.Count), GL_FLOAT, GL_FALSE, static_cast<int>(floatsPerVertex * current.Size), reinterpret_cast<void*>(offset * current.Size));
        glEnableVertexAttribArray(i);
        offset += current.Count;
    }

    // Deactivate the vertex array, since we don't need to start drawing right away.
    glBindVertexArray(0);
}

void Mesh::draw()
{
    glBindVertexArray(m_vertexArray);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(m_vertexCount));
}
