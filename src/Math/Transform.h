
#pragma once

#include "Mat4.h"
#include <cmath>

class Transform {
public:
    // 이동 행렬
    static Mat4 Translate(float x, float y, float z) {
        Mat4 result = Mat4::Identity();

        result(0, 3) = x;
        result(1, 3) = y;
        result(2, 3) = z;

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

    static Mat4 Scale(float x, float y, float z) {
        Mat4 result = Mat4::Identity();

        result(0, 0) = x;
        result(1, 1) = y;
        result(2, 2) = z;

        return result;
    }
};
