#include "renderer/utils/procedural_texture.h"

#include <algorithm>
#include <cmath>

namespace gfx
{
    // Limita valores al rango válido de color.
    static float clamp01(float v)
    {
        return std::max(0.0f, std::min(1.0f, v));
    }

    // Mezcla lineal entre dos colores.
    static Color lerpColor(const Color& a, const Color& b, float t)
    {
        t = clamp01(t);

        return
        {
            a.r * (1.0f - t) + b.r * t,
            a.g * (1.0f - t) + b.g * t,
            a.b * (1.0f - t) + b.b * t,
            1.0f
        };
    }

    // Textura procedural A: verde suave.
    static Color proceduralA(const Vec3& n)
    {
        const Vec3 normal = normalize(n);

        // Usa la normal para generar una variación simple sin depender de UVs.
        const float t = clamp01(normal.y * 0.5f + 0.5f);

        const Color base = { 0.58f, 0.92f, 0.52f, 1.0f };
        const Color light = { 0.82f, 1.00f, 0.76f, 1.0f };

        return lerpColor(base, light, t);
    }

    // Textura procedural B: azul/perla claro.
    static Color proceduralB(const Vec3& n)
    {
        const Vec3 normal = normalize(n);

        
        const float t = clamp01(normal.y * 0.5f + 0.5f);

        const Color base = { 0.72f, 0.78f, 0.96f, 1.0f };
        const Color light = { 0.95f, 0.97f, 1.0f, 1.0f };

        return lerpColor(base, light, t);
    }

    // Selecciona la textura procedural según el material activo.
    Color proceduralTextureForMaterial(const Material& material, const Vec3& normal)
    {
        
        if (material.shininess > 60.0f)
        {
            return proceduralB(normal);
        }

        return proceduralA(normal);
    }
}