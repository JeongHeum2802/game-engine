#pragma once

#include <glad/glad.h>
#include <vector>

class Mesh
{
public:
    Mesh(const std::vector<float>& vertices,
         const std::vector<unsigned int>& indices);

    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    void Draw() const;

private:
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;

    GLsizei indexCount = 0;
};
