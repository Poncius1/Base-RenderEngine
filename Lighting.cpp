#include "Lighting.h"
#include <algorithm>
#include <cmath>

namespace
{
    inline float clamp01(float v)
    {
        if (v < 0.0f) return 0.0f;
        if (v > 1.0f) return 1.0f;
        return v;
    }

    inline Vec3 mulVec3(const Vec3& a, const Vec3& b)
    {
        return { a.x * b.x, a.y * b.y, a.z * b.z };
    }
}

Vec3 Lighting::shadeSingleLight(
    const Vec3& baseColor,
    const Vec3& worldPos,
    const Vec3& normal,
    const Vec3& viewDir,
    const Material& material,
    const Light& light)
{
    (void)worldPos;

    if (!light.enabled)
        return { 0.0f, 0.0f, 0.0f };

    // Normal segura (por robustez)
    const Vec3 N = normalize(normal);

    const Vec3 lightDir = normalize({
        -light.direction.x,
        -light.direction.y,
        -light.direction.z
        });

    const float ndotl = std::max(0.0f, dot(N, lightDir));

    // Si no hay difusa, no hay especular (evita highlights incorrectos)
    if (ndotl <= 0.0f)
        return { 0.0f, 0.0f, 0.0f };

    // Difusa
    const Vec3 diffuse = mulVec3(
        mulVec3(baseColor, material.kd),
        mul(light.intensity, ndotl)
    );

    // 🔥 Blinn-Phong (half vector)
    const Vec3 halfVec = normalize(add(lightDir, viewDir));
    const float ndoth = std::max(0.0f, dot(N, halfVec));
    const float specFactor = std::pow(ndoth, material.shininess);

    Vec3 specular = mulVec3(
        material.ks,
        mul(light.intensity, specFactor)
    );

    specular = mul(specular, material.specularIntensity);

    return add(diffuse, specular);
}

Vec3 Lighting::shadePoint(
    const Vec3& baseColor,
    const Vec3& worldPos,
    const Vec3& worldNormal,
    const Camera& camera,
    const Material& material,
    const Light& lightA,
    const Light& lightB)
{
    const Vec3 N = normalize(worldNormal);
    const Vec3 V = normalize(sub(camera.eye, worldPos));

    // Componente ambiental
    Vec3 result = mulVec3(baseColor, material.ka);

    // Luces
    result = add(result, shadeSingleLight(baseColor, worldPos, N, V, material, lightA));
    result = add(result, shadeSingleLight(baseColor, worldPos, N, V, material, lightB));

    // Clamp final
    result.x = clamp01(result.x);
    result.y = clamp01(result.y);
    result.z = clamp01(result.z);

    return result;
}