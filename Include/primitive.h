#pragma once
#include <limits>
#include <vector>
#include "types.h"

// ----------------- Material -----------------
struct Material {
    Color ambient;   // ar, ag, ab
    Color diffuse;   // dr, dg, db
    Color specular;  // sr, sg, sb
    double  ns;        // phong exponent
    Color trans;     // tr, tg, tb
    double  ior;       // index of refraction
};

struct HitInfo {
    double distance;
    Point3 point;
    Direction3 normal;
    Material* material;

    HitInfo() : distance(INFINITY) {}
    HitInfo(double distance, Point3 point, Direction3 normal, Material* material) : distance(distance), point(point), normal(normal), material(material) {}
};

struct Primitive {
    virtual Material* getMaterial() const = 0;
    virtual Direction3 get_normal_at_point(const Point3 &p) const = 0;
};

// ----------------- Sphere -----------------
struct Sphere : public Primitive {
    Point3    center;
    double     radius;
    Material*     material;   // index into Scene::materials

    Material* getMaterial() const override;
    // normal at a point on the surface
    Direction3 get_normal_at_point(const Point3 &p) const override;
};

// ----------------- Triangle -----------------
struct Triangle : public Primitive {
    Point3 v1, v2, v3;
    Direction3 n1, n2, n3;
    Direction3 triPlane;
    bool flat = true;  // true -> flat triangle else false
    Material*     material;   // index into Scene::materials

    Material* getMaterial() const override;
    Direction3 get_normal_at_point(const Point3 &p) const override;
};