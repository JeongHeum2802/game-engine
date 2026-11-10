
#pragma once

#include "Vec3.h"
#include "Vec4.h"
#include <cstddef>

class Mat4
{
private:
    float data[16] = {};

public:
    // 기본 생성자: 단위행렬
    Mat4()
    {
        for (int i = 0; i < 4; ++i)
            (*this)(i, i) = 1.0f;
    }

    // 행렬 원소 접근 (row, column)
    float& operator()(int row, int col)
    {
        return data[col * 4 + row];
    }

    const float& operator()(int row, int col) const
    {
        return data[col * 4 + row];
    }

    // OpenGL에 전달할 메모리 주소
    const float* Data() const
    {
        return data;
    }

    static Mat4 Identity()
    {
        return Mat4();
    }

    static Mat4 Zero()
    {
        Mat4 result;

        for (float& value : result.data)
            value = 0.0f;

        return result;
    }

    // 행렬 곱셈
    Mat4 operator*(const Mat4& rhs) const
    {
        Mat4 result = Mat4::Zero();

        for (int row = 0; row < 4; ++row)
        {
            for (int col = 0; col < 4; ++col)
            {
                for (int k = 0; k < 4; ++k)
                {
                    result(row, col) +=
                        (*this)(row, k) * rhs(k, col);
                }
            }
        }

        return result;
    }

    Mat4& operator*=(const Mat4& rhs)
    {
        *this = *this * rhs;
        return *this;
    }

    // 위치 벡터 변환 (w = 1)
    Vec3 TransformPoint(const Vec3& v) const
    {
        float x =
            (*this)(0,0) * v.x +
            (*this)(0,1) * v.y +
            (*this)(0,2) * v.z +
            (*this)(0,3);

        float y =
            (*this)(1,0) * v.x +
            (*this)(1,1) * v.y +
            (*this)(1,2) * v.z +
            (*this)(1,3);

        float z =
            (*this)(2,0) * v.x +
            (*this)(2,1) * v.y +
            (*this)(2,2) * v.z +
            (*this)(2,3);

        return { x, y, z };
    }

    // 방향 벡터 변환 (w = 0)
    Vec3 TransformDirection(const Vec3& v) const
    {
        return {
            (*this)(0,0) * v.x +
            (*this)(0,1) * v.y +
            (*this)(0,2) * v.z,

            (*this)(1,0) * v.x +
            (*this)(1,1) * v.y +
            (*this)(1,2) * v.z,

            (*this)(2,0) * v.x +
            (*this)(2,1) * v.y +
            (*this)(2,2) * v.z
        };
    }

    Vec4 operator*(const Vec4& v) const
    {
        return {
            (*this)(0,0)*v.x + (*this)(0,1)*v.y +
            (*this)(0,2)*v.z + (*this)(0,3)*v.w,

            (*this)(1,0)*v.x + (*this)(1,1)*v.y +
            (*this)(1,2)*v.z + (*this)(1,3)*v.w,

            (*this)(2,0)*v.x + (*this)(2,1)*v.y +
            (*this)(2,2)*v.z + (*this)(2,3)*v.w,

            (*this)(3,0)*v.x + (*this)(3,1)*v.y +
            (*this)(3,2)*v.z + (*this)(3,3)*v.w
        };
    }
};


