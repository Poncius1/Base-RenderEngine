#pragma once

#include <cmath>

#include "core/math/vec3.h"
#include "core/math/vec4.h"
#include "core/math/math_utils.h"

namespace gfx
{
    // Matriz 4x4 en formato fila-columna.
    // Es la base para transformaciones afines y proyecciones.
    struct Mat4
    {
        float m[4][4] = {};

        // Regresa la matriz identidad.
        static Mat4 identity()
        {
            Mat4 result{};
            result.m[0][0] = 1.0f;
            result.m[1][1] = 1.0f;
            result.m[2][2] = 1.0f;
            result.m[3][3] = 1.0f;
            return result;
        }
    };

    // Multiplicación de matrices 4x4.
    inline Mat4 operator*(const Mat4& a, const Mat4& b)
    {
        Mat4 result{};

        for (int row = 0; row < 4; ++row)
        {
            for (int col = 0; col < 4; ++col)
            {
                float sum = 0.0f;
                for (int k = 0; k < 4; ++k)
                {
                    sum += a.m[row][k] * b.m[k][col];
                }
                result.m[row][col] = sum;
            }
        }

        return result;
    }

    // Multiplicación matriz por vector homogéneo.
    inline Vec4 operator*(const Mat4& matrix, const Vec4& v)
    {
        return
        {
            matrix.m[0][0] * v.x + matrix.m[0][1] * v.y + matrix.m[0][2] * v.z + matrix.m[0][3] * v.w,
            matrix.m[1][0] * v.x + matrix.m[1][1] * v.y + matrix.m[1][2] * v.z + matrix.m[1][3] * v.w,
            matrix.m[2][0] * v.x + matrix.m[2][1] * v.y + matrix.m[2][2] * v.z + matrix.m[2][3] * v.w,
            matrix.m[3][0] * v.x + matrix.m[3][1] * v.y + matrix.m[3][2] * v.z + matrix.m[3][3] * v.w
        };
    }

    // Construye una matriz de traslación.
    inline Mat4 makeTranslation(float tx, float ty, float tz)
    {
        Mat4 result = Mat4::identity();
        result.m[0][3] = tx;
        result.m[1][3] = ty;
        result.m[2][3] = tz;
        return result;
    }

    // Construye una matriz de escalamiento.
    inline Mat4 makeScale(float sx, float sy, float sz)
    {
        Mat4 result{};
        result.m[0][0] = sx;
        result.m[1][1] = sy;
        result.m[2][2] = sz;
        result.m[3][3] = 1.0f;
        return result;
    }

    // Construye una rotación respecto al eje X.
    inline Mat4 makeRotationX(float radians)
    {
        Mat4 result = Mat4::identity();
        const float c = std::cos(radians);
        const float s = std::sin(radians);

        result.m[1][1] = c;
        result.m[1][2] = -s;
        result.m[2][1] = s;
        result.m[2][2] = c;

        return result;
    }

    // Construye una rotación respecto al eje Y.
    inline Mat4 makeRotationY(float radians)
    {
        Mat4 result = Mat4::identity();
        const float c = std::cos(radians);
        const float s = std::sin(radians);

        result.m[0][0] = c;
        result.m[0][2] = s;
        result.m[2][0] = -s;
        result.m[2][2] = c;

        return result;
    }

    // Construye una rotación respecto al eje Z.
    inline Mat4 makeRotationZ(float radians)
    {
        Mat4 result = Mat4::identity();
        const float c = std::cos(radians);
        const float s = std::sin(radians);

        result.m[0][0] = c;
        result.m[0][1] = -s;
        result.m[1][0] = s;
        result.m[1][1] = c;

        return result;
    }

    // Construye una matriz TRS.
    // Orden aplicado: traslación * rotaciónZ * rotaciónY * rotaciónX * escala.
    inline Mat4 makeTRS(const Vec3& translation, const Vec3& rotationRadians, const Vec3& scale)
    {
        const Mat4 t = makeTranslation(translation.x, translation.y, translation.z);
        const Mat4 rx = makeRotationX(rotationRadians.x);
        const Mat4 ry = makeRotationY(rotationRadians.y);
        const Mat4 rz = makeRotationZ(rotationRadians.z);
        const Mat4 s = makeScale(scale.x, scale.y, scale.z);

        return t * rz * ry * rx * s;
    }

    // Transforma un punto 3D usando coordenadas homogéneas.
    // Usa w=1 para que la traslación tenga efecto.
    inline Vec3 transformPoint(const Mat4& matrix, const Vec3& point)
    {
        const Vec4 result = matrix * Vec4(point.x, point.y, point.z, 1.0f);

        if (!nearlyEqual(result.w, 0.0f))
        {
            return { result.x / result.w, result.y / result.w, result.z / result.w };
        }

        return { result.x, result.y, result.z };
    }

    // Transforma un vector dirección.
    // Usa w=0 para ignorar la traslación.
    inline Vec3 transformVector(const Mat4& matrix, const Vec3& vector)
    {
        const Vec4 result = matrix * Vec4(vector.x, vector.y, vector.z, 0.0f);
        return { result.x, result.y, result.z };
    }
}