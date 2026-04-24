#include "app/scene_presets.h"

#include "core/math/math_utils.h"

namespace gfx
{
    Material ProjectScenePresets::makeMaterialA()
    {
        Material material;
        material.ambient = { 0.00f, 0.00f, 0.00f, 1.00f };
        material.diffuse = { 0.50f, 0.50f, 0.50f, 1.00f };
        material.specular = { 0.70f, 0.70f, 0.70f, 1.00f };
        material.shininess = 32.0f;
        material.useTexture = true;
        material.useNormalMap = false;
        return material;
    }

    Material ProjectScenePresets::makeMaterialB()
    {
        Material material;
        material.ambient = { 0.23125f, 0.23125f, 0.23125f, 1.0f };
        material.diffuse = { 0.2775f, 0.2775f, 0.2775f, 1.0f };
        material.specular = { 0.773911f, 0.773911f, 0.773911f, 1.0f };
        material.shininess = 89.60f;
        material.useTexture = true;
        material.useNormalMap = false;
        return material;
    }

    Camera ProjectScenePresets::makeFrontPerspectiveCamera(float distance)
    {
        Camera camera;
        camera.eye = { 0.0f, 0.0f, distance };
        camera.target = { 0.0f, 0.0f, 0.0f };
        camera.up = { 0.0f, 1.0f, 0.0f };
        camera.projectionType = ProjectionType::Perspective;
        camera.fovYRadians = toRadians(60.0f);
        camera.nearPlane = 0.1f;
        camera.farPlane = 100.0f;
        return camera;
    }

    Camera ProjectScenePresets::makeSideOrthographicCamera(float extent)
    {
        Camera camera;
        camera.eye = { extent * 2.0f, 0.0f, 0.0f };
        camera.target = { 0.0f, 0.0f, 0.0f };
        camera.up = { 0.0f, 1.0f, 0.0f };
        camera.projectionType = ProjectionType::Orthographic;
        camera.left = -extent;
        camera.right = extent;
        camera.bottom = -extent;
        camera.top = extent;
        camera.nearPlane = 0.1f;
        camera.farPlane = 100.0f;
        return camera;
    }

    Camera ProjectScenePresets::makeTopOrthographicCamera(float extent)
    {
        Camera camera;
        camera.eye = { 0.0f, extent * 2.0f, 0.0f };
        camera.target = { 0.0f, 0.0f, 0.0f };
        camera.up = { 0.0f, 0.0f, -1.0f };
        camera.projectionType = ProjectionType::Orthographic;
        camera.left = -extent;
        camera.right = extent;
        camera.bottom = -extent;
        camera.top = extent;
        camera.nearPlane = 0.1f;
        camera.farPlane = 100.0f;
        return camera;
    }

    Light ProjectScenePresets::makeWhiteMainLight()
    {
        Light light;
        light.type = LightType::Directional;
        light.enabled = true;

        // Como LightingModel usa -light.direction,
        // esta dirección significa que la luz viene desde arriba-derecha-frente.
        light.direction = { 0.8f, -1.0f, 0.6f };
        light.intensity = { 1.15f, 1.12f, 1.08f, 1.0f };
        return light;
    }

    Light ProjectScenePresets::makeBlueSecondaryLight()
    {
        Light light;
        light.type = LightType::Directional;
        light.enabled = true;

        // Luz azul de relleno desde el lado opuesto.
        light.direction = { -1.0f, -0.4f, 0.3f };
        light.intensity = { 0.18f, 0.24f, 0.95f, 1.0f };
        return light;
    }

    std::string ProjectScenePresets::texturePathMaterialA()
    {
        return "D:/Visual Projects/Graficacion1/assets/textures/materialA.jpg";
    }

    std::string ProjectScenePresets::texturePathMaterialB()
    {
        return "D:/Visual Projects/Graficacion1/assets/textures/materialB.jpg";
    }
}