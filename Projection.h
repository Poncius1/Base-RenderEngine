#pragma once
#include "Matematicas.h"

enum class ProjectionType
{
    Ortho,
    Persp
};

struct Projection
{
    ProjectionType type = ProjectionType::Persp;

    // Persp
    float fovDeg = 60.0f;

    // Ortho
    float orthoHalfSize = 25.0f; // suficiente para ver 15x10x20 

    // Planos
    float nearPlane = 1.0f;
    float farPlane = 200.0f;
};

Mat4 makeProjection(const Projection& p, float aspect);