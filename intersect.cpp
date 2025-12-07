#include "Include/intersect.h"
#include <limits>

bool intersectSphere(const Sphere &s,
                     const Ray   &ray,
                     float t_min,
                     float t_max,
                     float &t_hit) {
    vec3 oc = ray.origin - s.center;
    float a = dot3(ray.dir, ray.dir);
    float b = 2.0f * dot3(oc, ray.dir);
    float c = dot3(oc, oc) - s.radius * s.radius;
    float disc = b*b - 4.0f*a*c;
    if (disc < 0.0f) return false;

    float sqrtD = std::sqrt(disc);
    float t0 = (-b - sqrtD) / (2.0f * a);
    float t1 = (-b + sqrtD) / (2.0f * a);

    float t = t0;
    if (t < t_min || t > t_max) {
        t = t1;
        if (t < t_min || t > t_max) return false;
    }

    t_hit = t;
    return true;
}

double rayTriangleIntersect(const Ray &ray, const Triangle &triangle)
{
    const double INF = std::numeric_limits<double>::infinity();

    vec3 n = triangle.triPlane;

    double denom = dot3(n, ray.dir);
    if (std::fabs(denom) < 1e-7) return INF;

    double t = dot3(n, triangle.v1 - ray.origin) / denom;
    if (t < 0.0) return INF;

    vec3 hitPoint = ray.origin + ray.dir * (float)t;

    vec3 c1 = cross3(triangle.v2 - triangle.v1, hitPoint - triangle.v1);
    vec3 c2 = cross3(triangle.v3 - triangle.v2, hitPoint - triangle.v2);
    vec3 c3 = cross3(triangle.v1 - triangle.v3, hitPoint - triangle.v3);

    if (dot3(c1, n) >= 0.0f &&
        dot3(c2, n) >= 0.0f &&
        dot3(c3, n) >= 0.0f)
    {
        double distance = dot3((hitPoint - ray.origin), ray.dir);
        if (distance >= 0.0) return distance;
    }

    return INF;
}

bool FindIntersection(const Ray &ray, HitInfo &hit) {
    // TODO: To be done by Neil
    return false;
}

