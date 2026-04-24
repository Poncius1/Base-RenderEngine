#pragma once

#include <cstdint>

#include "core/math/vec3.h"
#include "core/math/math_utils.h"

namespace gfx
{
    // Color RGBA en formato flotante.
    // Se usa durante el pipeline para iluminación, materiales y texturas.
    // El rango esperado de cada componente es [0, 1].
    struct Color
    {
        float r = 0.0f;
        float g = 0.0f;
        float b = 0.0f;
        float a = 1.0f;

        constexpr Color() = default;

        constexpr Color(float red, float green, float blue, float alpha = 1.0f)
            : r(red), g(green), b(blue), a(alpha) {
        }
    };

    // Convierte un Vec3 a Color usando alpha = 1.
    inline constexpr Color colorFromVec3(const Vec3& value)
    {
        return { value.x, value.y, value.z, 1.0f };
    }

    // Convierte un Color a Vec3 ignorando alpha.
    inline constexpr Vec3 colorToVec3(const Color& color)
    {
        return { color.r, color.g, color.b };
    }

    // Regresa una versión saturada del color en el rango [0, 1].
    inline constexpr Color saturateColor(const Color& color)
    {
        return
        {
            saturate(color.r),
            saturate(color.g),
            saturate(color.b),
            saturate(color.a)
        };
    }

    // Empaqueta un color flotante RGBA a formato ARGB de 32 bits.
    // Este formato es conveniente para framebuffers tipo QImage::Format_ARGB32.
    inline uint32_t packColorARGB(const Color& color)
    {
        const Color clamped = saturateColor(color);

        const uint32_t a = static_cast<uint32_t>(floatToByte(clamped.a));
        const uint32_t r = static_cast<uint32_t>(floatToByte(clamped.r));
        const uint32_t g = static_cast<uint32_t>(floatToByte(clamped.g));
        const uint32_t b = static_cast<uint32_t>(floatToByte(clamped.b));

        return (a << 24) | (r << 16) | (g << 8) | b;
    }

    // Construye un Color desde bytes de 8 bits por canal.
    inline constexpr Color colorFromBytes(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
    {
        return
        {
            r / 255.0f,
            g / 255.0f,
            b / 255.0f,
            a / 255.0f
        };
    }

    // Colores frecuentes para limpiar o depurar buffers.
    namespace colors
    {
        inline constexpr Color Black() { return { 0.0f, 0.0f, 0.0f, 1.0f }; }
        inline constexpr Color White() { return { 1.0f, 1.0f, 1.0f, 1.0f }; }
        inline constexpr Color Red() { return { 1.0f, 0.0f, 0.0f, 1.0f }; }
        inline constexpr Color Green() { return { 0.0f, 1.0f, 0.0f, 1.0f }; }
        inline constexpr Color Blue() { return { 0.0f, 0.0f, 1.0f, 1.0f }; }
    }
}