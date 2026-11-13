#pragma once

#include <cmath>
#include "Math/Vec3.h"

struct Ray
{
    Vec3 origin;
    Vec3 direction;

    bool IntersectHorizontalPlane(
        float planeY,
        Vec3& hitPoint
    ) const
    {
        constexpr float EPSILON = 0.00001f;

        // 평면과 평행한 경우
        if (std::abs(direction.y) < EPSILON)
            return false;

        float t = (planeY - origin.y) / direction.y;

        // 카메라 뒤쪽에 교차점이 있는 경우
        if (t < 0.0f)
            return false;

        hitPoint = origin + direction * t;

        return true;
    }
};
