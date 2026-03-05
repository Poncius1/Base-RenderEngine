#pragma once
#include "Matematicas.h"

// Cámara básica LookAt
struct Camera
{
    Vec3 eye{ 0, 0, 60 };
    Vec3 center{ 0, 0, 0 };
    Vec3 up{ 0, 1, 0 };
};

// View matrix (LookAt) estilo OpenGL (mira hacia -Z)
Mat4 makeView(const Camera& c);