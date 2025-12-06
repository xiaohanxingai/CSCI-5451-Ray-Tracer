#pragma once

#include "scene.h"
#include "ray.h"
#include <cmath>

inline float dot3(const vec3 &a, const vec3 &b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline float length3(const vec3 &v) {
    return std::sqrt(dot3(v, v));
}

inline vec3 normalize3(const vec3 &v) {
    float len = length3(v);
    if (len <= 0.0f) return v;
    return vec3(v.x / len, v.y / len, v.z / len);
}

inline vec3 cross3(const vec3 &a, const vec3 &b) {
    return vec3(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    );
}

// Ray-sphere intersection.
// Returns true if there is a hit in [t_min, t_max] and writes the hit distance to t_hit.
bool intersectSphere(const Sphere &s,
                     const Ray   &ray,
                     float t_min,
                     float t_max,
                     float &t_hit);

// Ray-Triangle Normal
vec3 triangleNormal(const vec3 &v0,
                    const vec3 &v1,
                    const vec3 &v2);

// Ray-triangle intersection
bool intersectTriangle(const vec3 &v0,
                       const vec3 &v1,
                       const vec3 &v2,
                       const Ray  &ray,
                       float t_min,
                       float t_max,
                       float &t_hit,
                       float &b0,
                       float &b1,
                       float &b2);
