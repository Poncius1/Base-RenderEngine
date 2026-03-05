#pragma once
#include "pixelbuffer.h"
#include "Matematicas.h"
#include "Camera.h"
#include "Projection.h"

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
    // Modo base
    Camera cam{};
    Projection proj{};

    // EXTRA A: dos cámaras
    Camera camA{};
    Camera camB{};

    // Base
    void setCameraPreset(int preset);
    void toggleProjection();
    void renderCubePoints(PixelBuffer& buffer) const;

    // EXTRA A
    void setCameraPresetA(int preset);
    void setCameraPresetB(int preset);
    void renderCubePointsDual(PixelBuffer& buffer) const;

    // EXTRA B
    void toggleAnimation();
    bool isAnimating() const { return m_animate; }
    void tick(float dtSeconds);

private:
    static bool ndcToPixel(float ndcX, float ndcY, float ndcZ,
        const PixelBuffer& b, const Viewport& vp,
        int& outX, int& outY);

    static void drawPointCross(PixelBuffer& b, int x, int y, uint32_t argb);

    void renderCubeIntoViewport(PixelBuffer& buffer,
        const Camera& camera,
        const Projection& projection,
        const Viewport& vp,
        const Mat4& model) const;

private:
    bool  m_animate = false;
    float m_angleDeg = 0.0f;
    float m_speedDegPerSec = 45.0f; // velocidad
};