#pragma once

#include "core/math/mat4.h"
#include "core/math/vec3.h"

namespace gfx
{
    // Transformación afín básica de un objeto en la escena.
    // traslación, rotación y escala.
    struct Transform
    {
        Vec3 position = { 0.0f, 0.0f, 0.0f };
        Vec3 rotation = { 0.0f, 0.0f, 0.0f }; // En radianes.
        Vec3 scale = { 1.0f, 1.0f, 1.0f };

        // Construye la matriz de modelo del objeto.
        // Esta matriz convierte coordenadas locales a coordenadas de mundo.
        Mat4 toMatrix() const
        {
            return makeTRS(position, rotation, scale);
        }
    };
}