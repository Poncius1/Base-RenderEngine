#pragma once

#include <vector>
#include <cstdint>
#include <algorithm>

#include "core/math/vec2.h"
#include "core/math/vec3.h"

namespace gfx
{
    struct Vertex
    {
        Vec3 position;
        Vec3 normal;
        Vec2 uv;
    };

    // Triángulo indexado precomputado.
    struct MeshTriangle
    {
        uint32_t i0 = 0;
        uint32_t i1 = 0;
        uint32_t i2 = 0;

        // Normal local de la cara
        Vec3 localFaceNormal = { 0.0f, 0.0f, 1.0f };
    };

    // Bounding box de la malla.
    struct MeshBounds
    {
        Vec3 min = { 0.0f, 0.0f, 0.0f };
        Vec3 max = { 0.0f, 0.0f, 0.0f };
        Vec3 center = { 0.0f, 0.0f, 0.0f };
        Vec3 size = { 0.0f, 0.0f, 0.0f };
        float maxExtent = 1.0f;
    };

    class Mesh
    {
    public:
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        // Cachés precomputados.
        std::vector<MeshTriangle> triangles;
        MeshBounds bounds;

        size_t triangleCount() const
        {
            return triangles.size();
        }

        void getTriangle(size_t triangleIndex, Vertex& v0, Vertex& v1, Vertex& v2) const
        {
            const MeshTriangle& tri = triangles[triangleIndex];
            v0 = vertices[tri.i0];
            v1 = vertices[tri.i1];
            v2 = vertices[tri.i2];
        }

        const MeshTriangle& getIndexedTriangle(size_t triangleIndex) const
        {
            return triangles[triangleIndex];
        }

        bool empty() const
        {
            return vertices.empty() || indices.empty();
        }

        // Precomputa bounds y triángulos indexados.
        void finalize()
        {
            buildBounds();
            buildTriangleCache();
        }

    private:
        void buildBounds()
        {
            if (vertices.empty())
            {
                bounds = {};
                bounds.maxExtent = 1.0f;
                return;
            }

            Vec3 minPoint = vertices[0].position;
            Vec3 maxPoint = vertices[0].position;

            for (const Vertex& vertex : vertices)
            {
                minPoint.x = std::min(minPoint.x, vertex.position.x);
                minPoint.y = std::min(minPoint.y, vertex.position.y);
                minPoint.z = std::min(minPoint.z, vertex.position.z);

                maxPoint.x = std::max(maxPoint.x, vertex.position.x);
                maxPoint.y = std::max(maxPoint.y, vertex.position.y);
                maxPoint.z = std::max(maxPoint.z, vertex.position.z);
            }

            bounds.min = minPoint;
            bounds.max = maxPoint;
            bounds.center =
            {
                (minPoint.x + maxPoint.x) * 0.5f,
                (minPoint.y + maxPoint.y) * 0.5f,
                (minPoint.z + maxPoint.z) * 0.5f
            };

            bounds.size =
            {
                maxPoint.x - minPoint.x,
                maxPoint.y - minPoint.y,
                maxPoint.z - minPoint.z
            };

            bounds.maxExtent = std::max({ bounds.size.x, bounds.size.y, bounds.size.z, 1e-6f });
        }

        void buildTriangleCache()
        {
            triangles.clear();
            triangles.reserve(indices.size() / 3);

            for (size_t i = 0; i + 2 < indices.size(); i += 3)
            {
                MeshTriangle tri;
                tri.i0 = indices[i + 0];
                tri.i1 = indices[i + 1];
                tri.i2 = indices[i + 2];

                const Vec3& p0 = vertices[tri.i0].position;
                const Vec3& p1 = vertices[tri.i1].position;
                const Vec3& p2 = vertices[tri.i2].position;

                tri.localFaceNormal = normalize(cross(p1 - p0, p2 - p0));
                triangles.push_back(tri);
            }
        }
    };
}