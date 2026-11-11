#pragma once

#include <vector>
#include <memory>
#include "Mesh.h"

class Primitive
{
public:
    static std::unique_ptr<Mesh> CreateQuad()
    {
        std::vector<float> vertices = {
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
             0.5f,  0.5f, 0.0f,
            -0.5f,  0.5f, 0.0f
        };

        std::vector<unsigned int> indices = {
            0, 1, 2,
            0, 2, 3
        };

        return std::make_unique<Mesh>(vertices, indices);
    }

    // 정육면체: 한 변의 길이 1
    static std::unique_ptr<Mesh> CreateCube()
    {
        std::vector<float> vertices = {
            // 뒷면 (z = -0.5)
            -0.5f, -0.5f, -0.5f,
             0.5f, -0.5f, -0.5f,
             0.5f,  0.5f, -0.5f,
            -0.5f,  0.5f, -0.5f,

            // 앞면 (z = 0.5)
            -0.5f, -0.5f,  0.5f,
             0.5f, -0.5f,  0.5f,
             0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f
        };

        std::vector<unsigned int> indices = {
            // 뒷면
            0, 2, 1,
            0, 3, 2,

            // 앞면
            4, 5, 6,
            4, 6, 7,

            // 왼쪽
            0, 4, 7,
            0, 7, 3,

            // 오른쪽
            1, 2, 6,
            1, 6, 5,

            // 위쪽
            3, 7, 6,
            3, 6, 2,

            // 아래쪽
            0, 1, 5,
            0, 5, 4
        };

        return std::make_unique<Mesh>(vertices, indices);
    }

    // 구: UV Sphere 방식
    static std::unique_ptr<Mesh> CreateSphere(
        float radius = 0.5f,
        unsigned int sectors = 32,
        unsigned int stacks = 16
    )
    {
        if (radius <= 0.0f || sectors < 3 || stacks < 2)
            throw std::invalid_argument(
                "Invalid sphere parameters"
            );

        constexpr float PI = 3.14159265358979323846f;

        std::vector<float> vertices;
        std::vector<unsigned int> indices;

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

                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);
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