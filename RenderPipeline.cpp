#include "RenderPipeline.h"

static constexpr uint32_t kBlackARGB = 0xFF000000u;

namespace
{
    inline Vec3 transformPoint(const Mat4& M, const Vec3& p)
    {
        const Vec4 r = mat4MulVec4(M, V4(p.x, p.y, p.z, 1.0f));
        return { r.x, r.y, r.z };
    }

    inline Vec3 transformDirection(const Mat4& M, const Vec3& v)
    {
        const Vec4 r = mat4MulVec4(M, V4(v.x, v.y, v.z, 0.0f));
        return { r.x, r.y, r.z };
    }
}

RenderPipeline::RenderPipeline()
{
    m_mesh = MeshFactory::buildCube();
}

bool RenderPipeline::ndcToPixel(float ndcX, float ndcY, float ndcZ,
    const PixelBuffer& b, const Viewport& vp,
    int& outX, int& outY)
{
    if (ndcX < -1.0f || ndcX > 1.0f) return false;
    if (ndcY < -1.0f || ndcY > 1.0f) return false;
    if (ndcZ < -1.0f || ndcZ > 1.0f) return false;

    const float fx = (ndcX * 0.5f + 0.5f) * float(vp.w - 1);
    const float fy = (1.0f - (ndcY * 0.5f + 0.5f)) * float(vp.h - 1);

    outX = vp.x + m_roundToInt(fx);
    outY = vp.y + m_roundToInt(fy);

    return b.inBounds(outX, outY);
}

void RenderPipeline::toggleProjection()
{
    proj.type = (proj.type == ProjectionType::Persp)
        ? ProjectionType::Ortho
        : ProjectionType::Persp;
}

void RenderPipeline::toggleWhiteLight()
{
    m_whiteLight.enabled = !m_whiteLight.enabled;
}

void RenderPipeline::toggleRedLight()
{
    m_redLight.enabled = !m_redLight.enabled;
}

void RenderPipeline::toggleShadingMode()
{
    m_shadingMode = (m_shadingMode == ShadingMode::Gouraud)
        ? ShadingMode::Phong
        : ShadingMode::Gouraud;
}

void RenderPipeline::increaseSpecularIntensity()
{
    m_material.specularIntensity += 0.1f;
    if (m_material.specularIntensity > 3.0f)
        m_material.specularIntensity = 3.0f;
}

void RenderPipeline::decreaseSpecularIntensity()
{
    m_material.specularIntensity -= 0.1f;
    if (m_material.specularIntensity < 0.0f)
        m_material.specularIntensity = 0.0f;
}

void RenderPipeline::setMesh(const Mesh& mesh)
{
    m_mesh = mesh;
}

void RenderPipeline::setCameraPreset(int preset)
{
    cam.center = { 0,0,0 };

    switch (preset)
    {
    default:
    case 0:
        cam.eye = { 0,0,60 };
        cam.up = { 0,1,0 };
        break;
    case 1:
        cam.eye = { 45,35,60 };
        cam.up = { 0,1,0 };
        break;
    }
}

void RenderPipeline::setCameraPresetA(int preset)
{
    camA.center = { 0,0,0 };

    switch (preset)
    {
    default:
    case 0:
        camA.eye = { 0,0,60 };
        camA.up = { 0,1,0 };
        break;
    case 1:
        camA.eye = { 60,0,0 };
        camA.up = { 0,1,0 };
        break;
    }
}

void RenderPipeline::setCameraPresetB(int preset)
{
    camB.center = { 0,0,0 };

    switch (preset)
    {
    default:
    case 0:
        camB.eye = { 45,35,60 };
        camB.up = { 0,1,0 };
        break;
    case 1:
        camB.eye = { 0,60,0 };
        camB.up = { 0,0,-1 };
        break;
    }
}

