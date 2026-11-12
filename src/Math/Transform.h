#pragma once

#include "Mat4.h"
#include "Vec3.h"
#include <cmath>

class Transform {
public:
    // 이동 행렬
    static Mat4 Translate(const Vec3& vec3) {
        Mat4 result = Mat4::Identity();

        result(0, 3) = vec3.x;
        result(1, 3) = vec3.y;
        result(2, 3) = vec3.z;

        return result;
    }

    // X축 회전
    static Mat4 RotateX(float angle) {
        Mat4 result = Mat4::Identity();

        float c = std::cos(angle);
        float s = std::sin(angle);

        result(1, 1) = c;
        result(1, 2) = -s;
        result(2, 1) = s;
        result(2, 2) = c;

        return result;
    }

    // Y축 회전
    static Mat4 RotateY(float angle) {
        Mat4 result = Mat4::Identity();

        float c = std::cos(angle);
        float s = std::sin(angle);

        result(0, 0) = c;
        result(0, 2) = s;
        result(2, 0) = -s;
        result(2, 2) = c;

        return result;
    }

    // Z축 회전
    static Mat4 RotateZ(float angle) {
        Mat4 result = Mat4::Identity();

        float c = std::cos(angle);
        float s = std::sin(angle);

        result(0, 0) = c;
        result(0, 1) = -s;
        result(1, 0) = s;
        result(1, 1) = c;

        return result;
    }

    static Mat4 Scale(const Vec3& vec3) {
        Mat4 result = Mat4::Identity();

        result(0, 0) = vec3.x;
        result(1, 1) = vec3.y;
        result(2, 2) = vec3.z;

        return result;
    }

    static Mat4 Perspective(
        float fov,
        float aspect,
        float nearPlane,
        float farPlane) {
        float tanHalfFov = std::tan(fov * 0.5f);

        Mat4 result = Mat4::Zero();

        result(0, 0) = 1.0f / (aspect * tanHalfFov);
        result(1, 1) = 1.0f / tanHalfFov;

        result(2, 2) =
            -(farPlane + nearPlane) /
            (farPlane - nearPlane);

        result(2, 3) =
            -(2.0f * farPlane * nearPlane) /
            (farPlane - nearPlane);

        result(3, 2) = -1.0f;

        return result;
    }

    static Mat4 LookAt(
        const Vec3 &eye,
        const Vec3 &target,
        const Vec3 &worldUp) 
    {
        Vec3 forward = (target - eye).Normalized();
        Vec3 right = forward.Cross(worldUp).Normalized();
        Vec3 up = right.Cross(forward);

        Mat4 result = Mat4::Identity();

        // 카메라의 회전
        result(0, 0) = right.x;
        result(0, 1) = right.y;
        result(0, 2) = right.z;

        result(1, 0) = up.x;
        result(1, 1) = up.y;
        result(1, 2) = up.z;

        result(2, 0) = -forward.x;
        result(2, 1) = -forward.y;
        result(2, 2) = -forward.z;

        // 카메라 위치의 역변환
        result(0, 3) = -right.Dot(eye);
        result(1, 3) = -up.Dot(eye);
        result(2, 3) = forward.Dot(eye);

        return result;
    }
};
