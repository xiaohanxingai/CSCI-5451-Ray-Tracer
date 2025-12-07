#pragma once

#include <vector>
#include <string>
#include "types.h"
#include "image_lib.h"

// ----------------- Material -----------------
struct Material {
    Color ambient;   // ar, ag, ab
    Color diffuse;   // dr, dg, db
    Color specular;  // sr, sg, sb
    float  ns;        // phong exponent
    Color trans;     // tr, tg, tb
    float  ior;       // index of refraction
};

struct HitInfo {
    float distance;
    Point3 point;
    Direction3 normal;
    Material material;

    HitInfo() : distance(INFINITY) {}
    HitInfo(float distance, Point3 point, Direction3 normal, Material material) : distance(distance), point(point), normal(normal), material(material) {}
};

struct Primitive {
    virtual int getMaterialID() const = 0;
    virtual Direction3 get_normal_at_point(const Point3 &p) const = 0;
};

// ----------------- Sphere -----------------
struct Sphere : public Primitive {
    Point3    center;
    float     radius;
    int       material_id;   // index into Scene::materials

    int getMaterialID() const override;
    // normal at a point on the surface
    Direction3 get_normal_at_point(const Point3 &p) const override;
};

// ----------------- Triangle -----------------
struct Triangle : public Primitive {
    Point3 v1, v2, v3;
    Direction3 n1, n2, n3;
    Direction3 triPlane;
    bool flat = true;  // true -> flat triangle else false
    int material_id;

    int getMaterialID() const override;
    Direction3 get_normal_at_point(const Point3 &p) const override;
};

// ----------------- Scene -----------------
struct Scene {
    // camera
    Point3     camera_pos;
    Direction3 camera_fwd;
    Direction3 camera_up;
    Direction3 camera_right;
    float      camera_fov_ha;

    // global settings
    Color background;
    Color ambient_light;
    int max_depth;

    // TODO: By Adrian
    // std::vector<Light> lights;

    // materials & primitives
    std::vector<Material> materials;
    std::vector<Sphere>   spheres;

    // triangle data
    std::vector<Point3>     vertices;  // positions
    std::vector<Direction3> normals;   // per-vertex normals
    std::vector<Triangle>   triangles;
};

// parse scene file and fill scene + output image info
Scene parseSceneFile(const std::string &filename,
                     int &img_width,
                     int &img_height,
                     std::string &imgName);
