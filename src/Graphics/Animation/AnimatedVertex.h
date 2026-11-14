#pragma once

#include <array>

#include "Math/Vec2.h"
#include "Math/Vec3.h"

constexpr int MAX_BONE_INFLUENCE = 4;

struct AnimatedVertex
{
    Vec3 position{};
    Vec3 normal{};
    Vec2 texCoord{};

    std::array<int, MAX_BONE_INFLUENCE> boneIDs{
        -1, -1, -1, -1
    };

    std::array<float, MAX_BONE_INFLUENCE> weights{
        0.0f, 0.0f, 0.0f, 0.0f
    };
};
