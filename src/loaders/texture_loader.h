#pragma once

#include <string>

#include "scene/texture.h"

namespace gfx
{
    // Cargador de texturas basado en archivos de imagen.
    // Convierte una imagen externa a la representación interna Texture.
    class TextureLoader
    {
    public:
        // Carga una textura desde archivo.
        // Si falla, regresa una textura vacía.
        static Texture loadFromFile(const std::string& filePath);
    };
}