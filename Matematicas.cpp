#include "Matematicas.h"
#include <QtMath>

float m_sqrt(float x) { return qSqrt(x); }
float m_fabs(float x) { return qFabs(x); }
float m_tan(float x) { return qTan(x); }
int   m_roundToInt(float x) { return int(x + (x >= 0 ? 0.5f : -0.5f)); }

float m_sin(float x) { return qSin(x); }
float m_cos(float x) { return qCos(x); }

float m_degToRad(float deg)
{
    return deg * (3.14159265358979323846f / 180.0f);
}