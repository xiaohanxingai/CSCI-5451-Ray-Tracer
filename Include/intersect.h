#pragma once
#include "types.h"
#include "ray.h"
#include "primitive.h"
#include "scene.h"

// Ray-sphere intersection.
// Returns distance to intersection or infinity if no hit.
double intersectSphere(const Ray &ray, const Sphere &s);

// Ray-triangle intersection
double rayTriangleIntersect(const Ray &ray, const Triangle &triangle);

bool FindIntersection(const Scene& scene, const Ray &ray, HitInfo &hit);
