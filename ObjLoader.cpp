#include "ObjLoader.h"

#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QDebug>

namespace
{
    // Construye un Vertex usando índices del OBJ
    Vertex makeVertexFromFace(
        const QVector<Vec3>& positions,
        const QVector<Vec3>& normals,
        const ObjLoader::FaceIndex& idx,
        const Vec3& fallbackNormal,
        const Vec3& color)
    {
        Vertex v{};

        // Posición
        if (idx.v >= 0 && idx.v < positions.size())
            v.position = positions[idx.v];

        // Normal (si no hay, usamos normal de cara)
        if (idx.vn >= 0 && idx.vn < normals.size())
            v.normal = normals[idx.vn];
        else
            v.normal = fallbackNormal;

        v.color = color;
        return v;
    }

    // Calcula normal de cara (cross product)
    Vec3 computeFaceNormal(const Vec3& a, const Vec3& b, const Vec3& c)
    {
        const Vec3 e1 = sub(b, a);
        const Vec3 e2 = sub(c, a);
        return normalize(cross(e1, e2));
    }

    // Asigna un color simple por cara (cíclico)
    Vec3 colorForFace(int faceIndex)
    {
        static const Vec3 colors[] = {
            {1.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 1.0f},
            {1.0f, 1.0f, 0.0f},
            {0.0f, 1.0f, 1.0f},
            {1.0f, 0.0f, 1.0f}
        };

        return colors[faceIndex % 6];
    }
}

// Parsea un token de cara: "v", "v//n", "v/vt/n"
bool ObjLoader::parseFaceToken(const QString& token, FaceIndex& outIndex)
{
    outIndex = {};

    const QStringList parts = token.split('/');

    if (parts.isEmpty())
        return false;

    // Índice de posición (OBLIGATORIO)
    bool okV = false;
    const int vIndex = parts[0].toInt(&okV);
    if (!okV || vIndex == 0)
        return false;

    outIndex.v = vIndex - 1; // OBJ es 1-based

    // Índice de normal (opcional)
    if (parts.size() >= 3 && !parts[2].isEmpty())
    {
        bool okN = false;
        const int nIndex = parts[2].toInt(&okN);
        if (okN && nIndex != 0)
            outIndex.vn = nIndex - 1;
    }

    return true;
}

Mesh ObjLoader::loadFromFile(const QString& path, bool* ok)
{
    if (ok)
        *ok = false;

    Mesh mesh;

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "No se pudo abrir el OBJ:" << path;
        return mesh;
    }

    QVector<Vec3> positions; // v
    QVector<Vec3> normals;   // vn

    QTextStream in(&file);
    int faceCounter = 0;

    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();

        // Ignorar líneas vacías o comentarios
        if (line.isEmpty() || line.startsWith('#'))
            continue;

        const QStringList tokens = line.split(' ', Qt::SkipEmptyParts);
        if (tokens.isEmpty())
            continue;

        const QString& tag = tokens[0];

        // -------------------------
        // v (posición)
        // -------------------------
        if (tag == "v")
        {
            if (tokens.size() < 4)
                continue;

            bool okX = false, okY = false, okZ = false;
            const float x = tokens[1].toFloat(&okX);
            const float y = tokens[2].toFloat(&okY);
            const float z = tokens[3].toFloat(&okZ);

            if (okX && okY && okZ)
                positions.push_back({ x, y, z });
        }

        // -------------------------
        // vn (normal)
        // -------------------------
        else if (tag == "vn")
        {
            if (tokens.size() < 4)
                continue;

            bool okX = false, okY = false, okZ = false;
            const float x = tokens[1].toFloat(&okX);
            const float y = tokens[2].toFloat(&okY);
            const float z = tokens[3].toFloat(&okZ);

            if (okX && okY && okZ)
                normals.push_back(normalize(Vec3{ x, y, z }));
        }

        // -------------------------
        // f (cara)
        // -------------------------
        else if (tag == "f")
        {
            if (tokens.size() < 4)
                continue;

            QVector<FaceIndex> face;
            face.reserve(tokens.size() - 1);

            // Parsear cada vértice de la cara
            for (int i = 1; i < tokens.size(); ++i)
            {
                FaceIndex idx;
                if (!parseFaceToken(tokens[i], idx))
                {
                    face.clear();
                    break;
                }
                face.push_back(idx);
            }

            if (face.size() < 3)
                continue;

            const Vec3 faceColor = colorForFace(faceCounter++);

            // Normal fallback (si no hay vn)
            const Vec3 a = positions[face[0].v];
            const Vec3 b = positions[face[1].v];
            const Vec3 c = positions[face[2].v];
            const Vec3 fallbackNormal = computeFaceNormal(a, b, c);

            // Construye un triángulo a partir de 3 índices
            auto buildTriangle = [&](const FaceIndex& i0, const FaceIndex& i1, const FaceIndex& i2)
                {
                    Triangle tri;
                    tri.v0 = makeVertexFromFace(positions, normals, i0, fallbackNormal, faceColor);
                    tri.v1 = makeVertexFromFace(positions, normals, i1, fallbackNormal, faceColor);
                    tri.v2 = makeVertexFromFace(positions, normals, i2, fallbackNormal, faceColor);
                    mesh.triangles.push_back(tri);
                };

            // Triángulo directo
            if (face.size() == 3)
            {
                buildTriangle(face[0], face[1], face[2]);
            }
            // Quad → 2 triángulos
            else if (face.size() == 4)
            {
                buildTriangle(face[0], face[1], face[2]);
                buildTriangle(face[0], face[2], face[3]);
            }
            // Polígono → triangulación en abanico
            else
            {
                for (int i = 1; i + 1 < face.size(); ++i)
                    buildTriangle(face[0], face[i], face[i + 1]);
            }
        }
    }

    if (ok)
        *ok = !mesh.triangles.isEmpty();

    if (mesh.triangles.isEmpty())
        qWarning() << "No se cargaron triángulos desde el OBJ:" << path;

    return mesh;
}