#pragma once

#include "../Math/Vec3.h"

struct DirectionalLight
{
    Vec3 direction = Vec3(-1.0f, -1.0f, -1.0f);
    Vec3 color = Vec3(1.0f, 1.0f, 1.0f);

    float intensity = 1.0f;
};

struct PointLight
{
    Vec3 position = Vec3(0.0f, 0.0f, 0.0f);
    Vec3 color = Vec3(1.0f, 1.0f, 1.0f);

    float intensity = 1.0f;

    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
};