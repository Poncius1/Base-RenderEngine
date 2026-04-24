#pragma once

#include "scene/scene.h"
#include "scene/mesh.h"

namespace gfx
{
    // Construye escenas base para el renderer.
    // Su objetivo es centralizar la creación de cámaras, luces,
    // materiales y objetos iniciales del proyecto.
    class SceneBuilder
    {
    public:
        // Construye una escena base usando una malla proporcionada.
        static Scene buildDefaultScene(const Mesh& mesh);

    private:
        // Crea las cámaras iniciales de la escena.
        static void addDefaultCameras(Scene& scene);

        // Crea las luces iniciales de la escena.
        static void addDefaultLights(Scene& scene);

        // Crea el objeto principal de la escena.
        static void addMainObject(Scene& scene, const Mesh& mesh);
    };
}