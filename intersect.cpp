#include "Include/intersect.h"
#include <limits>

double intersectSphere(const Ray &ray, const Sphere &s) {
    const double INF = std::numeric_limits<double>::infinity();
    
    Direction3 oc = ray.origin - s.center;
    double a = dot(ray.dir, ray.dir);
    double b = 2.0 * dot(oc, ray.dir);
    double c = dot(oc, oc) - s.radius * s.radius;
    double disc = b*b - 4.0*a*c;
    
    if (disc < 0.0) return INF;

    double sqrtD = std::sqrt(disc);
    double t0 = (-b - sqrtD) / (2.0 * a);
    double t1 = (-b + sqrtD) / (2.0 * a);


    double t = t0;
    if (t < 1e-4) {
        t = t1;
        if (t < 1e-4) return INF;
    }

    return t;
}

double rayTriangleIntersect(const Ray &ray, const Triangle &triangle)
{
    const double INF = std::numeric_limits<double>::infinity();

    Direction3 n = triangle.triPlane;
    double denom = dot(n, ray.dir);
    if (std::abs(denom) < 1e-7) return INF;

    double t = dot(n, triangle.v1 - ray.origin) / denom;
    if (t < 1e-4) return INF;

    Point3 hitPoint = ray.origin + ray.dir * t;

    Direction3 c1 = cross(triangle.v2 - triangle.v1, hitPoint - triangle.v1);
    Direction3 c2 = cross(triangle.v3 - triangle.v2, hitPoint - triangle.v2);
    Direction3 c3 = cross(triangle.v1 - triangle.v3, hitPoint - triangle.v3);

    double d1 = dot(c1, n);
    double d2 = dot(c2, n);
    double d3 = dot(c3, n);

    if ( (d1 >= 0 && d2 >= 0 && d3 >= 0) ||
         (d1 <= 0 && d2 <= 0 && d3 <= 0) )
    {
        return t;
    }

    return INF;
}

bool FindIntersection(const Scene &scene, const Ray &ray, HitInfo &hit) {
    double closest_t = std::numeric_limits<double>::max();
    const Primitive* closest_prim = nullptr;
    double t_min = 0.0001; // Epsilon to prevent self-intersection acne

    // 1. Check Spheres
    for (const auto& sphere : scene.spheres) {
        double t_d = intersectSphere(ray, *sphere); 
        if (t_d != std::numeric_limits<double>::infinity()) {
            double t_f = t_d;
            if (t_f > t_min && t_f < closest_t) {
                closest_t = t_f;
                closest_prim = sphere;
            }
        }
    }

    // 2. Check Triangles
    for (const auto& tri : scene.triangles) {
        double t_d = rayTriangleIntersect(ray, *tri);

        // rayTriangleIntersect returns infinity on miss
        if (t_d != std::numeric_limits<double>::infinity()) {
            double t_f = t_d;
            if (t_f > t_min && t_f < closest_t) {
                closest_t = t_f;
                closest_prim = tri;
            }
        }
    }

    // 3. Populate HitInfo if we hit something
    if (closest_prim != nullptr) {
        hit.distance = closest_t;
        hit.point = ray.origin + ray.dir * closest_t;

        // Polymorphic call to get normal (works for Sphere or Triangle)
        hit.normal = closest_prim->get_normal_at_point(hit.point).normalized();

        if (dot(hit.normal, ray.dir) > 0) hit.normal = -hit.normal; // to ensure normal is opposite to viewing ray

        // Retrieve material
        hit.material = closest_prim->getMaterial();

        return true;
    }

    return false;
}

