#pragma once

#include <vector>
#include <algorithm>
#include <cmath>

#include "core/image/color.h"
#include "core/math/vec3.h"
#include "core/math/math_utils.h"
#include "scene/camera.h"
#include "scene/light.h"
#include "scene/material.h"

namespace gfx
{
    // Modelo de iluminación tipo Phong.
    // Calcula ambiente + difusa + especular para una lista de luces activas.
    class LightingModel
    {
    public:
        static Color evaluate(
            const Vec3& worldPosition,
            const Vec3& worldNormal,
            const Color& baseColor,
            const Material& material,
            const Camera& camera,
            const std::vector<Light>& lights)
        {
            const Vec3 N = normalize(worldNormal);
            const Vec3 V = normalize(camera.eye - worldPosition);

            // Componente ambiente.
            Vec3 result = hadamard(colorToVec3(baseColor), colorToVec3(material.ambient));

            for (const Light& light : lights)
            {
                if (!light.enabled)
                {
                    continue;
                }

                result += evaluateSingleLight(
                    worldPosition,
                    N,
                    V,
                    baseColor,
                    material,
                    light
                );
            }

            return colorFromVec3({
                saturate(result.x),
                saturate(result.y),
                saturate(result.z)
                });
        }

    private:
        static Vec3 evaluateSingleLight(
            const Vec3& worldPosition,
            const Vec3& normal,
            const Vec3& viewDirection,
            const Color& baseColor,
            const Material& material,
            const Light& light)
        {
            Vec3 lightDirection{};

            if (light.type == LightType::Directional)
            {
                lightDirection = normalize(-light.direction);
            }
            else
            {
                lightDirection = normalize(light.position - worldPosition);
            }

            const float ndotl = std::max(0.0f, dot(normal, lightDirection));

            const Vec3 base = colorToVec3(baseColor);
            const Vec3 lightColor = colorToVec3(light.intensity);
            const Vec3 materialDiffuse = colorToVec3(material.diffuse);
            const Vec3 materialSpecular = colorToVec3(material.specular);

            // Difusa.
            const Vec3 diffuse =
                hadamard(hadamard(base, materialDiffuse), lightColor) * ndotl;

            // Especular.
            const Vec3 reflection =
                normalize((2.0f * dot(normal, lightDirection) * normal) - lightDirection);

            const float rdotv = std::max(0.0f, dot(reflection, viewDirection));
            const float specFactor = std::pow(rdotv, material.shininess);

            const Vec3 specular =
                hadamard(materialSpecular, lightColor) * specFactor;

            return diffuse + specular;
        }
    };
}