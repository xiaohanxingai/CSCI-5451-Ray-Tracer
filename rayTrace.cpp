#include "Include/intersect.h"
#include "Include/lighting.h"
#include "Include/rayTrace.h"
#include <cmath>

Color rayTrace(const Ray &ray, const int max_depth, const Scene& scene) {
    // Base Case: Stop the recursion if max depth is reached
    if (max_depth <= 0) {
        return Color(0, 0, 0);
    }

    bool b_hit = false;
    HitInfo hit;

    b_hit = FindIntersection(scene, ray, hit);
    if (b_hit) {
        return ApplyLighting(scene, ray, hit, max_depth);
    }

    return scene.background;
}

Ray Reflect(const Ray &ray, const HitInfo& hit){
    vec3 d = ray.dir.normalized();
    vec3 n = hit.normal.normalized();

    vec3 reflected_dir = d - 2.0f * dot(d, n) * n;

    return Ray(hit.point + reflected_dir * 0.001f, reflected_dir.normalized());
}

Ray Refract(const Ray &ray, const HitInfo& hit){
    vec3 I = ray.dir.normalized();
    vec3 N = hit.normal.normalized();

    float ior = hit.material.ior;
    float cos_theta = dot(I, N);
    float eta;
    vec3 n_eff;

    if (cos_theta > 0) {
        eta = ior;
        n_eff = -1.0f * N;
    } else {
        eta = 1.0f / ior;
        n_eff = N;
        cos_theta = -cos_theta;
    }

    float k = 1.0f - eta * eta * (1.0f - cos_theta * cos_theta);

    if (k < 0) {
        return Reflect(ray, hit); 
    }

    vec3 T = eta * I + (eta * cos_theta - std::sqrt(k)) * n_eff;

    return Ray(hit.point + T * 0.001f, T.normalized());
}
