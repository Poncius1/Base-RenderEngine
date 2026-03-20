#include "Mesh.h"

Mesh MeshFactory::buildCube(float hx, float hy, float hz)
{
    Mesh mesh;
    auto& tris = mesh.triangles;
    tris.reserve(12);

    // ============================================
    // Colores base por cara
    // ============================================
    const Vec3 red = { 1.0f, 0.0f, 0.0f };
    const Vec3 green = { 0.0f, 1.0f, 0.0f };
    const Vec3 blue = { 0.0f, 0.0f, 1.0f };
    const Vec3 yellow = { 1.0f, 1.0f, 0.0f };
    const Vec3 cyan = { 0.0f, 1.0f, 1.0f };
    const Vec3 magenta = { 1.0f, 0.0f, 1.0f };

    // ============================================
    // Helper para construir un Vertex
    // ============================================
    auto makeVertex = [](const Vec3& p, const Vec3& n, const Vec3& c) -> Vertex
        {
            Vertex v;
            v.position = p;
            v.normal = n;
            v.color = c;
            return v;
        };

    // ============================================
    // Esquinas del cuboide
    // Convención:
    // pXYZ donde:
    // X = 0 -> -hx, 1 -> +hx
    // Y = 0 -> -hy, 1 -> +hy
    // Z = 0 -> -hz, 1 -> +hz
    // ============================================
    const Vec3 p000 = { -hx, -hy, -hz };
    const Vec3 p100 = { hx, -hy, -hz };
    const Vec3 p110 = { hx,  hy, -hz };
    const Vec3 p010 = { -hx,  hy, -hz };

    const Vec3 p001 = { -hx, -hy,  hz };
    const Vec3 p101 = { hx, -hy,  hz };
    const Vec3 p111 = { hx,  hy,  hz };
    const Vec3 p011 = { -hx,  hy,  hz };

    // ============================================
    // Normales por cara
    // ============================================
    const Vec3 nFront = { 0.0f,  0.0f,  1.0f };
    const Vec3 nBack = { 0.0f,  0.0f, -1.0f };
    const Vec3 nLeft = { -1.0f,  0.0f,  0.0f };
    const Vec3 nRight = { 1.0f,  0.0f,  0.0f };
    const Vec3 nTop = { 0.0f,  1.0f,  0.0f };
    const Vec3 nBottom = { 0.0f, -1.0f,  0.0f };

    // =========================================================
    // 6 caras * 2 triángulos = 12 triángulos
    // =========================================================

    // Cara frontal (+Z) - rojo
    tris.push_back({
        makeVertex(p001, nFront, red),
        makeVertex(p101, nFront, red),
        makeVertex(p111, nFront, red)
        });
    tris.push_back({
        makeVertex(p001, nFront, red),
        makeVertex(p111, nFront, red),
        makeVertex(p011, nFront, red)
        });

    // Cara trasera (-Z) - verde
    tris.push_back({
        makeVertex(p100, nBack, green),
        makeVertex(p000, nBack, green),
        makeVertex(p010, nBack, green)
        });
    tris.push_back({
        makeVertex(p100, nBack, green),
        makeVertex(p010, nBack, green),
        makeVertex(p110, nBack, green)
        });

    // Cara izquierda (-X) - azul
    tris.push_back({
        makeVertex(p000, nLeft, blue),
        makeVertex(p001, nLeft, blue),
        makeVertex(p011, nLeft, blue)
        });
    tris.push_back({
        makeVertex(p000, nLeft, blue),
        makeVertex(p011, nLeft, blue),
        makeVertex(p010, nLeft, blue)
        });

    // Cara derecha (+X) - amarillo
    tris.push_back({
        makeVertex(p101, nRight, yellow),
        makeVertex(p100, nRight, yellow),
        makeVertex(p110, nRight, yellow)
        });
    tris.push_back({
        makeVertex(p101, nRight, yellow),
        makeVertex(p110, nRight, yellow),
        makeVertex(p111, nRight, yellow)
        });

    // Cara superior (+Y) - cyan
    tris.push_back({
        makeVertex(p010, nTop, cyan),
        makeVertex(p011, nTop, cyan),
        makeVertex(p111, nTop, cyan)
        });
    tris.push_back({
        makeVertex(p010, nTop, cyan),
        makeVertex(p111, nTop, cyan),
        makeVertex(p110, nTop, cyan)
        });

    // Cara inferior (-Y) - magenta
    tris.push_back({
        makeVertex(p000, nBottom, magenta),
        makeVertex(p100, nBottom, magenta),
        makeVertex(p101, nBottom, magenta)
        });
    tris.push_back({
        makeVertex(p000, nBottom, magenta),
        makeVertex(p101, nBottom, magenta),
        makeVertex(p001, nBottom, magenta)
        });

    return mesh;
}