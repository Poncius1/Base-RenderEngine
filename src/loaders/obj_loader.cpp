#include "loaders/obj_loader.h"
#include <algorithm>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <cstdlib>

#include "core/math/vec2.h"
#include "core/math/vec3.h"

namespace gfx
{
    namespace
    {
        // Clave única para evitar duplicar vértices
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

        // Hash para usar VertexKey en unordered_map
        struct VertexKeyHasher
        {
            std::size_t operator()(const VertexKey& key) const
            {
                std::size_t h = 1469598103934665603ull;
                h ^= static_cast<std::size_t>(key.positionIndex + 0x9e3779b9); h *= 1099511628211ull;
                h ^= static_cast<std::size_t>(key.texCoordIndex + 0x9e3779b9); h *= 1099511628211ull;
                h ^= static_cast<std::size_t>(key.normalIndex + 0x9e3779b9); h *= 1099511628211ull;
                return h;
            }
        };

        // Convierte índices OBJ (1-based o negativos) a índices válidos
        int resolveObjIndex(int objIndex, int elementCount)
        {
            if (objIndex > 0) return objIndex - 1;
            if (objIndex < 0) return elementCount + objIndex;
            return -1;
        }

        // Calcula normal si el modelo no la tiene
        Vec3 computeFallbackNormal(const Vec3& a, const Vec3& b, const Vec3& c)
        {
            return normalize(cross(b - a, c - a));
        }

        // Avanza espacios en parsing
        void skipSpaces(const char*& p)
        {
            while (*p == ' ' || *p == '\t') ++p;
        }

        // Parseo rápido de enteros
        int parseIntFast(const char*& p)
        {
            skipSpaces(p);

            int sign = 1;
            if (*p == '-') { sign = -1; ++p; }
            else if (*p == '+') { ++p; }

            int value = 0;
            while (*p >= '0' && *p <= '9')
            {
                value = value * 10 + (*p - '0');
                ++p;
            }

            return value * sign;
        }

        // Parseo rápido de floats
        float parseFloatFast(const char*& p)
        {
            skipSpaces(p);
            char* end = nullptr;
            float value = std::strtof(p, &end);
            p = end;
            return value;
        }

        // Parsea un token de cara (v/vt/vn)
        bool parseFaceTokenFast(const char*& p, ObjLoader::FaceIndex& outIndex)
        {
            skipSpaces(p);

            if (*p == '\0' || *p == '\r' || *p == '\n')
                return false;

            outIndex = {};
            outIndex.positionIndex = parseIntFast(p);

            if (*p == '/')
            {
                ++p;

                if (*p != '/')
                    outIndex.texCoordIndex = parseIntFast(p);

                if (*p == '/')
                {
                    ++p;
                    outIndex.normalIndex = parseIntFast(p);
                }
            }

            // Avanza al siguiente token
            while (*p != '\0' && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n')
                ++p;

            return outIndex.positionIndex != 0;
        }

        // Construye un vértice completo
        Vertex buildVertexFast(
            const ObjLoader::FaceIndex& faceIndex,
            const std::vector<Vec3>& positions,
            const std::vector<Vec2>& texCoords,
            const std::vector<Vec3>& normals,
            const Vec3& fallbackNormal)
        {
            Vertex vertex{};

            int positionIndex = resolveObjIndex(faceIndex.positionIndex, positions.size());
            if (positionIndex >= 0 && positionIndex < (int)positions.size())
                vertex.position = positions[positionIndex];

            int texCoordIndex = resolveObjIndex(faceIndex.texCoordIndex, texCoords.size());
            if (texCoordIndex >= 0 && texCoordIndex < (int)texCoords.size())
                vertex.uv = texCoords[texCoordIndex];

            int normalIndex = resolveObjIndex(faceIndex.normalIndex, normals.size());
            if (normalIndex >= 0 && normalIndex < (int)normals.size())
                vertex.normal = normals[normalIndex];
            else
                vertex.normal = fallbackNormal;

            return vertex;
        }

        // Cache para no duplicar vértices
        uint32_t getOrCreateVertex(
            const ObjLoader::FaceIndex& faceIndex,
            const std::vector<Vec3>& positions,
            const std::vector<Vec2>& texCoords,
            const std::vector<Vec3>& normals,
            const Vec3& fallbackNormal,
            std::unordered_map<VertexKey, uint32_t, VertexKeyHasher>& vertexCache,
            Mesh& mesh)
        {
            VertexKey key{ faceIndex.positionIndex, faceIndex.texCoordIndex, faceIndex.normalIndex };

            auto it = vertexCache.find(key);
            if (it != vertexCache.end())
                return it->second;

            Vertex vertex = buildVertexFast(faceIndex, positions, texCoords, normals, fallbackNormal);

            uint32_t index = mesh.vertices.size();
            mesh.vertices.push_back(vertex);
            vertexCache.emplace(key, index);

            return index;
        }

        // Normaliza el modelo (centrado y escalado)
        void normalizeMesh(Mesh& mesh, float targetSize)
        {
            if (mesh.vertices.empty()) return;

            Vec3 minP = mesh.vertices[0].position;
            Vec3 maxP = mesh.vertices[0].position;

            for (const Vertex& v : mesh.vertices)
            {
                minP.x = std::min(minP.x, v.position.x);
                minP.y = std::min(minP.y, v.position.y);
                minP.z = std::min(minP.z, v.position.z);

                maxP.x = std::max(maxP.x, v.position.x);
                maxP.y = std::max(maxP.y, v.position.y);
                maxP.z = std::max(maxP.z, v.position.z);
            }

            Vec3 size = { maxP.x - minP.x, maxP.y - minP.y, maxP.z - minP.z };
            float maxSize = std::max({ size.x, size.y, size.z });

            if (maxSize <= 0.00001f) return;

            float scale = targetSize / maxSize;

            float centerX = (minP.x + maxP.x) * 0.5f;
            float centerZ = (minP.z + maxP.z) * 0.5f;
            float bottomY = minP.y;

            for (Vertex& v : mesh.vertices)
            {
                v.position.x = (v.position.x - centerX) * scale;
                v.position.y = (v.position.y - bottomY) * scale;
                v.position.z = (v.position.z - centerZ) * scale;
            }
        }
    }

