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
        Vec3 computeMeshCenter(const Mesh& mesh)
        {
            return mesh.bounds.center;
        }

        float computeMeshRadius(const Mesh& mesh)
        {
            if (mesh.vertices.empty())
            {
                return 1.0f;
            }

            const Vec3 center = mesh.bounds.center;
            float maxDistanceSquared = 0.0f;

            for (const Vertex& vertex : mesh.vertices)
            {
                const Vec3 d = vertex.position - center;
                const float dist2 = dot(d, d);
                maxDistanceSquared = std::max(maxDistanceSquared, dist2);
            }

            return std::sqrt(std::max(maxDistanceSquared, 1e-6f));
        }

        float computeCameraDistanceForSphere(float radius, float fovYRadians, float fitFactor)
        {
            // Distancia para que la esfera entre razonablemente en pantalla.
            const float safeRadius = std::max(radius, 1e-4f);
            const float safeFov = std::max(fovYRadians, 0.1f);
            return (safeRadius / std::tan(safeFov * 0.5f)) * fitFactor;
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

    void SceneBuilder::addDefaultCameras(Scene& scene)
    {
        if (scene.objects.empty())
        {
            scene.cameras.push_back(ProjectScenePresets::makeFrontPerspectiveCamera(3.0f));
            scene.cameras.push_back(ProjectScenePresets::makeSideOrthographicCamera(2.0f));
            scene.cameras.push_back(ProjectScenePresets::makeTopOrthographicCamera(2.0f));
            return;
        }

        const Object3D& object = scene.objects[0];

        // El objeto ya está escalado, así que usamos su escala para ajustar el radio final.
        const float objectScale = object.transform.scale.x;
        const float originalRadius = computeMeshRadius(object.mesh);
        const float finalRadius = originalRadius * objectScale;

        const float fov = 60.0f * 3.14159265358979323846f / 180.0f;
        const float perspectiveDistance = computeCameraDistanceForSphere(finalRadius, fov, 2.0f);

        scene.cameras.push_back(ProjectScenePresets::makeFrontPerspectiveCamera(perspectiveDistance));
        scene.cameras.push_back(ProjectScenePresets::makeSideOrthographicCamera(finalRadius * 1.6f));
        scene.cameras.push_back(ProjectScenePresets::makeTopOrthographicCamera(finalRadius * 1.6f));
    }

    void SceneBuilder::addDefaultLights(Scene& scene)
    {
        scene.lights.push_back(ProjectScenePresets::makeWhiteMainLight());
        scene.lights.push_back(ProjectScenePresets::makeBlueSecondaryLight());
    }

    void SceneBuilder::addMainObject(Scene& scene, const Mesh& mesh)
    {
        Object3D object;
        object.mesh = mesh;
        object.visible = true;
        object.useTexture = true;

        const Vec3 meshCenter = computeMeshCenter(mesh);
        const float meshRadius = computeMeshRadius(mesh);

        // Normalizamos todos los modelos a un radio visual objetivo consistente.
        const float targetRadius = 1.0f;
        const float uniformScale = targetRadius / std::max(meshRadius, 1e-6f);

        object.transform.position = -meshCenter * uniformScale;
        object.transform.rotation = { 0.0f, 0.0f, 0.0f };
        object.transform.scale = { uniformScale, uniformScale, uniformScale };

        object.materialA = ProjectScenePresets::makeMaterialA();
        object.materialB = ProjectScenePresets::makeMaterialB();
        object.activeMaterial = MaterialType::MaterialA;

        object.textureA = TextureLoader::loadFromFile(ProjectScenePresets::texturePathMaterialA());
        object.textureB = TextureLoader::loadFromFile(ProjectScenePresets::texturePathMaterialB());

        scene.objects.push_back(object);
    }
}