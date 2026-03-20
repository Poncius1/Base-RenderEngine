#pragma once
#include "Matematicas.h"
#include <QVector>

// Representa un vértice del modelo
struct Vertex
{
    Vec3 position{}; // posición en espacio del objeto
    Vec3 normal{};   // normal asociada al vértice/cara
    Vec3 color{};    // color base
};

// Triángulo formado por 3 vértices
struct Triangle
{
    Vertex v0{};
    Vertex v1{};
    Vertex v2{};
};

// Malla genérica compuesta por triángulos
struct Mesh
{
    QVector<Triangle> triangles;
};

// Fábrica de mallas procedurales
class MeshFactory
{
public:
    // Genera un cubo/cuboide procedural
    // hx, hy, hz son mitad del cuboide en X, Y, Z
    static Mesh buildCube(float hx = 7.5f, float hy = 5.0f, float hz = 10.0f);
};