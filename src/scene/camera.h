#pragma once

#include "core/math/vec3.h"

namespace gfx
{
    // Tipo de proyección soportado por la cámara.
    enum class ProjectionType
    {
        Perspective,
        Orthographic
    };

    // Cámara básica de la escena.
    // Posicion Default de la camara
    struct Camera
    {
        Vec3 eye = { 0.0f, 0.0f, 5.0f };
        Vec3 target = { 0.0f, 0.0f, 0.0f };
        Vec3 up = { 0.0f, 1.0f, 0.0f };

        ProjectionType projectionType = ProjectionType::Perspective;

        // Parámetros de proyección en perspectiva.
        float fovYRadians = 1.0471975512f; // ~60 grados
        float nearPlane = 0.1f;
        float farPlane = 1000.0f;

        // Parámetros de proyección ortográfica.
        float left = -1.0f;
        float right = 1.0f;
        float bottom = -1.0f;
        float top = 1.0f;

        // Regresa la dirección de visión de la cámara normalizada.
        Vec3 forward() const
        {
            return normalize(target - eye);
        }
    };
}