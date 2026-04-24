#include "renderer/stages/rasterizer.h"

#include <algorithm>
#include <cmath>

namespace gfx
{
    float Rasterizer::edgeFunction(const Vec2& a, const Vec2& b, const Vec2& p)
    {
        return (p.x - a.x) * (b.y - a.y) - (p.y - a.y) * (b.x - a.x);
    }

    float Rasterizer::interpolate(float a, float b, float c, float w0, float w1, float w2)
    {
        return a * w0 + b * w1 + c * w2;
    }

    Vec2 Rasterizer::interpolate(const Vec2& a, const Vec2& b, const Vec2& c, float w0, float w1, float w2)
    {
        return
        {
            interpolate(a.x, b.x, c.x, w0, w1, w2),
            interpolate(a.y, b.y, c.y, w0, w1, w2)
        };
    }

    Vec3 Rasterizer::interpolate(const Vec3& a, const Vec3& b, const Vec3& c, float w0, float w1, float w2)
    {
        return
        {
            interpolate(a.x, b.x, c.x, w0, w1, w2),
            interpolate(a.y, b.y, c.y, w0, w1, w2),
            interpolate(a.z, b.z, c.z, w0, w1, w2)
        };
    }

    Color Rasterizer::interpolate(const Color& a, const Color& b, const Color& c, float w0, float w1, float w2)
    {
        return
        {
            interpolate(a.r, b.r, c.r, w0, w1, w2),
            interpolate(a.g, b.g, c.g, w0, w1, w2),
            interpolate(a.b, b.b, c.b, w0, w1, w2),
            interpolate(a.a, b.a, c.a, w0, w1, w2)
        };
    }

    bool Rasterizer::triangleOutsideViewport(
        const Vec2& p0,
        const Vec2& p1,
        const Vec2& p2,
        const Viewport& viewport)
    {
        const float minX = std::min({ p0.x, p1.x, p2.x });
        const float minY = std::min({ p0.y, p1.y, p2.y });
        const float maxX = std::max({ p0.x, p1.x, p2.x });
        const float maxY = std::max({ p0.y, p1.y, p2.y });

        const float vpMinX = static_cast<float>(viewport.x);
        const float vpMinY = static_cast<float>(viewport.y);
        const float vpMaxX = static_cast<float>(viewport.x + viewport.width - 1);
        const float vpMaxY = static_cast<float>(viewport.y + viewport.height - 1);

        return (maxX < vpMinX || minX > vpMaxX || maxY < vpMinY || minY > vpMaxY);
    }

    bool Rasterizer::triangleOutsideDepthRange(float z0, float z1, float z2)
    {
        const bool allBehindNear = (z0 < -1.0f && z1 < -1.0f && z2 < -1.0f);
        const bool allBeyondFar = (z0 > 1.0f && z1 > 1.0f && z2 > 1.0f);

        return allBehindNear || allBeyondFar;
    }

    bool Rasterizer::isBackFacing(float signedArea, const RenderSettings& settings)
    {
        if (!settings.backFaceCullingEnabled)
        {
            return false;
        }

        if (settings.frontFaceIsCCW)
        {
            return signedArea < 0.0f;
        }

        return signedArea > 0.0f;
    }

    void Rasterizer::drawTriangle(
        PixelBuffer& colorBuffer,
        DepthBuffer& depthBuffer,
        const Viewport& viewport,
        const RenderSettings& settings,
        const Camera& camera,
        const std::vector<Light>& lights,
        const Material& material,
        const Texture* texture,
        const ScreenVertex& v0,
        const ScreenVertex& v1,
        const ScreenVertex& v2) const
    {
        const Vec2 p0 = { v0.screenPosition.x, v0.screenPosition.y };
        const Vec2 p1 = { v1.screenPosition.x, v1.screenPosition.y };
        const Vec2 p2 = { v2.screenPosition.x, v2.screenPosition.y };

        if (triangleOutsideViewport(p0, p1, p2, viewport))
        {
            return;
        }

        if (triangleOutsideDepthRange(
            v0.screenPosition.z,
            v1.screenPosition.z,
            v2.screenPosition.z))
        {
            return;
        }

        const float area = edgeFunction(p0, p1, p2);

        if (std::fabs(area) < kEpsilon)
        {
            return;
        }

        if (isBackFacing(area, settings))
        {
            return;
        }

        int minX = static_cast<int>(std::floor(std::min({ p0.x, p1.x, p2.x })));
        int minY = static_cast<int>(std::floor(std::min({ p0.y, p1.y, p2.y })));
        int maxX = static_cast<int>(std::ceil(std::max({ p0.x, p1.x, p2.x })));
        int maxY = static_cast<int>(std::ceil(std::max({ p0.y, p1.y, p2.y })));

        minX = std::max(minX, viewport.x);
        minY = std::max(minY, viewport.y);
        maxX = std::min(maxX, viewport.x + viewport.width - 1);
        maxY = std::min(maxY, viewport.y + viewport.height - 1);

        if (minX > maxX || minY > maxY)
        {
            return;
        }

        for (int y = minY; y <= maxY; ++y)
        {
            for (int x = minX; x <= maxX; ++x)
            {
                const Vec2 p = { static_cast<float>(x) + 0.5f, static_cast<float>(y) + 0.5f };

                float w0 = edgeFunction(p1, p2, p);
                float w1 = edgeFunction(p2, p0, p);
                float w2 = edgeFunction(p0, p1, p);

                const bool isInside =
                    (area > 0.0f && w0 >= 0.0f && w1 >= 0.0f && w2 >= 0.0f) ||
                    (area < 0.0f && w0 <= 0.0f && w1 <= 0.0f && w2 <= 0.0f);

                if (!isInside)
                {
                    continue;
                }

                w0 /= area;
                w1 /= area;
                w2 /= area;

                const float depth = interpolate(
                    v0.screenPosition.z,
                    v1.screenPosition.z,
                    v2.screenPosition.z,
                    w0, w1, w2
                );

                if (settings.depthTestEnabled)
                {
                    if (!depthBuffer.testAndSet(x, y, depth))
                    {
                        continue;
                    }
                }

                Color shadedColor{};

                if (settings.shadingMode == ShadingMode::Gouraud)
                {
                    shadedColor = interpolate(v0.color, v1.color, v2.color, w0, w1, w2);
                }
                else
                {
                    const Vec3 worldPosition = interpolate(
                        v0.worldPosition,
                        v1.worldPosition,
                        v2.worldPosition,
                        w0, w1, w2
                    );

                    const Vec3 worldNormal = normalize(interpolate(
                        v0.worldNormal,
                        v1.worldNormal,
                        v2.worldNormal,
                        w0, w1, w2
                    ));

                    const Vec2 uv = interpolate(v0.uv, v1.uv, v2.uv, w0, w1, w2);

                    Color baseColor = material.diffuse;

                    if (texture != nullptr)
                    {
                        baseColor = texture->sample(uv);
                    }

                    shadedColor = LightingModel::evaluate(
                        worldPosition,
                        worldNormal,
                        baseColor,
                        material,
                        camera,
                        lights
                    );
                }

                colorBuffer.setPixel(x, y, shadedColor);
            }
        }
    }
}