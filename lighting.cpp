#define _USE_MATH_DEFINES
#include "Include/scene.h"
#include "Include/intersect.h"
#include "Include/lighting.h"
#include "Include/rayTrace.h"


Color DirectionalLight::getContribution(const Scene& scene, const Ray& ray, HitInfo& hit) {
    Color final_color(0, 0, 0);
    Point3 hit_point_tolerated = hit.point + hit.normal.normalized() * 1e-4;
    Ray shadowRay(hit_point_tolerated, -direction);
    bool b_hit = false;
    HitInfo shadowHit;
    b_hit = FindIntersection(scene, shadowRay, shadowHit);
    if (!b_hit) {
        final_color = final_color + hit.material->diffuse * color * std::max(0.0, dot(hit.normal.normalized(), -direction.normalized()));
        Direction3 h = (-direction - ray.dir).normalized();
        final_color = final_color + hit.material->specular * color * pow(std::max(0.0, dot(hit.normal.normalized(), h)), hit.material->ns);
    }

    return final_color;
}

Color PointLight::getContribution(const Scene& scene, const Ray& ray, HitInfo& hit) {
    Color final_color(0, 0, 0);
    Point3 hit_point_tolerated = hit.point + hit.normal.normalized() * 1e-4;
    Ray shadowRay(hit_point_tolerated, position - hit_point_tolerated);
    bool b_hit = false;
    HitInfo shadowHit;
    b_hit = FindIntersection(scene, shadowRay, shadowHit);
    double light_distance = dot((position - hit_point_tolerated), (position - hit_point_tolerated).normalized());
    if (!(b_hit && shadowHit.distance < light_distance)) {
        Color attenuated_color = color / pow(light_distance, 2);
        Direction3 direction = position - hit_point_tolerated;
        final_color = final_color + hit.material->diffuse * attenuated_color * std::max(0.0, dot(hit.normal.normalized(), direction.normalized()));
        Direction3 h = (direction - ray.dir).normalized();
        final_color = final_color + hit.material->specular * attenuated_color * pow(std::max(0.0, dot(hit.normal.normalized(), h)), hit.material->ns);
    }
    return final_color;
}

Color SpotLight::getContribution(const Scene& scene, const Ray& ray, HitInfo& hit) {
    Color final_color(0, 0, 0);
    Point3 hit_point_tolerated = hit.point + hit.normal.normalized() * 1e-6;
    Ray shadowRay(hit_point_tolerated, position - hit_point_tolerated);
    bool b_hit = false;
    HitInfo shadowHit;
    b_hit = FindIntersection(scene, shadowRay, shadowHit);
    double light_distance = dot((position - hit_point_tolerated), (position - hit_point_tolerated).normalized());
    if (!(b_hit && shadowHit.distance < light_distance)) {
        double hitAngle = acos(dot((hit_point_tolerated - position).normalized(), direction.normalized()));
        hitAngle = hitAngle * 180.0 / M_PI;
        if (hitAngle > angle2)
            return final_color;

        Color attenuated_color = color / pow(light_distance, 2);
        double falloff = 1.0;
        if (hitAngle > angle1) {
            double t = (hitAngle - angle1) / (angle2 - angle1);
            falloff = std::max(0.0, 1.0 - t);
        }
        attenuated_color = attenuated_color * falloff;

        Direction3 lightDir = (position - hit_point_tolerated).normalized();
        final_color = final_color + hit.material->diffuse * attenuated_color * dot(hit.normal.normalized(), lightDir);
        Direction3 h = (lightDir - ray.dir).normalized();
        final_color = final_color + hit.material->specular * attenuated_color * pow(dot(hit.normal.normalized(), h), hit.material->ns);
    }
    return final_color;
}

Color ApplyLighting(const Scene& scene,
                    Ray &ray,
                    HitInfo &hit,
                    int depth) {
    Color color = hit.material->ambient * scene.ambient_light;

    for (Light* light : scene.lights) {
        color = color + light->getContribution(scene, ray, hit);
    }

    Ray refraction = Refract(ray, hit);
    color = color + hit.material->trans * rayTrace(refraction, depth - 1, scene);
    Ray reflection = Reflect(ray, hit);
    color = color + hit.material->specular * rayTrace(reflection, depth - 1, scene);

    return color;
}