bool RenderPipeline::projectVertexToScreen(
    const Vertex& inVertex,
    const Mat4& model,
    const Mat4& MVP,
    const Camera& camera,
    const Projection& projection,
    const PixelBuffer& buffer,
    const Viewport& vp,
    ScreenVertex& outVertex) const
{
    const Vec3& p = inVertex.position;

    const Vec3 worldPos = transformPoint(model, inVertex.position);
    const Vec3 worldNormal = transformDirection(model, inVertex.normal);

    const Vec3 litColor = Lighting::shadePoint(
        inVertex.color,
        worldPos,
        worldNormal,
        camera,
        m_material,
        m_whiteLight,
        m_redLight
    );

    const Vec4 clip = mat4MulVec4(MVP, V4(p.x, p.y, p.z, 1.0f));

    if (projection.type == ProjectionType::Persp)
    {
        if (clip.w <= 1e-6f) return false;
    }
    else
    {
        if (m_fabs(clip.w) < 1e-6f) return false;
    }

    const float invW = 1.0f / clip.w;
    const float ndcX = clip.x * invW;
    const float ndcY = clip.y * invW;
    const float ndcZ = clip.z * invW;

    int px, py;
    if (!ndcToPixel(ndcX, ndcY, ndcZ, buffer, vp, px, py))
        return false;

    outVertex.x = float(px);
    outVertex.y = float(py);
    outVertex.z = ndcZ;

    outVertex.color = litColor;
    outVertex.baseColor = inVertex.color;
    outVertex.worldPos = worldPos;
    outVertex.worldNormal = worldNormal;

    return true;
}

void RenderPipeline::renderMeshIntoViewport(
    PixelBuffer& buffer,
    Rasterizer& rasterizer,
    const Mesh& mesh,
    const Camera& camera,
    const Projection& projection,
    const Viewport& vp,
    const Mat4& model) const
{
    const float aspect = (vp.h > 0) ? float(vp.w) / float(vp.h) : 1.0f;

    const Mat4 V = makeView(camera);
    const Mat4 P = makeProjection(projection, aspect);
    const Mat4 PV = mat4Mul(P, V);
    const Mat4 MVP = mat4Mul(PV, model);

    for (const Triangle& tri : mesh.triangles)
    {
        ScreenVertex sv0, sv1, sv2;

        const bool ok0 = projectVertexToScreen(tri.v0, model, MVP, camera, projection, buffer, vp, sv0);
        const bool ok1 = projectVertexToScreen(tri.v1, model, MVP, camera, projection, buffer, vp, sv1);
        const bool ok2 = projectVertexToScreen(tri.v2, model, MVP, camera, projection, buffer, vp, sv2);

        if (!ok0 || !ok1 || !ok2)
            continue;

        rasterizer.drawTriangle(buffer, sv0, sv1, sv2);
    }
}

void RenderPipeline::render(PixelBuffer& buffer) const
{
    buffer.clear(kBlackARGB);

    Mat4 model = mat4Identity();
    if (m_animate)
    {
        const float rad = m_degToRad(m_angleDeg);
        model = mat4RotationY(rad);
    }

    Rasterizer rasterizer;
    RasterizerFrameSettings settings;
    settings.shadingMode = m_shadingMode;
    settings.camera = cam;
    settings.material = m_material;
    settings.whiteLight = m_whiteLight;
    settings.redLight = m_redLight;

    rasterizer.beginFrame(buffer, settings);

    const Viewport vp{ 0, 0, buffer.w, buffer.h };
    renderMeshIntoViewport(buffer, rasterizer, m_mesh, cam, proj, vp, model);
}

void RenderPipeline::renderDual(PixelBuffer& buffer) const
{
    buffer.clear(kBlackARGB);

    Mat4 model = mat4Identity();
    if (m_animate)
    {
        const float rad = m_degToRad(m_angleDeg);
        model = mat4RotationY(rad);
    }

    Rasterizer rasterizer;
    RasterizerFrameSettings settings;
    settings.shadingMode = m_shadingMode;
    settings.camera = camA;
    settings.material = m_material;
    settings.whiteLight = m_whiteLight;
    settings.redLight = m_redLight;

    rasterizer.beginFrame(buffer, settings);

    const int halfW = buffer.w / 2;
    const Viewport left{ 0, 0, halfW, buffer.h };
    const Viewport right{ halfW, 0, buffer.w - halfW, buffer.h };

    renderMeshIntoViewport(buffer, rasterizer, m_mesh, camA, proj, left, model);

    settings.camera = camB;
    rasterizer.beginFrame(buffer, settings);
    renderMeshIntoViewport(buffer, rasterizer, m_mesh, camB, proj, right, model);
}

void RenderPipeline::toggleAnimation()
{
    m_animate = !m_animate;
}

void RenderPipeline::tick(float dtSeconds)
{
    if (!m_animate) return;

    m_angleDeg += m_speedDegPerSec * dtSeconds;
    if (m_angleDeg > 360.0f)
        m_angleDeg -= 360.0f;
}