#pragma once

#include <cstdint>
#include <vector>
#include <algorithm>

#include "core/image/color.h"

namespace gfx
{
    // Framebuffer de color.
    // Almacena un arreglo lineal de pixeles en formato ARGB de 32 bits.
    // Es el buffer principal donde el renderer escribe la imagen final.
    class PixelBuffer
    {
    public:
        PixelBuffer() = default;

        PixelBuffer(int width, int height)
        {
            resize(width, height);
        }

        // Cambia el tamaño del buffer y reinicializa su memoria.
        void resize(int width, int height)
        {
            m_width = std::max(0, width);
            m_height = std::max(0, height);
            m_pixels.assign(static_cast<size_t>(m_width * m_height), 0u);
        }

        // Limpia todo el framebuffer con un color empaquetado.
        void clear(uint32_t argb)
        {
            std::fill(m_pixels.begin(), m_pixels.end(), argb);
        }

        // Limpia todo el framebuffer con un Color flotante.
        void clear(const Color& color)
        {
            clear(packColorARGB(color));
        }

        // Escribe un pixel si está dentro de los límites del buffer.
        void setPixel(int x, int y, uint32_t argb)
        {
            if (!inBounds(x, y))
            {
                return;
            }

            m_pixels[static_cast<size_t>(y * m_width + x)] = argb;
        }

        // Sobrecarga para escribir usando Color flotante.
        void setPixel(int x, int y, const Color& color)
        {
            setPixel(x, y, packColorARGB(color));
        }

        // Regresa el pixel empaquetado en la posición solicitada.
        // Si está fuera de rango, regresa 0.
        uint32_t getPixel(int x, int y) const
        {
            if (!inBounds(x, y))
            {
                return 0u;
            }

            return m_pixels[static_cast<size_t>(y * m_width + x)];
        }

        // Verifica si una coordenada está dentro del buffer.
        bool inBounds(int x, int y) const
        {
            return x >= 0 && x < m_width && y >= 0 && y < m_height;
        }

        int width() const
        {
            return m_width;
        }

        int height() const
        {
            return m_height;
        }

        bool empty() const
        {
            return m_pixels.empty();
        }

        const uint32_t* data() const
        {
            return m_pixels.data();
        }

        uint32_t* data()
        {
            return m_pixels.data();
        }

        const std::vector<uint32_t>& pixels() const
        {
            return m_pixels;
        }

        std::vector<uint32_t>& pixels()
        {
            return m_pixels;
        }

    private:
        int m_width = 0;
        int m_height = 0;
        std::vector<uint32_t> m_pixels;
    };
}