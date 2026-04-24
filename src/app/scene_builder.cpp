#include "app/scene_builder.h"

#include <algorithm>
#include <cmath>

#include "app/scene_presets.h"
#include "scene/object3d.h"
#include "loaders/texture_loader.h"

namespace gfx
{
    namespace
    {
		// Calcula los bounds (min y max) de un mesh.
        void computeBounds(const Mesh& mesh, Vec3& minP, Vec3& maxP)
        {
            minP = mesh.vertices[0].position;
            maxP = mesh.vertices[0].position;

            for (const Vertex& v : mesh.vertices)
            {
                minP.x = std::min(minP.x, v.position.x);
                minP.y = std::min(minP.y, v.position.y);
                minP.z = std::min(minP.z, v.position.z);

                maxP.x = std::max(maxP.x, v.position.x);
                maxP.y = std::max(maxP.y, v.position.y);
                maxP.z = std::max(maxP.z, v.position.z);
            }
        }
    }

    Scene SceneBuilder::buildDefaultScene(const Mesh& mesh)
    {
        Scene scene;

        addMainObject(scene, mesh);
        addDefaultLights(scene);
        addDefaultCameras(scene);

        scene.setActiveCamera(0);
        return scene;
    }

    void SceneBuilder::addMainObject(Scene& scene, const Mesh& mesh)
    {
        Object3D object;
        object.mesh = mesh;
        object.visible = true;
        object.useTexture = true;

        Vec3 minP{};
        Vec3 maxP{};
        computeBounds(mesh, minP, maxP);

        const Vec3 center =
        {
            (minP.x + maxP.x) * 0.5f,
            (minP.y + maxP.y) * 0.5f,
            (minP.z + maxP.z) * 0.5f
        };

        const Vec3 size =
        {
            maxP.x - minP.x,
            maxP.y - minP.y,
            maxP.z - minP.z
        };

        const float maxSize = std::max({ size.x, size.y, size.z, 0.0001f });

        // Tamaño estándar para todos los modelos.
        const float targetSize = 2.4f;
        const float scale = targetSize / maxSize;

        // centrar modelo en el origen.
        object.transform.position =
        {
            -center.x * scale,
            -center.y * scale,
            -center.z * scale
        };

        object.transform.rotation = { 0.0f, 0.0f, 0.0f };
        object.transform.scale = { scale, scale, scale };

        object.materialA = ProjectScenePresets::makeMaterialA();
        object.materialB = ProjectScenePresets::makeMaterialB();
        object.activeMaterial = MaterialType::MaterialA;
        scene.objects.push_back(object);
    }
    
	// Agrega cámaras predeterminadas a la escena
    void SceneBuilder::addDefaultCameras(Scene& scene)
    {
        const float distance = 10.0f;

        scene.cameras.push_back(ProjectScenePresets::makeFrontRightCamera(distance));
        scene.cameras.push_back(ProjectScenePresets::makeRightProfileCamera(distance));
        scene.cameras.push_back(ProjectScenePresets::makeRearCamera(distance));
    }

	// Agrega luces predeterminadas a la escena
    void SceneBuilder::addDefaultLights(Scene& scene)
    {
        scene.lights.push_back(ProjectScenePresets::makeWhiteMainLight());
        scene.lights.push_back(ProjectScenePresets::makeBlueSecondaryLight());
        scene.lights.push_back(ProjectScenePresets::makeBasicLight());

    }
}