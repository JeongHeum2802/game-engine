#pragma once

#include <vector>
#include <memory>
#include "Mesh.h"

class Primitive
{
public:
    static std::unique_ptr<Mesh> CreateQuad()
    {
        std::vector<Vertex> vertices = {
            // Position                 Normal               UV
            {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
            {{ 0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
            {{ 0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
            {{-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}
        };

        std::vector<unsigned int> indices = {
            0, 1, 2,
            0, 2, 3
        };

        return std::make_unique<Mesh>(vertices, indices);
    }

    // Cube
    static std::unique_ptr<Mesh> CreateCube()
    {
        std::vector<Vertex> vertices = {
            // Front (+Z)
            {{-0.5f, -0.5f,  0.5f}, {0, 0, 1}, {0, 0}},
            {{ 0.5f, -0.5f,  0.5f}, {0, 0, 1}, {1, 0}},
            {{ 0.5f,  0.5f,  0.5f}, {0, 0, 1}, {1, 1}},
            {{-0.5f,  0.5f,  0.5f}, {0, 0, 1}, {0, 1}},

            // Back (-Z)
            {{ 0.5f, -0.5f, -0.5f}, {0, 0,-1}, {0, 0}},
            {{-0.5f, -0.5f, -0.5f}, {0, 0,-1}, {1, 0}},
            {{-0.5f,  0.5f, -0.5f}, {0, 0,-1}, {1, 1}},
            {{ 0.5f,  0.5f, -0.5f}, {0, 0,-1}, {0, 1}},

            // Left (-X)
            {{-0.5f, -0.5f, -0.5f}, {-1, 0, 0}, {0, 0}},
            {{-0.5f, -0.5f,  0.5f}, {-1, 0, 0}, {1, 0}},
            {{-0.5f,  0.5f,  0.5f}, {-1, 0, 0}, {1, 1}},
            {{-0.5f,  0.5f, -0.5f}, {-1, 0, 0}, {0, 1}},

            // Right (+X)
            {{ 0.5f, -0.5f,  0.5f}, {1, 0, 0}, {0, 0}},
            {{ 0.5f, -0.5f, -0.5f}, {1, 0, 0}, {1, 0}},
            {{ 0.5f,  0.5f, -0.5f}, {1, 0, 0}, {1, 1}},
            {{ 0.5f,  0.5f,  0.5f}, {1, 0, 0}, {0, 1}},

            // Top (+Y)
            {{-0.5f,  0.5f,  0.5f}, {0, 1, 0}, {0, 0}},
            {{ 0.5f,  0.5f,  0.5f}, {0, 1, 0}, {1, 0}},
            {{ 0.5f,  0.5f, -0.5f}, {0, 1, 0}, {1, 1}},
            {{-0.5f,  0.5f, -0.5f}, {0, 1, 0}, {0, 1}},

            // Bottom (-Y)
            {{-0.5f, -0.5f, -0.5f}, {0,-1, 0}, {0, 0}},
            {{ 0.5f, -0.5f, -0.5f}, {0,-1, 0}, {1, 0}},
            {{ 0.5f, -0.5f,  0.5f}, {0,-1, 0}, {1, 1}},
            {{-0.5f, -0.5f,  0.5f}, {0,-1, 0}, {0, 1}}
        };

        std::vector<unsigned int> indices;
        indices.reserve(36);

        // 각 면의 정점 4개로 삼각형 2개 생성
        for (unsigned int i = 0; i < 6; ++i)
        {
            unsigned int start = i * 4;

            indices.push_back(start + 0);
            indices.push_back(start + 1);
            indices.push_back(start + 2);

            indices.push_back(start + 0);
            indices.push_back(start + 2);
            indices.push_back(start + 3);
        }

        return std::make_unique<Mesh>(vertices, indices);
    }

    // Sphere
    static std::unique_ptr<Mesh> CreateSphere(
        float radius = 0.5f,
        unsigned int sectors = 32,
        unsigned int stacks = 16
    )
    {
        if (radius <= 0.0f || sectors < 3 || stacks < 2)
        {
            throw std::invalid_argument(
                "Invalid sphere parameters"
            );
        }

        constexpr float PI = 3.14159265358979323846f;

        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        vertices.reserve((stacks + 1) * (sectors + 1));
        indices.reserve(6 * sectors * (stacks - 1));

        // 정점 생성
        for (unsigned int i = 0; i <= stacks; ++i)
        {
            float stackAngle =
                PI / 2.0f - PI * i / stacks;

            float xy = radius * std::cos(stackAngle);
            float z = radius * std::sin(stackAngle);

            for (unsigned int j = 0; j <= sectors; ++j)
            {
                float sectorAngle =
                    2.0f * PI * j / sectors;

                float x = xy * std::cos(sectorAngle);
                float y = xy * std::sin(sectorAngle);

                Vec3 position(x, y, z);
                Vec3 normal = position.Normalized();

                Vec2 texCoord(
                    static_cast<float>(j) / sectors,
                    static_cast<float>(i) / stacks
                );

                vertices.push_back({
                    position,
                    normal,
                    texCoord
                });
            }
        }

        // 삼각형 인덱스 생성
        for (unsigned int i = 0; i < stacks; ++i)
        {
            unsigned int k1 = i * (sectors + 1);
            unsigned int k2 = k1 + sectors + 1;

            for (unsigned int j = 0; j < sectors; ++j)
            {
                if (i != 0)
                {
                    indices.push_back(k1 + j);
                    indices.push_back(k1 + j + 1);
                    indices.push_back(k2 + j);
                }

                if (i != stacks - 1)
                {
                    indices.push_back(k1 + j + 1);
                    indices.push_back(k2 + j + 1);
                    indices.push_back(k2 + j);
                }
            }
        }

        return std::make_unique<Mesh>(vertices, indices);
    }
};