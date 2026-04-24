#pragma once

#include <vector>
#include "scene/camera.h"
#include "scene/light.h"
#include "scene/object3d.h"

namespace gfx
{
    // Escena principal del renderer.
    // Contiene los objetos, cámaras y luces que serán procesados en cada frame.
    class Scene
    {
    public:
        std::vector<Object3D> objects;
        std::vector<Camera> cameras;
        std::vector<Light> lights;

        // Índice de la cámara activa.
        size_t activeCameraIndex = 0;

        // Regresa la cámara activa.
        // Si no hay cámaras, regresa nullptr.
        Camera* activeCamera()
        {
            if (cameras.empty() || activeCameraIndex >= cameras.size())
            {
                return nullptr;
            }

            return &cameras[activeCameraIndex];
        }

        const Camera* activeCamera() const
        {
            if (cameras.empty() || activeCameraIndex >= cameras.size())
            {
                return nullptr;
            }

            return &cameras[activeCameraIndex];
        }

        // Regresa true si la escena tiene una cámara válida para renderizar.
        bool hasValidCamera() const
        {
            return activeCamera() != nullptr;
        }

        // Cambia la cámara activa si el índice es válido.
        void setActiveCamera(size_t index)
        {
            if (index < cameras.size())
            {
                activeCameraIndex = index;
            }
        }

        // Limpia todos los elementos de la escena.
        void clear()
        {
            objects.clear();
            cameras.clear();
            lights.clear();
            activeCameraIndex = 0;
        }

        bool empty() const
        {
            return objects.empty();
        }
    };
}