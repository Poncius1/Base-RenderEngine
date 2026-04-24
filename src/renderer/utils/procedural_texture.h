#pragma once
#include "core/math/vec3.h"
#include "core/image/color.h"
#include "scene/material.h"

namespace gfx
{
    Color proceduralTextureForMaterial(const Material& material, const Vec3& normal);
}