#include "Camera.h"

Mat4 makeView(const Camera& c)
{
    Vec3 f = normalize(sub(c.center, c.eye));      // forward
    Vec3 s = normalize(cross(f, c.up));            // right
    Vec3 u = cross(s, f);                          // corrected up

    Mat4 V = mat4Identity();

    V.m[0][0] = s.x;  V.m[0][1] = s.y;  V.m[0][2] = s.z;  V.m[0][3] = -dot(s, c.eye);
    V.m[1][0] = u.x;  V.m[1][1] = u.y;  V.m[1][2] = u.z;  V.m[1][3] = -dot(u, c.eye);
    V.m[2][0] = -f.x; V.m[2][1] = -f.y; V.m[2][2] = -f.z; V.m[2][3] = dot(f, c.eye);
    V.m[3][0] = 0.0f; V.m[3][1] = 0.0f; V.m[3][2] = 0.0f; V.m[3][3] = 1.0f;

    return V;
}