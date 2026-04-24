#pragma once

#include "core/image/color.h"

namespace gfx
{
    enum class ShadingMode
    {
        Gouraud,
        Phong
    };

    enum class RenderMode
    {
        StaticQuality,
        Interactive
    };

    struct RenderSettings
    {
        ShadingMode shadingMode = ShadingMode::Phong;
        RenderMode renderMode = RenderMode::StaticQuality;

        Color clearColor = colors::Black();

        bool clearColorBuffer = true;
        bool clearDepthBuffer = true;
        bool depthTestEnabled = true;
        bool wireframeEnabled = false;

        bool backFaceCullingEnabled = true;
        bool frontFaceIsCCW = false;

        // Resolución interna configurable.
        float renderScale = 1.0f;
    };
}