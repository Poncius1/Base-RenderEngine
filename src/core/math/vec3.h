#pragma once

#include <cmath>

namespace gfx
{
    // Vector 3D base.
    // Se usa para posiciones, direcciones, normales y colores RGB.
    struct Vec3
    {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;

        constexpr Vec3() = default;
        constexpr Vec3(float xValue, float yValue, float zValue)
            : x(xValue), y(yValue), z(zValue) {
        }
    };

    // Suma componente a componente.
    inline constexpr Vec3 operator+(const Vec3& a, const Vec3& b)
    {
        return { a.x + b.x, a.y + b.y, a.z + b.z };
    }

    // Resta componente a componente.
    inline constexpr Vec3 operator-(const Vec3& a, const Vec3& b)
    {
        return { a.x - b.x, a.y - b.y, a.z - b.z };
    }

    // Negación unaria.
    inline constexpr Vec3 operator-(const Vec3& v)
    {
        return { -v.x, -v.y, -v.z };
    }

    // Multiplicación por escalar.
    inline constexpr Vec3 operator*(const Vec3& v, float s)
    {
        return { v.x * s, v.y * s, v.z * s };
    }

    inline constexpr Vec3 operator*(float s, const Vec3& v)
    {
        return v * s;
    }

    // División por escalar.
    inline constexpr Vec3 operator/(const Vec3& v, float s)
    {
        return { v.x / s, v.y / s, v.z / s };
    }

    inline Vec3& operator+=(Vec3& a, const Vec3& b)
    {
        a.x += b.x;
        a.y += b.y;
        a.z += b.z;
        return a;
    }

    inline Vec3& operator-=(Vec3& a, const Vec3& b)
    {
        a.x -= b.x;
        a.y -= b.y;
        a.z -= b.z;
        return a;
    }

    inline Vec3& operator*=(Vec3& v, float s)
    {
        v.x *= s;
        v.y *= s;
        v.z *= s;
        return v;
    }

    inline Vec3& operator/=(Vec3& v, float s)
    {
        v.x /= s;
        v.y /= s;
        v.z /= s;
        return v;
    }

    // Producto punto.
    // Muy útil para iluminación, proyecciones y ángulos.
    inline constexpr float dot(const Vec3& a, const Vec3& b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    // Producto cruz.
    // Se usa para generar vectores perpendiculares y normales.
    inline constexpr Vec3 cross(const Vec3& a, const Vec3& b)
    {
        return
        {
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        };
    }

    // Magnitud al cuadrado.
    inline float lengthSquared(const Vec3& v)
    {
        return dot(v, v);
    }

    // Magnitud del vector.
    inline float length(const Vec3& v)
    {
        return std::sqrt(lengthSquared(v));
    }

    // Regresa un vector unitario.
    // Si la magnitud es casi cero, regresa el vector cero.
    inline Vec3 normalize(const Vec3& v)
    {
        const float len = length(v);
        if (len <= 1e-8f)
        {
            return {};
        }

        return v / len;
    }

    // Multiplicación componente a componente.
    // Muy útil para color * material o color * intensidad de luz.
    inline constexpr Vec3 hadamard(const Vec3& a, const Vec3& b)
    {
        return { a.x * b.x, a.y * b.y, a.z * b.z };
    }

    // Interpolación lineal entre dos vectores.
    inline constexpr Vec3 lerp(const Vec3& a, const Vec3& b, float t)
    {
        return a + (b - a) * t;
    }
}