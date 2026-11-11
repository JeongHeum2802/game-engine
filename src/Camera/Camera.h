#pragma once

#include "Math/Transform.h"

class Camera
{
private:
    Vec3 position { 0.0f, 0.0f, 3.0f };
    Vec3 target   { 0.0f, 0.0f, 0.0f };
    Vec3 worldUp  { 0.0f, 1.0f, 0.0f };

    float fov = 45.0f * 3.14159265358979323846f / 180.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;

public:
    Camera() = default;

    void SetPosition(const Vec3& value)
    {
        position = value;
    }

    void SetTarget(const Vec3& value)
    {
        target = value;
    }

    Mat4 GetViewMatrix() const
    {
        return Transform::LookAt(
            position,
            target,
            worldUp
        );
    }

    Mat4 GetProjectionMatrix(float aspect) const
    {
        return Transform::Perspective(
            fov,
            aspect,
            nearPlane,
            farPlane
        );
    }
};
