#pragma once

#include "scene/material.h"
#include "scene/mesh.h"
#include "scene/texture.h"
#include "scene/transform.h"

namespace gfx
{
    enum class MaterialType
    {
        MaterialA,
        MaterialB
    };

    // Objeto renderizable de la escena.
    // Agrupa geometría, transformación, materiales y texturas.
    struct Object3D
    {
        Transform transform;
        Mesh mesh;

        Material materialA;
        Material materialB;

        Texture textureA;
        Texture textureB;

        MaterialType activeMaterial = MaterialType::MaterialA;

        bool useTexture = false;
        bool visible = true;

        // Regresa el material actualmente activo.
        const Material& currentMaterial() const
        {
            return (activeMaterial == MaterialType::MaterialA) ? materialA : materialB;
        }

        Material& currentMaterial()
        {
            return (activeMaterial == MaterialType::MaterialA) ? materialA : materialB;
        }

        // Regresa la textura actualmente activa.
        const Texture* currentTexture() const
        {
            if (!useTexture)
            {
                return nullptr;
            }

            return (activeMaterial == MaterialType::MaterialA) ? &textureA : &textureB;
        }
    };
}