#pragma once
#include "Matematicas.h"
#include "Camera.h"

struct Light
{
    bool enabled = true;
    Vec3 direction{ 0.0f, -1.0f, -1.0f };
    Vec3 intensity{ 1.0f, 1.0f, 1.0f };
};

struct Material
{
    Vec3 ka{ 0.3f, 0.3f, 0.3f };
    Vec3 kd{ 0.07568f, 0.61424f, 0.07568f };
    Vec3 ks{ 0.633f, 0.727811f, 0.633f };
    float shininess = 76.8f;
    float specularIntensity = 1.0f;
};

class Lighting
{
public:
    static Vec3 shadePoint(
        const Vec3& baseColor,
        const Vec3& worldPos,
        const Vec3& worldNormal,
        const Camera& camera,
        const Material& material,
        const Light& lightA,
        const Light& lightB);

private:
    static Vec3 shadeSingleLight(
        const Vec3& baseColor,
        const Vec3& worldPos,
        const Vec3& normal,
        const Vec3& viewDir,
        const Material& material,
        const Light& light);
};