#pragma once

#include <string>
#include <vector>

#include "scene/mesh.h"

namespace gfx
{
    class ObjLoader
    {
    public:
        struct FaceIndex
        {
            int positionIndex = -1;
            int texCoordIndex = -1;
            int normalIndex = -1;
        };

        static Mesh loadFromFile(const std::string& filePath);

    private:
        static bool parseFaceToken(const std::string& token, FaceIndex& outIndex);

        static Vertex buildVertex(
            const FaceIndex& faceIndex,
            const std::vector<Vec3>& positions,
            const std::vector<Vec2>& texCoords,
            const std::vector<Vec3>& normals,
            const Vec3& fallbackNormal);
    };
}