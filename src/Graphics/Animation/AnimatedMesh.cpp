#include "AnimatedMesh.h"

AnimatedMesh::AnimatedMesh(
    const std::vector<AnimatedVertex>& vertices,
    const std::vector<unsigned int>& indices
)
{
    indexCount = static_cast<GLsizei>(indices.size());

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(AnimatedVertex),
        vertices.data(),
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(unsigned int),
        indices.data(),
        GL_STATIC_DRAW
    );

    // location 0: Position (vec3)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE,
        sizeof(AnimatedVertex),
        (void*)offsetof(AnimatedVertex, position)
    );

    // location 1: Normal (vec3)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE,
        sizeof(AnimatedVertex),
        (void*)offsetof(AnimatedVertex, normal)
    );

    // location 2: UV (vec2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2, 2, GL_FLOAT, GL_FALSE,
        sizeof(AnimatedVertex),
        (void*)offsetof(AnimatedVertex, texCoord)
    );

    // location 3: Bone IDs (ivec4)
    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(
        3, 4, GL_INT,
        sizeof(AnimatedVertex),
        (void*)offsetof(AnimatedVertex, boneIDs)
    );

    // location 4: Weights (vec4)
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(
        4, 4, GL_FLOAT, GL_FALSE,
        sizeof(AnimatedVertex),
        (void*)offsetof(AnimatedVertex, weights)
    );

    glBindVertexArray(0);
}

AnimatedMesh::~AnimatedMesh()
{
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

void AnimatedMesh::Draw() const
{
    glBindVertexArray(VAO);

    glDrawElements(
        GL_TRIANGLES,
        indexCount,
        GL_UNSIGNED_INT,
        nullptr
    );

    glBindVertexArray(0);
}
