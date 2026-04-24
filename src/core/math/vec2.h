#pragma once

#include <cmath>

namespace gfx
{
    // Vector 2D base.
    // Se usa principalmente para coordenadas UV, posiciones 2D e interpolación.
    struct Vec2
    {
        float x = 0.0f;
        float y = 0.0f;

        constexpr Vec2() = default;
        constexpr Vec2(float xValue, float yValue) : x(xValue), y(yValue) {}
    };

    // Suma componente a componente.
    inline constexpr Vec2 operator+(const Vec2& a, const Vec2& b)
    {
        return { a.x + b.x, a.y + b.y };
    }

    // Resta componente a componente.
    inline constexpr Vec2 operator-(const Vec2& a, const Vec2& b)
    {
        return { a.x - b.x, a.y - b.y };
    }

    // Multiplicación por escalar.
    inline constexpr Vec2 operator*(const Vec2& v, float s)
    {
        return { v.x * s, v.y * s };
    }

    inline constexpr Vec2 operator*(float s, const Vec2& v)
    {
        return v * s;
    }

    // División por escalar.
    inline constexpr Vec2 operator/(const Vec2& v, float s)
    {
        return { v.x / s, v.y / s };
    }

    inline Vec2& operator+=(Vec2& a, const Vec2& b)
    {
        a.x += b.x;
        a.y += b.y;
        return a;
    }

    inline Vec2& operator-=(Vec2& a, const Vec2& b)
    {
        a.x -= b.x;
        a.y -= b.y;
        return a;
    }

    inline Vec2& operator*=(Vec2& v, float s)
    {
        v.x *= s;
        v.y *= s;
        return v;
    }

    inline Vec2& operator/=(Vec2& v, float s)
    {
        v.x /= s;
        v.y /= s;
        return v;
    }

    // Producto punto.
    inline constexpr float dot(const Vec2& a, const Vec2& b)
    {
        return a.x * b.x + a.y * b.y;
    }

    // Magnitud al cuadrado.
    inline float lengthSquared(const Vec2& v)
    {
        return dot(v, v);
    }

    // Magnitud del vector.
    inline float length(const Vec2& v)
    {
        return std::sqrt(lengthSquared(v));
    }

    // Regresa un vector normalizado.
    // Si la longitud es muy pequeña, regresa el vector cero.
    inline Vec2 normalize(const Vec2& v)
    {
        const float len = length(v);
        if (len <= 1e-8f)
        {
            return {};
        }

        return v / len;
    }

    // Interpolación lineal entre dos vectores.
    inline constexpr Vec2 lerp(const Vec2& a, const Vec2& b, float t)
    {
        return a + (b - a) * t;
    }
}