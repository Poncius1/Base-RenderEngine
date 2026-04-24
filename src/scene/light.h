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

    // Fuente de luz básica.
    // Se puede usar como luz direccional o puntual dependiendo del tipo.
    struct Light
    {
        LightType type = LightType::Directional;
        bool enabled = true;

        // Para luces direccionales.
        Vec3 direction = { 0.0f, -1.0f, -1.0f };

        // Para luces puntuales.
        Vec3 position = { 0.0f, 0.0f, 0.0f };

        // Intensidad/color de la luz.
        Color intensity = colors::White();
    };
}