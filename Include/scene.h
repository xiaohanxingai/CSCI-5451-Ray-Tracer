#pragma once

#include <vector>
#include <string>
#include "types.h"

// ----------------- Material -----------------
struct Material {
    Color3 ambient;   // ar, ag, ab
    Color3 diffuse;   // dr, dg, db
    Color3 specular;  // sr, sg, sb
    float  ns;        // phong exponent
    Color3 trans;     // tr, tg, tb
    float  ior;       // index of refraction
};

// ----------------- Sphere -----------------
struct Sphere {
    Point3    center;
    float     radius;
    int       material_id;   // index into Scene::materials

    // normal at a point on the surface
    Direction3 get_normal_at_point(const Point3 &p) const;
};

// ----------------- Triangle -----------------
struct Triangle {
    int v[3];   // indices into Scene::vertices
    int n[3];   // indices into Scene::normals (or -1 if none)
    int  material_id;
    bool has_vertex_normals;
};

// ----------------- Scene -----------------
struct Scene {
    // camera
    Point3     camera_pos;
    Direction3 camera_fwd;
    Direction3 camera_up;
    float      camera_fov_ha;

    // global settings
    Color3 background;
    Color3 ambient_light;

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
