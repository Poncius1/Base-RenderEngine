#pragma once

#include "core/image/depth_buffer.h"
#include "core/image/pixel_buffer.h"
#include "core/math/vec2.h"
#include "core/math/vec3.h"
#include "renderer/pipeline/render_settings.h"
#include "renderer/pipeline/viewport.h"
#include "renderer/stages/lighting_model.h"
#include "scene/camera.h"
#include "scene/light.h"
#include "scene/material.h"
#include "scene/texture.h"

namespace gfx
{
    struct ScreenVertex
    {
        Vec3 screenPosition;
        Vec3 worldPosition;
        Vec3 worldNormal;
        Vec2 uv;
        Color color;
    };

    class Rasterizer
    {
    public:
        void drawTriangle(
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
            const ScreenVertex& v2) const;

    private:
        static float edgeFunction(const Vec2& a, const Vec2& b, const Vec2& p);

        static float interpolate(float a, float b, float c, float w0, float w1, float w2);
        static Vec2 interpolate(const Vec2& a, const Vec2& b, const Vec2& c, float w0, float w1, float w2);
        static Vec3 interpolate(const Vec3& a, const Vec3& b, const Vec3& c, float w0, float w1, float w2);
        static Color interpolate(const Color& a, const Color& b, const Color& c, float w0, float w1, float w2);

        static bool triangleOutsideViewport(
            const Vec2& p0,
            const Vec2& p1,
            const Vec2& p2,
            const Viewport& viewport);

        static bool triangleOutsideDepthRange(
            float z0,
            float z1,
            float z2);

        static bool isBackFacing(
            float signedArea,
            const RenderSettings& settings);
    };
}