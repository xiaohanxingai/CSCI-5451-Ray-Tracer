#include "Include/primitive.h"

Direction3 Triangle::get_normal_at_point(const Point3 &p) const
{
    if (flat) return n1;

    Direction3 e0 = v2 - v1;
    Direction3 e1 = v3 - v1;
    Direction3 vp = p  - v1;

    double d00 = dot(e0, e0);
    double d01 = dot(e0, e1);
    double d11 = dot(e1, e1);
    double d20 = dot(vp, e0);
    double d21 = dot(vp, e1);

    double denom = d00 * d11 - d01 * d01;
    double b2 = (d11 * d20 - d01 * d21) / denom;
    double b3 = (d00 * d21 - d01 * d20) / denom;
    double b1 = 1.0 - b2 - b3;

    return (b1 * n1 + b2 * n2 + b3 * n3).normalized();
}

Direction3 Sphere::get_normal_at_point(const Point3 &p) const {
    return (p - center).normalized();
}

Material* Sphere::getMaterial() const {
    return material;
}

Material* Triangle::getMaterial() const {
    return material;
}
