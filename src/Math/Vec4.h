#pragma once

#include "Vec3.h"

struct Vec4
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 0.0f;

    Vec4() = default;

    Vec4(float x, float y, float z, float w)
        : x(x), y(y), z(z), w(w) {}

    Vec4(const Vec3& v, float w)
        : x(v.x), y(v.y), z(v.z), w(w) {}

    Vec4 operator+(const Vec4& rhs) const
    {
        return {
            x + rhs.x,
            y + rhs.y,
            z + rhs.z,
            w + rhs.w
        };
    }

    Vec4 operator*(float scalar) const
    {
        return {
            x * scalar,
            y * scalar,
            z * scalar,
            w * scalar
        };
    }
};
