#pragma once

#include <cmath>
#include <cstdint>

namespace gfx
{
    // Constantes básicas de uso general.
    constexpr float kPi = 3.14159265358979323846f;
    constexpr float kEpsilon = 1e-6f;

    // Limita un valor a un rango [minValue, maxValue].
    template<typename T>
    inline constexpr T clamp(const T& value, const T& minValue, const T& maxValue)
    {
        return (value < minValue) ? minValue : ((value > maxValue) ? maxValue : value);
    }

    // Limita un valor al rango [0, 1].
    // útil para color e iluminación.
    inline constexpr float saturate(float value)
    {
        return clamp(value, 0.0f, 1.0f);
    }

    // Interpolación lineal entre dos escalares.
    inline constexpr float lerp(float a, float b, float t)
    {
        return a + (b - a) * t;
    }

    // Compara dos flotantes con tolerancia.
    inline constexpr bool nearlyEqual(float a, float b, float epsilon = kEpsilon)
    {
        return (a > b ? a - b : b - a) <= epsilon;
    }

    // Convierte grados a radianes.
    inline float toRadians(float degrees)
    {
        return degrees * (kPi / 180.0f);
    }

    // Convierte radianes a grados.
    inline float toDegrees(float radians)
    {
        return radians * (180.0f / kPi);
    }

    // Redondea un flotante al entero más cercano.
    inline int roundToInt(float value)
    {
        return static_cast<int>(std::lround(value));
    }

    // Convierte un valor flotante [0,1] a byte [0,255].
    // Útil para empaquetar colores.
    inline uint8_t floatToByte(float value)
    {
        const float clamped = saturate(value);
        return static_cast<uint8_t>(clamped * 255.0f + 0.5f);
    }
}