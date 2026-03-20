#pragma once

#include "Mesh.h"
#include <QString>

class ObjLoader
{
public:
    struct FaceIndex   // ← MOVER AQUÍ
    {
        int v = -1;
        int vn = -1;
    };

    static Mesh loadFromFile(const QString& path, bool* ok = nullptr);

private:
    static bool parseFaceToken(const QString& token, FaceIndex& outIndex);
};