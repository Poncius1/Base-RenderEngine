#pragma once

// Matemáticas para pipeline 3D

struct Vec3 { float x{}, y{}, z{}; };
struct Vec4 { float x{}, y{}, z{}, w{}; };

inline Vec3 V3(float x, float y, float z) { return { x,y,z }; }
inline Vec4 V4(float x, float y, float z, float w) { return { x,y,z,w }; }

inline Vec3 add(const Vec3& a, const Vec3& b) { return { a.x + b.x,a.y + b.y,a.z + b.z }; }
inline Vec3 sub(const Vec3& a, const Vec3& b) { return { a.x - b.x,a.y - b.y,a.z - b.z }; }
inline Vec3 mul(const Vec3& a, float s) { return { a.x * s,a.y * s,a.z * s }; }

inline float dot(const Vec3& a, const Vec3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
inline Vec3 cross(const Vec3& a, const Vec3& b)
{
    return { a.y * b.z - a.z * b.y,
             a.z * b.x - a.x * b.z,
             a.x * b.y - a.y * b.x };
}

//  wrappers (Matematicas.cpp)
float m_sqrt(float x);
float m_fabs(float x);
float m_tan(float x);
int   m_roundToInt(float x);
float m_degToRad(float deg);
float m_sin(float x);
float m_cos(float x);

inline float length(const Vec3& v) { return m_sqrt(dot(v, v)); }
inline Vec3 normalize(const Vec3& v)
{
    float l = length(v);
    if (l <= 1e-8f) return { 0,0,0 };
    return { v.x / l, v.y / l, v.z / l };
}

struct Mat4 { float m[4][4]{}; };

inline Mat4 mat4Identity()
{
    Mat4 r{};
    r.m[0][0] = 1.0f; r.m[1][1] = 1.0f; r.m[2][2] = 1.0f; r.m[3][3] = 1.0f;
    return r;
}

// Mat4 * Mat4
inline Mat4 mat4Mul(const Mat4& A, const Mat4& B)
{
    Mat4 R{};
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            float s = 0.0f;
            for (int k = 0; k < 4; k++) s += A.m[i][k] * B.m[k][j];
            R.m[i][j] = s;
        }
    }
    return R;
}

// Mat4 * Vec4  (vector columna)
inline Vec4 mat4MulVec4(const Mat4& M, const Vec4& v)
{
    Vec4 r{};
    r.x = M.m[0][0] * v.x + M.m[0][1] * v.y + M.m[0][2] * v.z + M.m[0][3] * v.w;
    r.y = M.m[1][0] * v.x + M.m[1][1] * v.y + M.m[1][2] * v.z + M.m[1][3] * v.w;
    r.z = M.m[2][0] * v.x + M.m[2][1] * v.y + M.m[2][2] * v.z + M.m[2][3] * v.w;
    r.w = M.m[3][0] * v.x + M.m[3][1] * v.y + M.m[3][2] * v.z + M.m[3][3] * v.w;
    return r;
}


//Rotacion

inline Mat4 mat4RotationY(float rad)
{
    Mat4 r = mat4Identity();
    float c = m_cos(rad);
    float s = m_sin(rad);

    r.m[0][0] = c;  r.m[0][2] = s;
    r.m[2][0] = -s;  r.m[2][2] = c;
    return r;
}