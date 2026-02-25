#pragma once

#include <vector>
#include <cstdint>
#include <algorithm>

/*
    Buffer simple de píxeles en memoria.
    Cada píxel usa formato ARGB32:
    0xAARRGGBB
*/

struct PixelBuffer
{
    int w = 0; // ancho
    int h = 0; // alto

    // Datos del buffer (fila por fila)
    std::vector<uint32_t> data;

    // Cambia el tamaño del buffer 
    void resize(int width, int height)
    {
        w = std::max(1, width);
        h = std::max(1, height);

        data.assign(size_t(w) * size_t(h), 0xFF000000u); // negro
    }

    // Limpia todo el buffer con un color (negro por defecto)
    void clear(uint32_t argb = 0xFF000000u)
    {
        std::fill(data.begin(), data.end(), argb);
    }

    // Verifica si una coordenada está dentro del buffer
    inline bool inBounds(int x, int y) const
    {
        return (x >= 0 && y >= 0 && x < w && y < h);
    }

    // Escribe un píxel en (x,y)
    inline void setPixel(int x, int y, uint32_t argb)
    {
        if (!inBounds(x, y)) return;

        data[size_t(y) * size_t(w) + size_t(x)] = argb;
    }
};
