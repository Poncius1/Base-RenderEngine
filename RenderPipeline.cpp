#include "RenderPipeline.h"

static constexpr uint32_t kWhiteARGB = 0xFFFFFFFFu;
static constexpr uint32_t kBlackARGB = 0xFF000000u;

bool RenderPipeline::ndcToPixel(float ndcX, float ndcY, float ndcZ,
    const PixelBuffer& b, const Viewport& vp,
    int& outX, int& outY)
{
    if (ndcX < -1.0f || ndcX > 1.0f) return false;
    if (ndcY < -1.0f || ndcY > 1.0f) return false;
    if (ndcZ < -1.0f || ndcZ > 1.0f) return false;

    float fx = (ndcX * 0.5f + 0.5f) * float(vp.w - 1);
    float fy = (1.0f - (ndcY * 0.5f + 0.5f)) * float(vp.h - 1);

    outX = vp.x + m_roundToInt(fx);
    outY = vp.y + m_roundToInt(fy);

    return b.inBounds(outX, outY);
}

void RenderPipeline::drawPointCross(PixelBuffer& b, int x, int y, uint32_t argb)
{
    b.setPixel(x, y, argb);
    b.setPixel(x - 1, y, argb);
    b.setPixel(x + 1, y, argb);
    b.setPixel(x, y - 1, argb);
    b.setPixel(x, y + 1, argb);
}

void RenderPipeline::toggleProjection()
{
    proj.type = (proj.type == ProjectionType::Persp) ? ProjectionType::Ortho : ProjectionType::Persp;
}

// presets base
void RenderPipeline::setCameraPreset(int preset)
{
    cam.center = { 0,0,0 };

    switch (preset)
    {
    default:
    case 0: cam.eye = { 0,0,60 };   cam.up = { 0,1,0 };  break; // frente
    case 1: cam.eye = { 45,35,60 }; cam.up = { 0,1,0 };  break; // diagonal
    }
}

// presets para split
void RenderPipeline::setCameraPresetA(int preset)
{
    camA.center = { 0,0,0 };
    switch (preset)
    {
    default:
    case 0: camA.eye = { 0,0,60 };  camA.up = { 0,1,0 }; break; // frontal
    case 1: camA.eye = { 60,0,0 };  camA.up = { 0,1,0 }; break; // derecha
    }
}

void RenderPipeline::setCameraPresetB(int preset)
{
    camB.center = { 0,0,0 };
    switch (preset)
    {
    default:
    case 0: camB.eye = { 45,35,60 }; camB.up = { 0,1,0 };  break; // diagonal
    case 1: camB.eye = { 0,60,0 };   camB.up = { 0,0,-1 }; break; // arriba
    }
}

// Renderiza el cubo en un viewport específico 
void RenderPipeline::renderCubeIntoViewport(PixelBuffer& buffer,
    const Camera& camera,
    const Projection& projection,
    const Viewport& vp,
    const Mat4& model) const
{
    float aspect = (vp.h > 0) ? float(vp.w) / float(vp.h) : 1.0f;

    Mat4 V = makeView(camera);
    Mat4 P = makeProjection(projection, aspect);

    // MVP = P * V * M
    Mat4 PV = mat4Mul(P, V);
    Mat4 MVP = mat4Mul(PV, model);

    const float hx = 7.5f;
    const float hy = 5.0f;
    const float hz = 10.0f;

    const Vec3 verts[8] = {
        {-hx,-hy,-hz},{ hx,-hy,-hz},{ hx, hy,-hz},{-hx, hy,-hz},
        {-hx,-hy, hz},{ hx,-hy, hz},{ hx, hy, hz},{-hx, hy, hz}
    };

    for (const Vec3& v : verts)
    {
        Vec4 clip = mat4MulVec4(MVP, V4(v.x, v.y, v.z, 1.0f));

        if (projection.type == ProjectionType::Persp)
        {
            if (clip.w <= 1e-6f) continue; // detrás de cámara
        }
        else
        {
            if (m_fabs(clip.w) < 1e-6f) continue;
        }

        float invW = 1.0f / clip.w;
        float ndcX = clip.x * invW;
        float ndcY = clip.y * invW;
        float ndcZ = clip.z * invW;

        int px, py;
        if (!ndcToPixel(ndcX, ndcY, ndcZ, buffer, vp, px, py)) continue;

        drawPointCross(buffer, px, py, kWhiteARGB);
    }
}

// Base: una sola vista (full viewport)
void RenderPipeline::renderCubePoints(PixelBuffer& buffer) const
{
    buffer.clear(kBlackARGB);

    // EXTRA B: model animado
    Mat4 model = mat4Identity();
    if (m_animate)
    {
        float rad = m_degToRad(m_angleDeg);
        model = mat4RotationY(rad);
    }

    Viewport vp{ 0, 0, buffer.w, buffer.h };
    renderCubeIntoViewport(buffer, cam, proj, vp, model);
}

// EXTRA A: dos vistas al mismo tiempo (split horizontal)
void RenderPipeline::renderCubePointsDual(PixelBuffer& buffer) const
{
    buffer.clear(kBlackARGB);

    // EXTRA B: model animado
    Mat4 model = mat4Identity();
    if (m_animate)
    {
        float rad = m_degToRad(m_angleDeg);
        model = mat4RotationY(rad);
    }

    int halfW = buffer.w / 2;

    Viewport left{ 0,      0, halfW,            buffer.h };
    Viewport right{ halfW,  0, buffer.w - halfW, buffer.h };

    renderCubeIntoViewport(buffer, camA, proj, left, model);
    renderCubeIntoViewport(buffer, camB, proj, right, model);
}

// EXTRA B Rotacion
void RenderPipeline::toggleAnimation()
{
    m_animate = !m_animate;
}

void RenderPipeline::tick(float dtSeconds)
{
    if (!m_animate) return;

    m_angleDeg += m_speedDegPerSec * dtSeconds;
    if (m_angleDeg > 360.0f) m_angleDeg -= 360.0f;
}