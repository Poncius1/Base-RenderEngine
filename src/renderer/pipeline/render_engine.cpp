#include "renderer/pipeline/render_engine.h"

#include <vector>

#include "renderer/stages/rasterizer.h"
#include "renderer/stages/vertex_processor.h"

namespace gfx
{
    void RenderEngine::render(const Scene& scene)
    {
        beginFrame();

        const Camera* camera = scene.activeCamera();
        if (!camera || !m_viewport.isValid())
        {
            return;
        }

        Rasterizer rasterizer;

        for (const Object3D& obj : scene.objects)
        {
            if (!obj.visible || obj.mesh.empty())
            {
                continue;
            }

            const Mesh& mesh = obj.mesh;
            const Material& material = obj.currentMaterial();
            const Texture* texture = obj.currentTexture();

            // Caché de vértices transformados por frame.
            std::vector<ScreenVertex> processedVertices(mesh.vertices.size());

            for (size_t vertexIndex = 0; vertexIndex < mesh.vertices.size(); ++vertexIndex)
            {
                processedVertices[vertexIndex] =
                    VertexProcessor::process(
                        mesh.vertices[vertexIndex],
                        obj.transform,
                        *camera,
                        m_viewport
                    );

                if (m_settings.shadingMode == ShadingMode::Gouraud)
                {
                    Color baseColor = material.diffuse;

                    if (texture != nullptr)
                    {
                        baseColor = texture->sample(processedVertices[vertexIndex].uv);
                    }

                    processedVertices[vertexIndex].color =
                        LightingModel::evaluate(
                            processedVertices[vertexIndex].worldPosition,
                            processedVertices[vertexIndex].worldNormal,
                            baseColor,
                            material,
                            *camera,
                            scene.lights
                        );
                }
            }

            // Rasterización usando triángulos precomputados del mesh.
            for (size_t triangleIndex = 0; triangleIndex < mesh.triangleCount(); ++triangleIndex)
            {
                const MeshTriangle& tri = mesh.getIndexedTriangle(triangleIndex);

                const ScreenVertex& sv0 = processedVertices[tri.i0];
                const ScreenVertex& sv1 = processedVertices[tri.i1];
                const ScreenVertex& sv2 = processedVertices[tri.i2];

                rasterizer.drawTriangle(
                    m_colorBuffer,
                    m_depthBuffer,
                    m_viewport,
                    m_settings,
                    *camera,
                    scene.lights,
                    material,
                    texture,
                    sv0,
                    sv1,
                    sv2
                );
            }
        }
    }
}