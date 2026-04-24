#include "RasterizerOld.h"
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
}

uint32_t Rasterizer::packColorARGB(const Vec3& c)
{
    const int r = int(clamp01(c.x) * 255.0f + 0.5f);
    const int g = int(clamp01(c.y) * 255.0f + 0.5f);
    const int b = int(clamp01(c.z) * 255.0f + 0.5f);

    return 0xFF000000u |
        (uint32_t(r) << 16) |
        (uint32_t(g) << 8) |
        uint32_t(b);
}

void Rasterizer::beginFrame(const PixelBuffer& buffer, const RasterizerFrameSettings& settings)
{
    m_zBuffer = QVector<float>(buffer.w * buffer.h, 1e30f);
    m_settings = settings;
}

float Rasterizer::edgeFunction(
    float ax, float ay,
    float bx, float by,
    float px, float py)
{
    return (px - ax) * (by - ay) - (py - ay) * (bx - ax);
}

void Rasterizer::drawTriangle(
    PixelBuffer& buffer,
    const ScreenVertex& v0,
    const ScreenVertex& v1,
    const ScreenVertex& v2)
{
    int minX = int(std::floor(std::min({ v0.x, v1.x, v2.x })));
    int maxX = int(std::ceil(std::max({ v0.x, v1.x, v2.x })));
    int minY = int(std::floor(std::min({ v0.y, v1.y, v2.y })));
    int maxY = int(std::ceil(std::max({ v0.y, v1.y, v2.y })));

    if (minX < 0) minX = 0;
    if (minY < 0) minY = 0;
    if (maxX >= buffer.w) maxX = buffer.w - 1;
    if (maxY >= buffer.h) maxY = buffer.h - 1;

    const float area = edgeFunction(v0.x, v0.y, v1.x, v1.y, v2.x, v2.y);
    if (std::fabs(area) < 1e-6f)
        return;

    for (int y = minY; y <= maxY; ++y)
    {
        for (int x = minX; x <= maxX; ++x)
        {
            const float px = float(x) + 0.5f;
            const float py = float(y) + 0.5f;

            float w0 = edgeFunction(v1.x, v1.y, v2.x, v2.y, px, py);
            float w1 = edgeFunction(v2.x, v2.y, v0.x, v0.y, px, py);
            float w2 = edgeFunction(v0.x, v0.y, v1.x, v1.y, px, py);

            bool inside = false;
            if (area > 0.0f)
                inside = (w0 >= 0.0f && w1 >= 0.0f && w2 >= 0.0f);
            else
                inside = (w0 <= 0.0f && w1 <= 0.0f && w2 <= 0.0f);

            if (!inside)
                continue;

            w0 /= area;
            w1 /= area;
            w2 /= area;

            const float z = w0 * v0.z + w1 * v1.z + w2 * v2.z;

            const int idx = y * buffer.w + x;
            if (idx < 0 || idx >= m_zBuffer.size())
                continue;

            if (z < m_zBuffer[idx])
            {
                m_zBuffer[idx] = z;

                Vec3 finalColor{};

                if (m_settings.shadingMode == ShadingMode::Gouraud)
                {
                    finalColor.x = w0 * v0.color.x + w1 * v1.color.x + w2 * v2.color.x;
                    finalColor.y = w0 * v0.color.y + w1 * v1.color.y + w2 * v2.color.y;
                    finalColor.z = w0 * v0.color.z + w1 * v1.color.z + w2 * v2.color.z;
                }
                else
                {
                    Vec3 baseColor{};
                    baseColor.x = w0 * v0.baseColor.x + w1 * v1.baseColor.x + w2 * v2.baseColor.x;
                    baseColor.y = w0 * v0.baseColor.y + w1 * v1.baseColor.y + w2 * v2.baseColor.y;
                    baseColor.z = w0 * v0.baseColor.z + w1 * v1.baseColor.z + w2 * v2.baseColor.z;

                    Vec3 worldPos{};
                    worldPos.x = w0 * v0.worldPos.x + w1 * v1.worldPos.x + w2 * v2.worldPos.x;
                    worldPos.y = w0 * v0.worldPos.y + w1 * v1.worldPos.y + w2 * v2.worldPos.y;
                    worldPos.z = w0 * v0.worldPos.z + w1 * v1.worldPos.z + w2 * v2.worldPos.z;

                    Vec3 worldNormal{};
                    worldNormal.x = w0 * v0.worldNormal.x + w1 * v1.worldNormal.x + w2 * v2.worldNormal.x;
                    worldNormal.y = w0 * v0.worldNormal.y + w1 * v1.worldNormal.y + w2 * v2.worldNormal.y;
                    worldNormal.z = w0 * v0.worldNormal.z + w1 * v1.worldNormal.z + w2 * v2.worldNormal.z;

                    finalColor = Lighting::shadePoint(
                        baseColor,
                        worldPos,
                        worldNormal,
                        m_settings.camera,
                        m_settings.material,
                        m_settings.whiteLight,
                        m_settings.redLight
                    );
                }

                buffer.setPixel(x, y, packColorARGB(finalColor));
            }
        }
    }
}