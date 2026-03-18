#pragma once
#include "pixelbuffer.h"
#include "Matematicas.h"
#include "Camera.h"
#include "Projection.h"
#include "CubeMesh.h"
#include "Rasterizer.h"
#include "Lighting.h"

struct Viewport
{
    int x = 0;
    int y = 0;
    int w = 1;
    int h = 1;
};

class RenderPipeline
{
public:
    Camera cam{};
    Projection proj{};

    Camera camA{};
    Camera camB{};

    void setCameraPreset(int preset);
    void setCameraPresetA(int preset);
    void setCameraPresetB(int preset);

    void toggleProjection();

    void toggleWhiteLight();
    void toggleRedLight();

    void toggleShadingMode();
    bool isPhongMode() const { return m_shadingMode == ShadingMode::Phong; }

    void increaseSpecularIntensity();
    void decreaseSpecularIntensity();
    float specularIntensity() const { return m_material.specularIntensity; }

    void renderCube(PixelBuffer& buffer) const;
    void renderCubeDual(PixelBuffer& buffer) const;

    void toggleAnimation();
    bool isAnimating() const { return m_animate; }
    void tick(float dtSeconds);

private:
    static bool ndcToPixel(float ndcX, float ndcY, float ndcZ,
        const PixelBuffer& b, const Viewport& vp,
        int& outX, int& outY);

    bool projectVertexToScreen(
        const Vertex& inVertex,
        const Mat4& model,
        const Mat4& MVP,
        const Camera& camera,
        const Projection& projection,
        const PixelBuffer& buffer,
        const Viewport& vp,
        ScreenVertex& outVertex) const;

    void renderCubeIntoViewport(
        PixelBuffer& buffer,
        Rasterizer& rasterizer,
        const Camera& camera,
        const Projection& projection,
        const Viewport& vp,
        const Mat4& model) const;

private:
    Light m_whiteLight{
        true,
        { -1.0f, -1.0f, -1.0f },
        {  0.8f,  0.8f,  0.8f }
    };

    Light m_redLight{
        true,
        {  1.0f, -1.0f, -0.5f },
        {  0.8f,  0.0f,  0.0f }
    };

    Material m_material{};
    ShadingMode m_shadingMode = ShadingMode::Gouraud;

    bool  m_animate = false;
    float m_angleDeg = 0.0f;
    float m_speedDegPerSec = 45.0f;
};