#include "Projection.h"

static Mat4 makeOrtho(float l, float r, float b, float t, float n, float f)
{
    Mat4 P{};
    P.m[0][0] = 2.0f / (r - l);
    P.m[1][1] = 2.0f / (t - b);
    P.m[2][2] = -2.0f / (f - n);

    P.m[0][3] = -(r + l) / (r - l);
    P.m[1][3] = -(t + b) / (t - b);
    P.m[2][3] = -(f + n) / (f - n);

    P.m[3][3] = 1.0f;
    return P;
}

static Mat4 makePerspective(float fovDeg, float aspect, float n, float f)
{
    float fovRad = m_degToRad(fovDeg);
    float s = 1.0f / m_tan(fovRad * 0.5f);

    Mat4 P{};
    P.m[0][0] = s / aspect;
    P.m[1][1] = s;
    P.m[2][2] = (f + n) / (n - f);
    P.m[2][3] = (2.0f * f * n) / (n - f);
    P.m[3][2] = -1.0f;
    P.m[3][3] = 0.0f;
    return P;
}

Mat4 makeProjection(const Projection& p, float aspect)
{
    if (aspect <= 1e-6f) aspect = 1.0f;

    if (p.type == ProjectionType::Ortho)
    {
        float s = p.orthoHalfSize;
        return makeOrtho(-s * aspect, s * aspect, -s, s, p.nearPlane, p.farPlane);
    }
    return makePerspective(p.fovDeg, aspect, p.nearPlane, p.farPlane);
}