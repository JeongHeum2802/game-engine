 #pragma once

 #include <cmath>

 struct Vec3
 {
  float x = 0.0f;
  float y = 0.0f;
  float z = 0.0f;

  Vec3() = default;

  Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

  Vec3 operator+(const Vec3& rhs) const
  {
    return Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
  }

  Vec3 operator-(const Vec3& rhs) const
  {
    return Vec3(x - rhs.x, y - rhs.y, z - rhs.z);
  }

  Vec3 operator*(float scalar) const
  {
    return Vec3(x * scalar, y * scalar, z * scalar);
  }

   Vec3 operator/(float scalar) const
  {
        return Vec3(x / scalar, y / scalar, z / scalar);
  }

  Vec3& operator+=(const Vec3& rhs)
  {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
  }

  Vec3& operator-=(const Vec3& rhs)
  {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
  }

  Vec3& operator*=(float scalar)
  {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
  }

  Vec3& operator/=(float scalar)
  {
    x /= scalar;
    y /= scalar;
    z /= scalar;
    return *this;
  }

  // 내적
  float Dot(const Vec3& rhs) const
  {
    return x * rhs.x + y * rhs.y + z * rhs.z;
  }

  // 외적
  Vec3 Cross(const Vec3& rhs) const
  {
    return Vec3(
      y * rhs.z - z * rhs.y,
      z * rhs.x - x * rhs.z,
      x * rhs.y - y * rhs.x
    );
  }

  float Length() const
  {
      return std::sqrt(Dot(*this));
  }

  Vec3 Normalized() const
  {
      float length = Length();

      if (length < 1e-6f)
          return {};

      return *this / length;
  }
};