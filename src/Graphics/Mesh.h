#pragma once

#include <glad/glad.h>
#include <vector>
#include <memory>

#include "Vertex.h"
#include "Texture2D.h"

struct MaterialData
{
    Vec3 baseColor = Vec3(1.0f, 1.0f, 1.0f);
    std::shared_ptr<Texture2D> baseColorTexture;
};

class Mesh
{
public:
    Mesh(const std::vector<Vertex>& vertices,
         const std::vector<unsigned int>& indices,
         MaterialData material = {});

    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    void Draw() const;

    const MaterialData& GetMaterial() const { return material; }

private:
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;

    GLsizei indexCount = 0;

    MaterialData material;
};
