#pragma once
#include "pixelbuffer.h"
#include "Matematicas.h"
#include "Lighting.h"
#include <QVector>

enum class ShadingMode
{
    Gouraud,
    Phong
};

struct ScreenVertex
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    Vec3 color{};
    Vec3 baseColor{};
    Vec3 worldPos{};
    Vec3 worldNormal{};
};

struct RasterizerFrameSettings
{
    ShadingMode shadingMode = ShadingMode::Gouraud;
    Camera camera{};
    Material material{};
    Light whiteLight{};
    Light redLight{};
};

class Rasterizer
{
public:
    Rasterizer() = default;

    void beginFrame(const PixelBuffer& buffer, const RasterizerFrameSettings& settings);

    void drawTriangle(
        PixelBuffer& buffer,
        const ScreenVertex& v0,
        const ScreenVertex& v1,
        const ScreenVertex& v2);

private:
    static float edgeFunction(
        float ax, float ay,
        float bx, float by,
        float px, float py);

    static uint32_t packColorARGB(const Vec3& c);

private:
    QVector<float> m_zBuffer;
    RasterizerFrameSettings m_settings;
};