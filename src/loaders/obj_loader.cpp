#include "loaders/obj_loader.h"
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include "core/math/vec2.h"
#include "core/math/vec3.h"

namespace gfx
{
    namespace
    {
        //clave unica para evitar duplicar vertices
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

        //hash para usar vertexkey en unordered_map
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

        //convierte indices obj a indices base cero
        int resolveObjIndex(int objIndex, int elementCount)
        {
            if (objIndex > 0) return objIndex - 1;
            if (objIndex < 0) return elementCount + objIndex;
            return -1;
        }

        //calcula normal si el obj no trae normales validas
        Vec3 computeFallbackNormal(const Vec3& a, const Vec3& b, const Vec3& c)
        {
            return normalize(cross(b - a, c - a));
        }

        //avanza espacios en parsing
        void skipSpaces(const char*& p)
        {
            while (*p == ' ' || *p == '\t') ++p;
        }

        //parseo rapido de enteros
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

        //parseo rapido de floats
        float parseFloatFast(const char*& p)
        {
            skipSpaces(p);

            char* end = nullptr;
            const float value = std::strtof(p, &end);
            p = end;

            return value;
        }

        //parsea un token de cara v/vt/vn
        bool parseFaceTokenFast(const char*& p, ObjLoader::FaceIndex& outIndex)
        {
            skipSpaces(p);

            if (*p == '\0' || *p == '\r' || *p == '\n')
            {
                return false;
            }

            outIndex = {};
            outIndex.positionIndex = parseIntFast(p);

            if (*p == '/')
            {
                ++p;

                if (*p != '/')
                {
                    outIndex.texCoordIndex = parseIntFast(p);
                }

                if (*p == '/')
                {
                    ++p;
                    outIndex.normalIndex = parseIntFast(p);
                }
            }

            while (*p != '\0' && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n')
            {
                ++p;
            }

            return outIndex.positionIndex != 0;
        }

        //construye un vertice completo
        Vertex buildVertexFast(
            const ObjLoader::FaceIndex& faceIndex,
            const std::vector<Vec3>& positions,
            const std::vector<Vec2>& texCoords,
            const std::vector<Vec3>& normals,
            const Vec3& fallbackNormal)
        {
            Vertex vertex{};

            const int positionIndex = resolveObjIndex(
                faceIndex.positionIndex,
                static_cast<int>(positions.size())
            );

            if (positionIndex >= 0 && positionIndex < static_cast<int>(positions.size()))
            {
                vertex.position = positions[static_cast<size_t>(positionIndex)];
            }

            const int texCoordIndex = resolveObjIndex(
                faceIndex.texCoordIndex,
                static_cast<int>(texCoords.size())
            );

            if (texCoordIndex >= 0 && texCoordIndex < static_cast<int>(texCoords.size()))
            {
                vertex.uv = texCoords[static_cast<size_t>(texCoordIndex)];
            }

            const int normalIndex = resolveObjIndex(
                faceIndex.normalIndex,
                static_cast<int>(normals.size())
            );

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

        //cache para no duplicar vertices
        uint32_t getOrCreateVertex(
            const ObjLoader::FaceIndex& faceIndex,
            const std::vector<Vec3>& positions,
            const std::vector<Vec2>& texCoords,
            const std::vector<Vec3>& normals,
            const Vec3& fallbackNormal,
            std::unordered_map<VertexKey, uint32_t, VertexKeyHasher>& vertexCache,
            Mesh& mesh)
        {
            const VertexKey key
            {
                faceIndex.positionIndex,
                faceIndex.texCoordIndex,
                faceIndex.normalIndex
            };

            const auto it = vertexCache.find(key);
            if (it != vertexCache.end())
            {
                return it->second;
            }

            const Vertex vertex = buildVertexFast(
                faceIndex,
                positions,
                texCoords,
                normals,
                fallbackNormal
            );

            const uint32_t index = static_cast<uint32_t>(mesh.vertices.size());
            mesh.vertices.push_back(vertex);
            vertexCache.emplace(key, index);

            return index;
        }

        //normaliza el modelo para mantener escala consistente
        void normalizeMesh(Mesh& mesh, float targetSize)
        {
            if (mesh.vertices.empty())
            {
                return;
            }

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

            const Vec3 size =
            {
                maxP.x - minP.x,
                maxP.y - minP.y,
                maxP.z - minP.z
            };

            const float maxSize = std::max({ size.x, size.y, size.z });

            if (maxSize <= 0.00001f)
            {
                return;
            }

            const float scale = targetSize / maxSize;

            const float centerX = (minP.x + maxP.x) * 0.5f;
            const float centerZ = (minP.z + maxP.z) * 0.5f;
            const float bottomY = minP.y;

            for (Vertex& v : mesh.vertices)
            {
                v.position.x = (v.position.x - centerX) * scale;
                v.position.y = (v.position.y - bottomY) * scale;
                v.position.z = (v.position.z - centerZ) * scale;
            }
        }
    }

    //carga principal del archivo obj
    Mesh ObjLoader::loadFromFile(const std::string& filePath)
    {
        Mesh mesh;

        std::ifstream file(filePath, std::ios::binary);
        if (!file.is_open())
        {
            return mesh;
        }

        std::vector<Vec3> positions;
        std::vector<Vec2> texCoords;
        std::vector<Vec3> normals;

        //reservas para reducir realocaciones
        positions.reserve(100000);
        texCoords.reserve(100000);
        normals.reserve(100000);
        mesh.vertices.reserve(100000);
        mesh.indices.reserve(300000);

        std::unordered_map<VertexKey, uint32_t, VertexKeyHasher> vertexCache;
        vertexCache.reserve(100000);

        std::string line;
        line.reserve(512);

        std::vector<FaceIndex> faceIndices;
        faceIndices.reserve(8);

        while (std::getline(file, line))
        {
            if (line.empty() || line[0] == '#')
            {
                continue;
            }

            const char* p = line.c_str();

            //posiciones
            if (p[0] == 'v' && (p[1] == ' ' || p[1] == '\t'))
            {
                p += 2;

                Vec3 position{};
                position.x = parseFloatFast(p);
                position.y = parseFloatFast(p);
                position.z = parseFloatFast(p);

                positions.push_back(position);
            }
            //coordenadas uv
            else if (p[0] == 'v' && p[1] == 't' && (p[2] == ' ' || p[2] == '\t'))
            {
                p += 3;

                Vec2 uv{};
                uv.x = parseFloatFast(p);
                uv.y = parseFloatFast(p);

                texCoords.push_back(uv);
            }
            //normales
            else if (p[0] == 'v' && p[1] == 'n' && (p[2] == ' ' || p[2] == '\t'))
            {
                p += 3;

                Vec3 normal{};
                normal.x = parseFloatFast(p);
                normal.y = parseFloatFast(p);
                normal.z = parseFloatFast(p);

                normals.push_back(normalize(normal));
            }
            //caras
            else if (p[0] == 'f' && (p[1] == ' ' || p[1] == '\t'))
            {
                p += 2;
                faceIndices.clear();

                FaceIndex faceIndex{};
                while (parseFaceTokenFast(p, faceIndex))
                {
                    faceIndices.push_back(faceIndex);
                    skipSpaces(p);

                    if (*p == '\0' || *p == '\r' || *p == '\n')
                    {
                        break;
                    }
                }

                if (faceIndices.size() < 3)
                {
                    continue;
                }

                const int i0 = resolveObjIndex(
                    faceIndices[0].positionIndex,
                    static_cast<int>(positions.size())
                );

                const int i1 = resolveObjIndex(
                    faceIndices[1].positionIndex,
                    static_cast<int>(positions.size())
                );

                const int i2 = resolveObjIndex(
                    faceIndices[2].positionIndex,
                    static_cast<int>(positions.size())
                );

                if (i0 < 0 || i1 < 0 || i2 < 0 ||
                    i0 >= static_cast<int>(positions.size()) ||
                    i1 >= static_cast<int>(positions.size()) ||
                    i2 >= static_cast<int>(positions.size()))
                {
                    continue;
                }

                const Vec3 fallbackNormal = computeFallbackNormal(
                    positions[static_cast<size_t>(i0)],
                    positions[static_cast<size_t>(i1)],
                    positions[static_cast<size_t>(i2)]
                );

                //triangulacion en abanico
                for (size_t i = 1; i + 1 < faceIndices.size(); ++i)
                {
                    const uint32_t a = getOrCreateVertex(
                        faceIndices[0],
                        positions,
                        texCoords,
                        normals,
                        fallbackNormal,
                        vertexCache,
                        mesh
                    );

                    const uint32_t b = getOrCreateVertex(
                        faceIndices[i],
                        positions,
                        texCoords,
                        normals,
                        fallbackNormal,
                        vertexCache,
                        mesh
                    );

                    const uint32_t c = getOrCreateVertex(
                        faceIndices[i + 1],
                        positions,
                        texCoords,
                        normals,
                        fallbackNormal,
                        vertexCache,
                        mesh
                    );

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

    bool ObjLoader::parseFaceToken(const std::string& token, FaceIndex& outIndex)
    {
        const char* p = token.c_str();
        return parseFaceTokenFast(p, outIndex);
    }

    Vertex ObjLoader::buildVertex(
        const FaceIndex& faceIndex,
        const std::vector<Vec3>& positions,
        const std::vector<Vec2>& texCoords,
        const std::vector<Vec3>& normals,
        const Vec3& fallbackNormal)
    {
        return buildVertexFast(
            faceIndex,
            positions,
            texCoords,
            normals,
            fallbackNormal
        );
    }
}