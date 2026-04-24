#pragma once

namespace gfx
{
    // Vector 4D.
    // Se usa sobre todo para coordenadas homogéneas y transformaciones con Mat4.
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

    // Suma componente a componente.
    inline constexpr Vec4 operator+(const Vec4& a, const Vec4& b)
    {
        return { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
    }

    // Resta componente a componente.
    inline constexpr Vec4 operator-(const Vec4& a, const Vec4& b)
    {
        return { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
    }

    // Multiplicación por escalar.
    inline constexpr Vec4 operator*(const Vec4& v, float s)
    {
        return { v.x * s, v.y * s, v.z * s, v.w * s };
    }

    inline constexpr Vec4 operator*(float s, const Vec4& v)
    {
        return v * s;
    }

    // División por escalar.
    inline constexpr Vec4 operator/(const Vec4& v, float s)
    {
        return { v.x / s, v.y / s, v.z / s, v.w / s };
    }
}