    // Función principal de carga OBJ
    Mesh ObjLoader::loadFromFile(const std::string& filePath)
    {
        Mesh mesh;

        std::ifstream file(filePath, std::ios::binary);
        if (!file.is_open())
            return mesh;

        std::vector<Vec3> positions;
        std::vector<Vec2> texCoords;
        std::vector<Vec3> normals;

        // Reservas para performance
        positions.reserve(100000);
        texCoords.reserve(100000);
        normals.reserve(100000);
        mesh.vertices.reserve(100000);
        mesh.indices.reserve(300000);

        std::unordered_map<VertexKey, uint32_t, VertexKeyHasher> vertexCache;
        vertexCache.reserve(100000);

        std::string line;
        std::vector<FaceIndex> faceIndices;

        while (std::getline(file, line))
        {
            if (line.empty() || line[0] == '#') continue;

            const char* p = line.c_str();

            // Posiciones
            if (p[0] == 'v' && (p[1] == ' ' || p[1] == '\t'))
            {
                p += 2;
                positions.push_back({ parseFloatFast(p), parseFloatFast(p), parseFloatFast(p) });
            }
            // UVs
            else if (p[0] == 'v' && p[1] == 't')
            {
                p += 3;
                texCoords.push_back({ parseFloatFast(p), parseFloatFast(p) });
            }
            // Normales
            else if (p[0] == 'v' && p[1] == 'n')
            {
                p += 3;
                normals.push_back(normalize({ parseFloatFast(p), parseFloatFast(p), parseFloatFast(p) }));
            }
            // Caras
            else if (p[0] == 'f')
            {
                p += 2;
                faceIndices.clear();

                FaceIndex faceIndex{};
                while (parseFaceTokenFast(p, faceIndex))
                {
                    faceIndices.push_back(faceIndex);
                    skipSpaces(p);
                }

                if (faceIndices.size() < 3) continue;

                // Triangulación tipo abanico
                for (size_t i = 1; i + 1 < faceIndices.size(); ++i)
                {
                    uint32_t a = getOrCreateVertex(faceIndices[0], positions, texCoords, normals, {}, vertexCache, mesh);
                    uint32_t b = getOrCreateVertex(faceIndices[i], positions, texCoords, normals, {}, vertexCache, mesh);
                    uint32_t c = getOrCreateVertex(faceIndices[i + 1], positions, texCoords, normals, {}, vertexCache, mesh);

                    mesh.indices.push_back(a);
                    mesh.indices.push_back(b);
                    mesh.indices.push_back(c);
                }
            }
        }

        mesh.finalize();
        normalizeMesh(mesh, 2.6f);

        return mesh;
    }
}