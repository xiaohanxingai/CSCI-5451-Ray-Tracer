#include "scene.h"


#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>

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

    scene.background    = vec3(0, 0, 0);
    scene.ambient_light = vec3(0, 0, 0);

    // Default image parameters
    img_width  = 640;
    img_height = 480;
    imgName    = "raytraced.bmp";

    // Default material: 0 0 0  1 1 1  0 0 0  5  0 0 0  1
    Material defaultMat;
    defaultMat.ambient  = vec3(0, 0, 0);
    defaultMat.diffuse  = vec3(1, 1, 1);
    defaultMat.specular = vec3(0, 0, 0);
    defaultMat.ns       = 5.0f;
    defaultMat.trans    = vec3(0, 0, 0);
    defaultMat.ior      = 1.0f;

    scene.materials.push_back(defaultMat);
    int currentMatId = 0;
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
            ss >> scene.background.x >> scene.background.y >> scene.background.z;
        } else if (key == "ambient_light") {
            ss >> scene.ambient_light.x >> scene.ambient_light.y >> scene.ambient_light.z;
        } else if (key == "material") {
            Material m;
            ss >> m.ambient.x >> m.ambient.y >> m.ambient.z
               >> m.diffuse.x >> m.diffuse.y >> m.diffuse.z
               >> m.specular.x >> m.specular.y >> m.specular.z
               >> m.ns
               >> m.trans.x >> m.trans.y >> m.trans.z
               >> m.ior;
            scene.materials.push_back(m);
            currentMatId = (int)scene.materials.size() - 1;
        } else if (key == "sphere") {
            float x, y, z, r;
            ss >> x >> y >> z >> r;
            Sphere s;
            s.center      = vec3(x, y, z);
            s.radius      = r;
            s.material_id = currentMatId;
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
            Triangle t;
            t.v[0] = v0;
            t.v[1] = v1;
            t.v[2] = v2;
            t.n[0] = t.n[1] = t.n[2] = -1;
            t.material_id   = currentMatId;
            t.has_vertex_normals = false;
            scene.triangles.push_back(t);
        } else if (key == "normal_triangle") {
            int v0, v1, v2, n0, n1, n2;
            ss >> v0 >> v1 >> v2 >> n0 >> n1 >> n2;
            Triangle t;
            t.v[0] = v0;
            t.v[1] = v1;
            t.v[2] = v2;
            t.n[0] = n0;
            t.n[1] = n1;
            t.n[2] = n2;
            t.material_id   = currentMatId;
            t.has_vertex_normals = true;
            scene.triangles.push_back(t);
        } else {
            // Other keys (vertex, triangle, lights, etc.) are left for teammates
            // Chandan's Comment : Updated Triange and Normalized Triangle Part
        }
    }

    return scene;
}
