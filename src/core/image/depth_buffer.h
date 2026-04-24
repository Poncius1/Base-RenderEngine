#pragma once

#include <vector>
#include <algorithm>
#include <limits>

namespace gfx
{
    // Z-buffer.
    // Guarda la profundidad más cercana escrita en cada pixel.
    class DepthBuffer
    {
    public:
        DepthBuffer() = default;

        DepthBuffer(int width, int height)
        {
            resize(width, height);
        }

        // Cambia el tamaño del buffer y lo limpia con profundidad infinita.
        void resize(int width, int height)
        {
            m_width = (width < 0) ? 0 : width;
            m_height = (height < 0) ? 0 : height;
            m_values.assign(static_cast<size_t>(m_width * m_height), defaultClearValue());
        }

        // Limpia el buffer con el valor de profundidad especificado.
        void clear(float depthValue = defaultClearValue())
        {
            std::fill(m_values.begin(), m_values.end(), depthValue);
        }

        // Verifica si la coordenada está dentro de rango.
        bool inBounds(int x, int y) const
        {
            return x >= 0 && x < m_width && y >= 0 && y < m_height;
        }

        // Regresa la profundidad almacenada.
        // Si está fuera de rango, regresa infinito.
        float get(int x, int y) const
        {
            if (!inBounds(x, y))
            {
                return defaultClearValue();
            }

            return m_values[static_cast<size_t>(y * m_width + x)];
        }

        // Escribe la profundidad si la coordenada es válida.
        void set(int x, int y, float depthValue)
        {
            if (!inBounds(x, y))
            {
                return;
            }

            m_values[static_cast<size_t>(y * m_width + x)] = depthValue;
        }

        // Prueba de profundidad básica.
        // Si el nuevo valor es menor, significa que está más cerca.
        // En ese caso actualiza el buffer y regresa true.
        bool testAndSet(int x, int y, float depthValue)
        {
            if (!inBounds(x, y))
            {
                return false;
            }

            const size_t index = static_cast<size_t>(y * m_width + x);

            if (depthValue < m_values[index])
            {
                m_values[index] = depthValue;
                return true;
            }

            return false;
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
            return m_values.empty();
        }

        static constexpr float defaultClearValue()
        {
            return std::numeric_limits<float>::infinity();
        }

    private:
        int m_width = 0;
        int m_height = 0;
        std::vector<float> m_values;
    };
}