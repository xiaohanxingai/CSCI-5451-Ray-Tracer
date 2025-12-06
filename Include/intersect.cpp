#include "intersect.h"

vec3 triangleNormal(const vec3 &v0,
                    const vec3 &v1,
                    const vec3 &v2)
{
    vec3 e1 = v1 - v0;
    vec3 e2 = v2 - v0;
    vec3 n  = cross3(e1, e2);
    return normalize3(n);  // "normalized triangle"
}

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

bool intersectTriangle(const vec3 &v0,
                       const vec3 &v1,
                       const vec3 &v2,
                       const Ray  &ray,
                       float t_min,
                       float t_max,
                       float &t_hit,
                       float &b0,
                       float &b1,
                       float &b2) {
    const float EPS = 1e-7f;
    vec3 e1 = v1 - v0;
    vec3 e2 = v2 - v0;
    vec3 pvec = cross3(ray.dir, e2);
    float det = dot3(e1, pvec);
    if (std::fabs(det) < EPS) return false;
    float invDet = 1.0f / det;
    vec3 tvec = ray.origin - v0;
    float u = dot3(tvec, pvec) * invDet;
    if (u < -EPS || u > 1.0f + EPS) return false;
    vec3 qvec = cross3(tvec, e1);
    float v = dot3(ray.dir, qvec) * invDet;
    if (v < -EPS || u + v > 1.0f + EPS)return false;
    float t = dot3(e2, qvec) * invDet;
    if (t < t_min - EPS || t > t_max + EPS) return false;
    t_hit = t;
    b1    = u;
    b2    = v;
    b0    = 1.0f - u - v;
    return true;
}
