#pragma once

#include "core/image/color.h"
#include "core/math/vec3.h"

namespace gfx
{
    // Tipo de luz soportado por el renderer.
    enum class LightType
    {
        Directional,
        Point
    };

    // Fuente de luz básica,se puede usar como luz direccional o puntual dependiendo del tipo
    struct Light
    {
        LightType type = LightType::Directional;
        bool enabled = true;

        //para luces direccionales
        Vec3 direction = { 0.0f, -1.0f, -1.0f };

        //para luces puntuales
        Vec3 position = { 0.0f, 0.0f, 0.0f };

        //intensidad/color de la luz
        Color intensity = colors::White();
    };
}