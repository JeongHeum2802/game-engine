
#include "Texture2D.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <limits>
#include <stdexcept>

Texture2D::Texture2D(
    const unsigned char* data,
    size_t size
)
{
    if (!data ||
        size > static_cast<size_t>(std::numeric_limits<int>::max()))
    {
        throw std::runtime_error("Invalid image data");
    }

    int width = 0;
    int height = 0;
    int channels = 0;

    // GLB의 UV 방향에 맞춰 이미지 행을 뒤집지 않음
    stbi_set_flip_vertically_on_load(0);

    unsigned char* pixels = stbi_load_from_memory(
        data,
        static_cast<int>(size),
        &width,
        &height,
        &channels,
        STBI_rgb_alpha
    );

    if (!pixels)
        throw std::runtime_error("Image decode failed");

    try
    {
        Upload(pixels, width, height, GL_RGBA);
    }
    catch (...)
    {
        stbi_image_free(pixels);
        throw;
    }

    stbi_image_free(pixels);
}

Texture2D::Texture2D(
    const unsigned char* pixels,
    int width,
    int height,
    GLenum format
)
{
    Upload(pixels, width, height, format);
}

void Texture2D::Upload(
    const unsigned char* pixels,
    int width,
    int height,
    GLenum format
)
{
    if (!pixels || width <= 0 || height <= 0)
        throw std::runtime_error("Invalid texture dimensions");

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA8,
        width,
        height,
        0,
        format,
        GL_UNSIGNED_BYTE,
        pixels
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_LINEAR_MIPMAP_LINEAR
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MAG_FILTER,
        GL_LINEAR
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_S,
        GL_REPEAT
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_T,
        GL_REPEAT
    );

    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::Bind(unsigned int slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

Texture2D::~Texture2D()
{
    if (textureID != 0)
        glDeleteTextures(1, &textureID);
}
