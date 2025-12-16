#include "Include/intersect.h"
#include "Include/lighting.h"
#include "Include/rayTrace.h"
#include <cmath>

Color rayTrace(Ray &ray, const int max_depth, const Scene& scene) {
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

Ray Reflect(Ray &ray, HitInfo& hit){
    Direction3 d = ray.dir.normalized();
    Direction3 n = hit.normal.normalized();

    Direction3 reflected_dir = d - 2.0 * dot(d, n) * n;

    return Ray(hit.point + reflected_dir * 0.001, reflected_dir.normalized());
}

Ray Refract(Ray &ray, HitInfo& hit){
    Direction3 I = ray.dir.normalized();
    Direction3 N = hit.normal.normalized();

    double ior = hit.material->ior;
    double cos_theta = dot(I, N);
    double eta;
    Direction3 n_eff;

    if (cos_theta > 0) {
        eta = ior;
        n_eff = -1.0 * N;
    } else {
        eta = 1.0 / ior;
        n_eff = N;
        cos_theta = -cos_theta;
    }

    double k = 1.0 - eta * eta * (1.0 - cos_theta * cos_theta);

    if (k < 0) {
        return Reflect(ray, hit); 
    }

    Direction3 T = eta * I + (eta * cos_theta - std::sqrt(k)) * n_eff;

    return Ray(hit.point + T * 0.001, T.normalized());
}
