#include "Texture.h"
#include "Debug.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture(TextureSettings settings)
{
    // Create our new handle for the texture, stored in OpenGL.
    glGenTextures(1, &m_handle);
    
    // Bind to start modifying parameters
    glBindTexture(GL_TEXTURE_2D, m_handle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, settings.wrapModeS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, settings.wrapModeT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, settings.filterDownscale);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, settings.filterUpscale);

    // Actually load image data from disk and bind it to our texture.
    int width, height, nrChannels;
    unsigned char* data = stbi_load(settings.path, &width, &height, &nrChannels, 0);

    if (data)
    {
        Debug::log("Successfully loaded texture\n");
        glTexImage2D(GL_TEXTURE_2D, 0, settings.colorFormat, width, height, 0, settings.colorFormat, settings.colorFormatDataType, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        Debug::log("Failed to load texture\n");
    }

    // Clean up data, now that OpenGL is tracking it.
    stbi_image_free(data);
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_handle);
}

void Texture::bindTo(int textureUnit)
{
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, m_handle);
}