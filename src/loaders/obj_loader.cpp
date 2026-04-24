#include "loaders/obj_loader.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>

#include "core/math/vec2.h"
#include "core/math/vec3.h"

namespace gfx
{
    namespace
    {
        // Clave para reutilizar vértices ya construidos.
        struct VertexKey
        {
            int positionIndex = -1;
            int texCoordIndex = -1;
            int normalIndex = -1;

            bool operator==(const VertexKey& other) const
            {
                return positionIndex == other.positionIndex
                    && texCoordIndex == other.texCoordIndex
                    && normalIndex == other.normalIndex;
            }
        };

        struct VertexKeyHasher
        {
            std::size_t operator()(const VertexKey& key) const
            {
                const std::size_t h1 = std::hash<int>{}(key.positionIndex);
                const std::size_t h2 = std::hash<int>{}(key.texCoordIndex);
                const std::size_t h3 = std::hash<int>{}(key.normalIndex);

                return h1 ^ (h2 << 1) ^ (h3 << 2);
            }
        };

        // Convierte índice OBJ 1-based a índice 0-based.
        // También soporta índices negativos relativos al final del arreglo.
        int resolveObjIndex(int objIndex, int elementCount)
        {
            if (objIndex > 0)
            {
                return objIndex - 1;
            }

            if (objIndex < 0)
            {
                return elementCount + objIndex;
            }

            return -1;
        }

        // Calcula una normal de respaldo para una cara si el OBJ no trae normales.
        Vec3 computeFallbackNormal(const Vec3& a, const Vec3& b, const Vec3& c)
        {
            const Vec3 e1 = b - a;
            const Vec3 e2 = c - a;
            return normalize(cross(e1, e2));
        }
    }

    Mesh ObjLoader::loadFromFile(const std::string& filePath)
    {
        Mesh mesh;

        std::ifstream file(filePath);
        if (!file.is_open())
        {
            return mesh;
        }

        std::vector<Vec3> positions;
        std::vector<Vec2> texCoords;
        std::vector<Vec3> normals;

        std::unordered_map<VertexKey, uint32_t, VertexKeyHasher> vertexCache;

        std::string line;
        while (std::getline(file, line))
        {
            if (line.empty() || line[0] == '#')
            {
                continue;
            }

            std::istringstream lineStream(line);
            std::string prefix;
            lineStream >> prefix;

            if (prefix == "v")
            {
                Vec3 position{};
                lineStream >> position.x >> position.y >> position.z;
                positions.push_back(position);
            }
            else if (prefix == "vt")
            {
                Vec2 uv{};
                lineStream >> uv.x >> uv.y;
                texCoords.push_back(uv);
            }
            else if (prefix == "vn")
            {
                Vec3 normal{};
                lineStream >> normal.x >> normal.y >> normal.z;
                normals.push_back(normalize(normal));
            }
            else if (prefix == "f")
            {
                std::vector<FaceIndex> faceIndices;
                std::string token;

                while (lineStream >> token)
                {
                    FaceIndex faceIndex;
                    if (parseFaceToken(token, faceIndex))
                    {
                        faceIndices.push_back(faceIndex);
                    }
                }

                if (faceIndices.size() < 3)
                {
                    continue;
                }

                // Normal fallback a partir de los primeros tres vértices de la cara.
                const int i0 = resolveObjIndex(faceIndices[0].positionIndex, static_cast<int>(positions.size()));
                const int i1 = resolveObjIndex(faceIndices[1].positionIndex, static_cast<int>(positions.size()));
                const int i2 = resolveObjIndex(faceIndices[2].positionIndex, static_cast<int>(positions.size()));

                if (i0 < 0 || i1 < 0 || i2 < 0)
                {
                    continue;
                }

                const Vec3 fallbackNormal = computeFallbackNormal(
                    positions[static_cast<size_t>(i0)],
                    positions[static_cast<size_t>(i1)],
                    positions[static_cast<size_t>(i2)]
                );

                // Triangulación en abanico.
                for (size_t i = 1; i + 1 < faceIndices.size(); ++i)
                {
                    const FaceIndex triFace[3] =
                    {
                        faceIndices[0],
                        faceIndices[i],
                        faceIndices[i + 1]
                    };

                    for (int corner = 0; corner < 3; ++corner)
                    {
                        const VertexKey key
                        {
                            triFace[corner].positionIndex,
                            triFace[corner].texCoordIndex,
                            triFace[corner].normalIndex
                        };

                        auto it = vertexCache.find(key);
                        if (it != vertexCache.end())
                        {
                            mesh.indices.push_back(it->second);
                            continue;
                        }

                        const Vertex vertex = buildVertex(
                            triFace[corner],
                            positions,
                            texCoords,
                            normals,
                            fallbackNormal
                        );

                        const uint32_t newIndex = static_cast<uint32_t>(mesh.vertices.size());
                        mesh.vertices.push_back(vertex);
                        mesh.indices.push_back(newIndex);
                        vertexCache.emplace(key, newIndex);
                    }
                }
            }
        }
        mesh.finalize();
        return mesh;
    }

    bool ObjLoader::parseFaceToken(const std::string& token, FaceIndex& outIndex)
    {
        outIndex = {};

        std::stringstream tokenStream(token);
        std::string part;
        std::vector<std::string> parts;

        while (std::getline(tokenStream, part, '/'))
        {
            parts.push_back(part);
        }

        if (parts.empty() || parts[0].empty())
        {
            return false;
        }

        outIndex.positionIndex = std::stoi(parts[0]);

        if (parts.size() > 1 && !parts[1].empty())
        {
            outIndex.texCoordIndex = std::stoi(parts[1]);
        }

        if (parts.size() > 2 && !parts[2].empty())
        {
            outIndex.normalIndex = std::stoi(parts[2]);
        }

        return true;
    }

    Vertex ObjLoader::buildVertex(
        const FaceIndex& faceIndex,
        const std::vector<Vec3>& positions,
        const std::vector<Vec2>& texCoords,
        const std::vector<Vec3>& normals,
        const Vec3& fallbackNormal)
    {
        Vertex vertex{};

        const int positionIndex = resolveObjIndex(faceIndex.positionIndex, static_cast<int>(positions.size()));
        if (positionIndex >= 0 && positionIndex < static_cast<int>(positions.size()))
        {
            vertex.position = positions[static_cast<size_t>(positionIndex)];
        }

        const int texCoordIndex = resolveObjIndex(faceIndex.texCoordIndex, static_cast<int>(texCoords.size()));
        if (texCoordIndex >= 0 && texCoordIndex < static_cast<int>(texCoords.size()))
        {
            vertex.uv = texCoords[static_cast<size_t>(texCoordIndex)];
        }

        const int normalIndex = resolveObjIndex(faceIndex.normalIndex, static_cast<int>(normals.size()));
        if (normalIndex >= 0 && normalIndex < static_cast<int>(normals.size()))
        {
            vertex.normal = normals[static_cast<size_t>(normalIndex)];
        }
        else
        {
            vertex.normal = fallbackNormal;
        }

        return vertex;
    }
}