#pragma once

#include "core/image/color.h"

namespace gfx
{
    // Material del renderer
    // Modela las componentes ambiente, difusa y especular
    // usadas por el sombreado tipo Phong
    struct Material
    {
        Color ambient = { 0.3f, 0.3f, 0.3f, 1.0f };
        Color diffuse = { 0.5f, 0.5f, 0.5f, 1.0f };
        Color specular = { 0.7f, 0.7f, 0.7f, 1.0f };

        float shininess = 32.0f;

        // textura y normal map
        bool useTexture = true;
        bool useNormalMap = false;
    };
}