#pragma once

#include "core/image/depth_buffer.h"
#include "core/image/pixel_buffer.h"
#include "renderer/pipeline/render_settings.h"
#include "renderer/pipeline/viewport.h"
#include "scene/scene.h"

namespace gfx
{
    // Renderer principal del proyecto.
    // Su responsabilidad es orquestar el proceso de render de una escena
    // sobre un framebuffer y un depth buffer.
    class RenderEngine
    {
    public:
        RenderEngine() = default;

        void setRenderSettings(const RenderSettings& settings)
        {
            m_settings = settings;
        }

        const RenderSettings& renderSettings() const
        {
            return m_settings;
        }

        void setViewport(const Viewport& viewport)
        {
            m_viewport = viewport;
        }

        const Viewport& viewport() const
        {
            return m_viewport;
        }

        void resize(int width, int height)
        {
            m_colorBuffer.resize(width, height);
            m_depthBuffer.resize(width, height);

            if (m_viewport.width == 0 || m_viewport.height == 0)
            {
                m_viewport = { 0, 0, width, height };
            }
        }

        PixelBuffer& colorBuffer()
        {
            return m_colorBuffer;
        }

        const PixelBuffer& colorBuffer() const
        {
            return m_colorBuffer;
        }

        DepthBuffer& depthBuffer()
        {
            return m_depthBuffer;
        }

        const DepthBuffer& depthBuffer() const
        {
            return m_depthBuffer;
        }

        void beginFrame()
        {
            if (m_settings.clearColorBuffer)
            {
                m_colorBuffer.clear(m_settings.clearColor);
            }

            if (m_settings.clearDepthBuffer)
            {
                m_depthBuffer.clear();
            }
        }

        // Solo declaración aquí.
        void render(const Scene& scene);

    private:
        RenderSettings m_settings;
        Viewport m_viewport;

        PixelBuffer m_colorBuffer;
        DepthBuffer m_depthBuffer;
    };
}