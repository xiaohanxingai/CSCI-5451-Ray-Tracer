#pragma once
#include "types.h"
#include "ray.h"
#include "primitive.h"

struct Scene;

// Ray-sphere intersection.
// Returns true if there is a hit in [t_min, t_max] and writes the hit distance to t_hit.
bool intersectSphere(const Sphere &s,
                     const Ray   &ray,
                     float t_min,
                     float t_max,
                     float &t_hit);

// Ray-triangle intersection
double rayTriangleIntersect(const Ray &ray, const Triangle &triangle);

// UPDATED: Now takes 'scene' so we can access spheres and triangles
bool FindIntersection(const Scene &scene, const Ray &ray, HitInfo &hit);
