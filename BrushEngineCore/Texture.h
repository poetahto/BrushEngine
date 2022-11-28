#pragma once

#include <gl/glew.h>
#include <unordered_map>

struct TextureSettings
{
    const char* path;
    int wrapModeS = GL_REPEAT;
    int wrapModeT = GL_REPEAT;
    int filterDownscale = GL_LINEAR_MIPMAP_LINEAR;
    int filterUpscale = GL_LINEAR;
    int colorFormat = GL_RGB;
    int colorFormatDataType = GL_UNSIGNED_BYTE;
};

class Texture
{
public:
    Texture(TextureSettings settings);
    ~Texture();

    void applyTo(int textureUnit);

private:
    unsigned int m_handle;
};