#pragma once

namespace gfx
{
    // Vector 4D.
    // coordenadas homogéneas y transformaciones con Mat4
    struct Vec4
    {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float w = 0.0f;

        constexpr Vec4() = default;
        constexpr Vec4(float xValue, float yValue, float zValue, float wValue)
            : x(xValue), y(yValue), z(zValue), w(wValue) {
        }
    };

    //suma componente a componente
    inline constexpr Vec4 operator+(const Vec4& a, const Vec4& b)
    {
        return { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
    }

    //resta componente a componente
    inline constexpr Vec4 operator-(const Vec4& a, const Vec4& b)
    {
        return { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
    }

    //multiplicación por escalar
    inline constexpr Vec4 operator*(const Vec4& v, float s)
    {
        return { v.x * s, v.y * s, v.z * s, v.w * s };
    }

    inline constexpr Vec4 operator*(float s, const Vec4& v)
    {
        return v * s;
    }

    //división por escalar
    inline constexpr Vec4 operator/(const Vec4& v, float s)
    {
        return { v.x / s, v.y / s, v.z / s, v.w / s };
    }
}