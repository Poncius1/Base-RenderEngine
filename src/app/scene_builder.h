#pragma once

#include "scene/scene.h"
#include "scene/mesh.h"

namespace gfx
{
    // Construye escenas base para el renderer.
    class SceneBuilder
    {
    public:
       
        static Scene buildDefaultScene(const Mesh& mesh);

    private:
        
        static void addDefaultCameras(Scene& scene);

        static void addDefaultLights(Scene& scene);

        static void addMainObject(Scene& scene, const Mesh& mesh);
    };
}