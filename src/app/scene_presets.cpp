#include "app/scene_presets.h"

#include <cmath>

#include "core/math/math_utils.h"

namespace gfx
{
    namespace
    {
		// Función auxiliar para crear cámaras en órbita alrededor del origen.
        Camera makeOrbitCamera(float azimuthDegrees, float elevationDegrees, float distance)
        {
            Camera camera;

            const float azimuth = toRadians(azimuthDegrees);
            const float elevation = toRadians(elevationDegrees);

            const float cosElev = std::cos(elevation);

            camera.eye =
            {
                distance * cosElev * std::sin(azimuth),
                distance * std::sin(elevation),
                distance * cosElev * std::cos(azimuth)
            };

            camera.target = { 0.0f, 0.0f, 0.0f };
            camera.up = { 0.0f, 1.0f, 0.0f };

            camera.projectionType = ProjectionType::Perspective;
            camera.fovYRadians = toRadians(30.0f);
            camera.nearPlane = 0.05f;
            camera.farPlane = 100.0f;

            return camera;
        }
    }

    Material ProjectScenePresets::makeMaterialA()
    {
        Material material;

        // Valores pedidos en el PDF
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

        // Valores pedidos en el PDF
        material.ambient = { 0.23125f, 0.23125f, 0.23125f, 1.0f };
        material.diffuse = { 0.2775f, 0.2775f, 0.2775f, 1.0f };
        material.specular = { 0.773911f, 0.773911f, 0.773911f, 1.0f };
        material.shininess = 89.60f;

      
        material.useTexture = true;
        material.useNormalMap = false;

        return material;
    }

    Light ProjectScenePresets::makeWhiteMainLight()
    {
        Light light;
        light.type = LightType::Directional;
        light.enabled = true;

        light.direction = { 0.0f, -1.0f, 0.0f };
        light.intensity = { 1.4f, 1.4f, 1.4f, 1.0f };


        return light;
    }

    Light ProjectScenePresets::makeBlueSecondaryLight()
    {
        Light light;
        light.type = LightType::Directional;
        light.enabled = true;
        light.direction = { -0.5f, -0.3f, 0.55f };
        light.intensity = { 0.0f, 0.05f, 1.5f, 1.0f };

        return light;
    }
    Light ProjectScenePresets::makeBasicLight()
    {
        Light light;
        light.type = LightType::Directional;
        light.enabled = true;
        light.direction = { 0.0f, -0.2f, -1.0f };
        light.intensity = { 0.35f, 0.35f, 0.35f, 1.0f };

        return light;
    }



    Camera ProjectScenePresets::makeFrontRightCamera(float distance)
    {
        return makeOrbitCamera(35.0f, 28.0f, distance);
    }

    Camera ProjectScenePresets::makeRightProfileCamera(float distance)
    {
        return makeOrbitCamera(90.0f, 0.0f, distance);
    }

    Camera ProjectScenePresets::makeRearCamera(float distance)
    {
        return makeOrbitCamera(180.0f, 0.0f, distance);
    }


    std::string ProjectScenePresets::texturePathMaterialA()
    {
        return "";
    }

    std::string ProjectScenePresets::texturePathMaterialB()
    {
        return "";
    }
}