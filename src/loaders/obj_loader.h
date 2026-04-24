#pragma once

#include <string>

#include "scene/mesh.h"

namespace gfx
{
    // Cargador simple de archivos OBJ.
    // Soporta posiciones (v), coordenadas de textura (vt), normales (vn) y caras poligonales con triangulación en abanico.
    class ObjLoader
    {
    public:
        // Carga una malla desde un archivo OBJ.
        // Si existe un error, regresa una malla vacía.
        static Mesh loadFromFile(const std::string& filePath);

    private:
        // Índice de un vértice dentro de una cara OBJ.
        // OBJ usa índices separados para posición, UV y normal.
        struct FaceIndex
        {
            int positionIndex = -1;
            int texCoordIndex = -1;
            int normalIndex = -1;
        };

        // Parsea un token de cara, por ejemplo:
        // "1", "1/2", "1//3", "1/2/3"
        static bool parseFaceToken(const std::string& token, FaceIndex& outIndex);

        // Convierte índices OBJ a un Vertex del engine.
        static Vertex buildVertex(
            const FaceIndex& faceIndex,
            const std::vector<Vec3>& positions,
            const std::vector<Vec2>& texCoords,
            const std::vector<Vec3>& normals,
            const Vec3& fallbackNormal);
    };
}