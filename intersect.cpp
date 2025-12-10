#include "Include/intersect.h"
#include <limits>

bool intersectSphere(const Sphere &s,
                     const Ray   &ray,
                     float t_min,
                     float t_max,
                     float &t_hit) {
    vec3 oc = ray.origin - s.center;
    float a = dot(ray.dir, ray.dir);
    float b = 2.0f * dot(oc, ray.dir);
    float c = dot(oc, oc) - s.radius * s.radius;
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

    double denom = dot(n, ray.dir);
    if (std::fabs(denom) < 1e-7) return INF;

    double t = dot(n, triangle.v1 - ray.origin) / denom;
    if (t < 0.0) return INF;

    vec3 hitPoint = ray.origin + ray.dir * (float)t;

    vec3 c1 = cross(triangle.v2 - triangle.v1, hitPoint - triangle.v1);
    vec3 c2 = cross(triangle.v3 - triangle.v2, hitPoint - triangle.v2);
    vec3 c3 = cross(triangle.v1 - triangle.v3, hitPoint - triangle.v3);

    if (dot(c1, n) >= 0.0f &&
        dot(c2, n) >= 0.0f &&
        dot(c3, n) >= 0.0f)
    {
        double distance = dot((hitPoint - ray.origin), ray.dir);
        if (distance >= 0.0) return distance;
    }

    return INF;
}

bool FindIntersection(const Scene &scene, const Ray &ray, HitInfo &hit) {
    float closest_t = std::numeric_limits<float>::max();
    const Primitive* closest_prim = nullptr;
    float t_min = 0.0001f; // Epsilon to prevent self-intersection acne

    // 1. Check Spheres
    for (const auto& sphere : scene.spheres) {
        float t_hit_sphere;
        // Pass closest_t as t_max to automatically prune further intersections
        if (intersectSphere(sphere, ray, t_min, closest_t, t_hit_sphere)) {
            closest_t = t_hit_sphere;
            closest_prim = &sphere;
        }
    }

    // 2. Check Triangles
    for (const auto& tri : scene.triangles) {
        double t_d = rayTriangleIntersect(ray, tri);
        
        // rayTriangleIntersect returns infinity on miss
        if (t_d != std::numeric_limits<double>::infinity()) {
            float t_f = (float)t_d;
            if (t_f > t_min && t_f < closest_t) {
                closest_t = t_f;
                closest_prim = &tri;
            }
        }
    }

    // 3. Populate HitInfo if we hit something
    if (closest_prim != nullptr) {
        hit.distance = closest_t;
        hit.point = ray.origin + ray.dir * closest_t;
        
        // Polymorphic call to get normal (works for Sphere or Triangle)
        hit.normal = closest_prim->get_normal_at_point(hit.point);
        
        // Retrieve material
        int matID = closest_prim->getMaterialID();
        if (matID >= 0 && matID < (int)scene.materials.size()) {
            hit.material = scene.materials[matID];
        }
        return true;
    }

    return false;
}
