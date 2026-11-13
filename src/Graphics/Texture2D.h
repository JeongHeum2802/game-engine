#pragma once

#include <glad/glad.h>
#include <cstddef>

class Texture2D
{
public:
    // PNG/JPEG 등 이미지 데이터가 메모리에 있는 경우
    Texture2D(const unsigned char* data, size_t size);

    // 이미 디코딩된 RGBA 이미지
    Texture2D(
        const unsigned char* pixels,
        int width,
        int height,
        GLenum format
    );

    ~Texture2D();

    Texture2D(const Texture2D&) = delete;
    Texture2D& operator=(const Texture2D&) = delete;

    void Bind(unsigned int slot = 0) const;

private:
    GLuint textureID = 0;

    void Upload(
        const unsigned char* pixels,
        int width,
        int height,
        GLenum format
    );
};
