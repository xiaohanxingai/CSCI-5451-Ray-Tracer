#pragma once

#include "vec3.h"
#include <vector>
#include <string>

struct Material {
    vec3 ambient;   // ar, ag, ab
    vec3 diffuse;   // dr, dg, db
    vec3 specular;  // sr, sg, sb
    float ns;       // Phong exponent
    vec3 trans;     // tr, tg, tb
    float ior;      // index of refraction
};

struct Sphere {
    vec3 center;
    float radius;
    int  material_id;   // index into Scene::materials
};

struct Triangle {
    int v[3];                // indices for vertices
    int n[3];                // indices for normals
    int material_id;         // index for materials
    bool has_vertex_normals;
};

struct Scene {
    // Camera parameters
    vec3  camera_pos;
    vec3  camera_fwd;
    vec3  camera_up;
    float camera_fov_ha;

    // Global settings
    vec3 background;
    vec3 ambient_light;

    // Geometry and materials
    std::vector<Material> materials;
    std::vector<Sphere>   spheres;

    // Triangle mesh data
    std::vector<vec3>     vertices;
    std::vector<vec3>     normals;
    std::vector<Triangle> triangles;
};

// Parses the scene file, fills a Scene, and also sets output image size & name.
Scene parseSceneFile(const std::string &filename,
                     int &img_width,
                     int &img_height,
                     std::string &imgName);
