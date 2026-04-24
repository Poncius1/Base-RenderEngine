#pragma once

#include "core/math/mat4.h"
#include "core/math/vec3.h"
#include "core/math/vec4.h"
#include "scene/camera.h"
#include "scene/transform.h"
#include "renderer/pipeline/viewport.h"
#include "renderer/stages/rasterizer.h"

namespace gfx
{
    // Procesador de vértices.
    // Convierte un vértice de espacio local a espacio de pantalla.
    class VertexProcessor
    {
    public:
        static ScreenVertex process(
            const Vertex& vertex,
            const Transform& transform,
            const Camera& camera,
            const Viewport& viewport)
        {
            // 1. Modelo (local → mundo)
            const Mat4 model = transform.toMatrix();

            // 2. Vista
            const Mat4 view = makeView(camera);

            // 3. Proyección
            const Mat4 projection = makeProjection(camera);

            const Mat4 mvp = projection * view * model;

            // Posición en mundo
            const Vec3 worldPos = transformPoint(model, vertex.position);

            // Normal en mundo
            const Vec3 worldNormal = normalize(transformVector(model, vertex.normal));

            // Clip space
            const Vec4 clip = mvp * Vec4(vertex.position.x, vertex.position.y, vertex.position.z, 1.0f);

            // NDC
            Vec3 ndc{};
            if (!nearlyEqual(clip.w, 0.0f))
            {
                ndc = { clip.x / clip.w, clip.y / clip.w, clip.z / clip.w };
            }

            // Viewport transform
            const float sx = viewport.x + (ndc.x * 0.5f + 0.5f) * viewport.width;
            const float sy = viewport.y + (1.0f - (ndc.y * 0.5f + 0.5f)) * viewport.height;
            const float sz = ndc.z;

            ScreenVertex out{};
            out.screenPosition = { sx, sy, sz };
            out.worldPosition = worldPos;
            out.worldNormal = worldNormal;
            out.uv = vertex.uv;
            out.color = colors::White(); // se usa en Gouraud

            return out;
        }

    private:
        // Construye matriz de vista tipo lookAt.
        static Mat4 makeView(const Camera& cam)
        {
            const Vec3 z = normalize(cam.eye - cam.target);
            const Vec3 x = normalize(cross(cam.up, z));
            const Vec3 y = cross(z, x);

            Mat4 view = Mat4::identity();

            view.m[0][0] = x.x; view.m[0][1] = x.y; view.m[0][2] = x.z;
            view.m[1][0] = y.x; view.m[1][1] = y.y; view.m[1][2] = y.z;
            view.m[2][0] = z.x; view.m[2][1] = z.y; view.m[2][2] = z.z;

            view.m[0][3] = -dot(x, cam.eye);
            view.m[1][3] = -dot(y, cam.eye);
            view.m[2][3] = -dot(z, cam.eye);

            return view;
        }

        // Construye matriz de proyección.
        static Mat4 makeProjection(const Camera& cam)
        {
            Mat4 proj{};

            if (cam.projectionType == ProjectionType::Perspective)
            {
                const float f = 1.0f / std::tan(cam.fovYRadians * 0.5f);

                proj.m[0][0] = f;
                proj.m[1][1] = f;
                proj.m[2][2] = (cam.farPlane + cam.nearPlane) / (cam.nearPlane - cam.farPlane);
                proj.m[2][3] = (2.0f * cam.farPlane * cam.nearPlane) / (cam.nearPlane - cam.farPlane);
                proj.m[3][2] = -1.0f;
            }
            else
            {
                proj.m[0][0] = 2.0f / (cam.right - cam.left);
                proj.m[1][1] = 2.0f / (cam.top - cam.bottom);
                proj.m[2][2] = -2.0f / (cam.farPlane - cam.nearPlane);
                proj.m[3][3] = 1.0f;

                proj.m[0][3] = -(cam.right + cam.left) / (cam.right - cam.left);
                proj.m[1][3] = -(cam.top + cam.bottom) / (cam.top - cam.bottom);
                proj.m[2][3] = -(cam.farPlane + cam.nearPlane) / (cam.farPlane - cam.nearPlane);
            }

            return proj;
        }
    };
}