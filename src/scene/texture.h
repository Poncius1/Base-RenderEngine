#pragma once

#include "core/image/pixel_buffer.h"
#include "core/math/vec2.h"
#include "core/math/math_utils.h"

namespace gfx
{
    // Textura basada en un PixelBuffer.
    // Permite muestrear color usando coordenadas UV.
    class Texture
    {
    public:
        Texture() = default;

        explicit Texture(const PixelBuffer& buffer)
            : m_buffer(buffer)
        {
        }

        // Asigna el contenido del buffer.
        void setBuffer(const PixelBuffer& buffer)
        {
            m_buffer = buffer;
        }

        const PixelBuffer& buffer() const
        {
            return m_buffer;
        }

        // Muestra la textura usando coordenadas UV.
        // UV esperadas en rango [0,1].
        Color sample(const Vec2& uv) const
        {
            if (m_buffer.empty())
            {
                return colors::White();
            }

            // Clamp UV
            const float u = saturate(uv.x);
            const float v = saturate(uv.y);

            const int x = static_cast<int>(u * (m_buffer.width() - 1));
            const int y = static_cast<int>((1.0f - v) * (m_buffer.height() - 1));

            const uint32_t argb = m_buffer.getPixel(x, y);

            return unpackColor(argb);
        }

    private:
        PixelBuffer m_buffer;

        // Convierte ARGB (uint32) a Color flotante.
        static Color unpackColor(uint32_t argb)
        {
            const float a = ((argb >> 24) & 0xFF) / 255.0f;
            const float r = ((argb >> 16) & 0xFF) / 255.0f;
            const float g = ((argb >> 8) & 0xFF) / 255.0f;
            const float b = (argb & 0xFF) / 255.0f;

            return { r, g, b, a };
        }
    };
}