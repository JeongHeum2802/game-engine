#pragma once

#include <cmath>
#include "Math/Vec3.h"

// XZ 평면에서 광선과 원의 충돌 판정
// direction은 정규화된 벡터여야 함
inline bool RaycastCircleXZ(
    const Vec3& origin,
    const Vec3& direction,
    const Vec3& center,
    float radius,
    float maxDistance,
    float& hitDistance
)
{
    float ox = origin.x - center.x;
    float oz = origin.z - center.z;

    float dx = direction.x;
    float dz = direction.z;

    // |(O + tD) - C|^2 = r^2
    float b = ox * dx + oz * dz;
    float c = ox * ox + oz * oz - radius * radius;

    float discriminant = b * b - c;

    if (discriminant < 0.0f)
        return false;

    float root = std::sqrt(discriminant);

    float t = -b - root;

    // 원 안에서 발사한 경우 출구 교차점 사용
    if (t < 0.0f)
        t = -b + root;

    if (t < 0.0f || t > maxDistance)
        return false;

    hitDistance = t;
    return true;
}
