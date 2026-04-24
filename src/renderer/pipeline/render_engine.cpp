#include "renderer/pipeline/render_engine.h"

#include <algorithm>
#include <cmath>
#include <vector>
#include "renderer/utils/procedural_texture.h"
#include "renderer/stages/rasterizer.h"
#include "renderer/stages/vertex_processor.h"

namespace gfx
{
    namespace
    {
        //umbral para considerar un mesh pesado
        constexpr size_t kHeavyMeshTriangleCount = 3000;

    }

    void RenderEngine::render(const Scene& scene)
    {
        //inicializa buffers por frame
        beginFrame();

        //obtiene camara activa
        const Camera* camera = scene.activeCamera();
        if (!camera || !m_viewport.isValid())
        {
            return;
        }

        Rasterizer rasterizer;

        //recorre todos los objetos de la escena
        for (const Object3D& obj : scene.objects)
        {
            //omite objetos invisibles o sin mesh
            if (!obj.visible || obj.mesh.empty())
            {
                continue;
            }

            const Mesh& mesh = obj.mesh;
            const Material& material = obj.currentMaterial();
            const Texture* texture = obj.currentTexture();

            //copia settings globales para modificarlos por objeto
            RenderSettings objectSettings = m_settings;

            const bool isHeavyMesh = mesh.triangleCount() >= kHeavyMeshTriangleCount;

            //ajustes para modelos grandes
            if (isHeavyMesh)
            {
                objectSettings.shadingMode = ShadingMode::Gouraud;
                objectSettings.backFaceCullingEnabled = false;
                objectSettings.depthTestEnabled = true;
            }

            //construye matrices de transformacion
            const Mat4 model = obj.transform.toMatrix();
            const Mat4 view = VertexProcessor::makeView(*camera);
            const Mat4 projection = VertexProcessor::makeProjection(*camera);
            const Mat4 mvp = projection * view * model;

            //cache de vertices procesados
            std::vector<ScreenVertex> processedVertices;
            processedVertices.resize(mesh.vertices.size());

            //procesamiento de vertices
            for (size_t vertexIndex = 0; vertexIndex < mesh.vertices.size(); ++vertexIndex)
            {
                processedVertices[vertexIndex] =
                    VertexProcessor::process(
                        mesh.vertices[vertexIndex],
                        model,
                        mvp,
                        *camera,
                        m_viewport
                    );

                //gouraud calcula iluminacion por vertice
                if (objectSettings.shadingMode == ShadingMode::Gouraud)
                {
                    Color baseColor = material.diffuse;

                    //usa textura procedural si esta activa
                    if (material.useTexture)
                    {
                        baseColor = proceduralTextureForMaterial(
                            material,
                            processedVertices[vertexIndex].worldNormal
                        );
                    }

                    //calcula iluminacion
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

            //rasterizacion de triangulos
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
                    objectSettings,
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