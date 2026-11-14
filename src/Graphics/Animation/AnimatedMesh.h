#pragma once

#include <glad/glad.h>
#include <cstddef>
#include <vector>

#include "AnimatedVertex.h"

class AnimatedMesh
{
public:
    AnimatedMesh(
        const std::vector<AnimatedVertex>& vertices,
        const std::vector<unsigned int>& indices
    );

    ~AnimatedMesh();

    AnimatedMesh(const AnimatedMesh&) = delete;
    AnimatedMesh& operator=(const AnimatedMesh&) = delete;

    void Draw() const;

private:
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;

    GLsizei indexCount = 0;
};
