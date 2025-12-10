#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include <vector>
#include "Include/ray.h"
#include "Include/rayTrace.h"
#include "Include/scene.h"
#include "Include/types.h"

// Trim leading and trailing whitespace from a string
static inline void trimInPlace(std::string &s) {
    size_t b = s.find_first_not_of(" \t\r\n");
    if (b == std::string::npos) {
        s.clear();
        return;
    }
    size_t e = s.find_last_not_of(" \t\r\n");
    s = s.substr(b, e - b + 1);
}

Scene parseSceneFile(const std::string &filename,
                     int &img_width,
                     int &img_height,
                     std::string &imgName) {
    Scene scene;

    // Default camera and global settings
    scene.camera_pos    = vec3(0, 0, 0);
    scene.camera_fwd    = vec3(0, 0, -1);
    scene.camera_up     = vec3(0, 1, 0);
    scene.camera_fov_ha = 45.0f;

    scene.background    = Color(0, 0, 0);
    scene.ambient_light = Color(0, 0, 0);

    // Default image parameters
    img_width  = 640;
    img_height = 480;
    imgName    = "raytraced.bmp";

    // Default material: 0 0 0  1 1 1  0 0 0  5  0 0 0  1
    Material* material = new Material;
    material->ambient  = Color(0, 0, 0);
    material->diffuse  = Color(1, 1, 1);
    material->specular = Color(0, 0, 0);
    material->ns       = 5.0f;
    material->trans    = Color(0, 0, 0);
    material->ior      = 1.0f;

    int max_vertices = 0;
    int max_normals  = 0;

    std::ifstream in(filename);
    if (!in) {
        std::cerr << "Cannot open scene file: " << filename << std::endl;
        return scene;
    }

    std::string line;
    while (std::getline(in, line)) {
        trimInPlace(line);
        if (line.empty()) continue;
        if (line[0] == '#') continue; // comment line

        std::string key, rest;
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            key  = line.substr(0, colonPos);
            rest = line.substr(colonPos + 1);
        } else {
            std::stringstream ss(line);
            ss >> key;
            std::getline(ss, rest);
        }
        trimInPlace(key);
        trimInPlace(rest);
        std::stringstream ss(rest);

        if (key == "film_resolution") {
            ss >> img_width >> img_height;
        } else if (key == "output_image") {
            ss >> imgName;
            trimInPlace(imgName);
            if (!imgName.empty() && imgName.front() == '"' && imgName.back() == '"') {
                imgName = imgName.substr(1, imgName.size() - 2);
            }
        } else if (key == "camera_pos") {
            ss >> scene.camera_pos.x >> scene.camera_pos.y >> scene.camera_pos.z;
        } else if (key == "camera_fwd") {
            ss >> scene.camera_fwd.x >> scene.camera_fwd.y >> scene.camera_fwd.z;
        } else if (key == "camera_up") {
            ss >> scene.camera_up.x >> scene.camera_up.y >> scene.camera_up.z;
        } else if (key == "camera_fov_ha") {
            ss >> scene.camera_fov_ha;
        } else if (key == "background") {
            ss >> scene.background.r >> scene.background.g >> scene.background.b;
        } else if (key == "ambient_light") {
            ss >> scene.ambient_light.r >> scene.ambient_light.g >> scene.ambient_light.b;
        } else if (key == "material") {
            material = new Material;
            ss >> material->ambient.r >> material->ambient.g >> material->ambient.b
               >> material->diffuse.r >> material->diffuse.g >> material->diffuse.b
               >> material->specular.r >> material->specular.g >> material->specular.b
               >> material->ns
               >> material->trans.r >> material->trans.g >> material->trans.b
               >> material->ior;

        } else if (key == "sphere") {
            float x, y, z, r;
            ss >> x >> y >> z >> r;
            Sphere* s = new Sphere;
            s->center      = vec3(x, y, z);
            s->radius      = r;
            s->material = material;
            scene.spheres.push_back(s);
        } else if (key == "max_vertices") {
            ss >> max_vertices;
            if (max_vertices < 0) max_vertices = 0;
            scene.vertices.reserve(max_vertices);
        } else if (key == "max_normals") {
            ss >> max_normals;
            if (max_normals < 0) max_normals = 0;
            scene.normals.reserve(max_normals);
        } else if (key == "vertex") {
            float x, y, z;
            ss >> x >> y >> z;
            if (max_vertices > 0 &&
                (int)scene.vertices.size() >= max_vertices) {
                std::cerr << "Warning: more vertices than max_vertices in "
                          << filename << std::endl;
            }
            scene.vertices.push_back(vec3(x, y, z));
        } else if (key == "normal") {
            float x, y, z;
            ss >> x >> y >> z;
            if (max_normals > 0 &&
                (int)scene.normals.size() >= max_normals) {
                std::cerr << "Warning: more normals than max_normals in "
                          << filename << std::endl;
            }
            scene.normals.push_back(vec3(x, y, z));
        } else if (key == "triangle") {
            int v0, v1, v2;
            ss >> v0 >> v1 >> v2;

            Triangle* t = new Triangle;
            t->v1 = scene.vertices[v0];
            t->v2 = scene.vertices[v1];
            t->v3 = scene.vertices[v2];

            vec3 fn = cross(t->v2 - t->v1, t->v3 - t->v1).normalized();
            t->triPlane = Direction3(fn);

            t->n1 = t->n2 = t->n3 = t->triPlane;
            t->flat = true;

            t->material = material;
            scene.triangles.push_back(t);

        } else if (key == "normal_triangle") {
            int v0, v1, v2, n0, n1, n2;
            ss >> v0 >> v1 >> v2 >> n0 >> n1 >> n2;

            Triangle* t = new Triangle;
            t->v1 = scene.vertices[v0];
            t->v2 = scene.vertices[v1];
            t->v3 = scene.vertices[v2];

            t->n1 = scene.normals[n0];
            t->n2 = scene.normals[n1];
            t->n3 = scene.normals[n2];

            vec3 fn = cross(t->v2 - t->v1, t->v3 - t->v1).normalized();
            t->triPlane = Direction3(fn);

            t->flat = false;

            t->material = material;
            scene.triangles.push_back(t);
        }
        else if (key == "directional_light") {
            float r, g, b, x, y, z;
            ss >> r >> g >> b >> x >> y >> z;
            Light* light = new DirectionalLight(Color(r,g,b), Direction3(x, y, z));
            scene.lights.push_back(light);
        } else if (key == "point_light") {
            float r, g, b, x, y, z;
            ss >> r >> g >> b >> x >> y >> z;
            Light* light = new PointLight(Color(r,g,b), Point3(x, y, z));
            scene.lights.push_back(light);
        } else if (key == "spot_light") {
            float r, g, b, x, y, z, dir_x, dir_y, dir_z, angle1, angle2;
            ss >> r >> g >> b >> x >> y >> z >> dir_x >> dir_y >> dir_z >> angle1 >> angle2;
            Light* light = new SpotLight(Color(r,g,b), Point3(x, y, z), Direction3(dir_x, dir_y, dir_z), angle1, angle2);
            scene.lights.push_back(light);
        }
        else if (key == "max_depth"){
            ss >> scene.max_depth;
        }
        else {
            std::cerr << "Warning: unknown key " << key << " in " << filename << std::endl;
        }
    }

    Direction3 right = cross(scene.camera_up, scene.camera_fwd).normalized();

    // calculate up again using forward and right which we are sure are orthogonal
    scene.camera_up = cross(scene.camera_fwd, right).normalized();

    scene.camera_fwd = scene.camera_fwd.normalized();

    scene.camera_right = right;

    return scene;
}