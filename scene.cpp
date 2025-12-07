#include "Include/scene.h"
#include "Include/intersect.h"

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

    scene.background    = Color(0, 0, 0);
    scene.ambient_light = Color(0, 0, 0);

    // Default image parameters
    img_width  = 640;
    img_height = 480;
    imgName    = "raytraced.bmp";

    // Default material: 0 0 0  1 1 1  0 0 0  5  0 0 0  1
    Material defaultMat;
    defaultMat.ambient  = Color(0, 0, 0);
    defaultMat.diffuse  = Color(1, 1, 1);
    defaultMat.specular = Color(0, 0, 0);
    defaultMat.ns       = 5.0f;
    defaultMat.trans    = Color(0, 0, 0);
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
            ss >> scene.background.r >> scene.background.g >> scene.background.b;
        } else if (key == "ambient_light") {
            ss >> scene.ambient_light.r >> scene.ambient_light.g >> scene.ambient_light.b;
        } else if (key == "material") {
            Material m;
            ss >> m.ambient.r >> m.ambient.g >> m.ambient.b
               >> m.diffuse.r >> m.diffuse.g >> m.diffuse.b
               >> m.specular.r >> m.specular.g >> m.specular.b
               >> m.ns
               >> m.trans.r >> m.trans.g >> m.trans.b
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
            t.v1 = scene.vertices[v0];
            t.v2 = scene.vertices[v1];
            t.v3 = scene.vertices[v2];

            vec3 fn = normalize3(cross(t.v2 - t.v1, t.v3 - t.v1));
            t.triPlane = Direction3(fn);

            t.n1 = t.n2 = t.n3 = t.triPlane;
            t.flat = true;

            t.material_id = currentMatId;
            scene.triangles.push_back(t);

        } else if (key == "normal_triangle") {
            int v0, v1, v2, n0, n1, n2;
            ss >> v0 >> v1 >> v2 >> n0 >> n1 >> n2;

            Triangle t;
            t.v1 = scene.vertices[v0];
            t.v2 = scene.vertices[v1];
            t.v3 = scene.vertices[v2];

            t.n1 = scene.normals[n0];
            t.n2 = scene.normals[n1];
            t.n3 = scene.normals[n2];

            vec3 fn = normalize3(cross(t.v2 - t.v1, t.v3 - t.v1));
            t.triPlane = Direction3(fn);

            t.flat = false;

            t.material_id = currentMatId;
            scene.triangles.push_back(t);

        }
        /*                                          TODO: To be Implemented by Adrian
        else if (key == "directional_light") {
            float r, g, b, x, y, z;
            ss >> r >> g >> b >> x >> y >> z;
            DirectionalLight light;
            light.color = vec3(r, g, b);
            light.dir   = vec3(x, y, z);
            scene.lights.push_back(light);
        } else if (key == "point_light") {
            float r, g, b, x, y, z;
            ss >> r >> g >> b >> x >> y >> z;
            PointLight light;
            light.color = vec3(r, g, b);
            light.pos   = vec3(x, y, z);
            scene.lights.push_back(light);
        } else if (key == "spot_light") {
            float r, g, b, x, y, z, dir_x, dir_y, dir_z, angle1, angle2;
            ss >> r >> g >> b >> x >> y >> z >> dir_x >> dir_y >> dir_z >> angle1 >> angle2;
            SpotLight light;
            light.color = vec3(r, g, b);
            light.pos   = vec3(x, y, z);
            light.dir   = vec3(dir_x, dir_y, dir_z);
            light.angle1 = angle1;
            light.angle2 = angle2;
            scene.lights.push_back(light);
        }
         */
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

Direction3 Triangle::get_normal_at_point(const Point3 &p) const
{
    if (flat) return n1;

    vec3 e0 = v2 - v1;
    vec3 e1 = v3 - v1;
    vec3 vp = p  - v1;

    float d00 = dot(e0, e0);
    float d01 = dot(e0, e1);
    float d11 = dot(e1, e1);
    float d20 = dot(vp, e0);
    float d21 = dot(vp, e1);

    float denom = d00 * d11 - d01 * d01;
    float b2 = (d11 * d20 - d01 * d21) / denom;
    float b3 = (d00 * d21 - d01 * d20) / denom;
    float b1 = 1.0f - b2 - b3;

    vec3 n = normalize3(b1 * n1 + b2 * n2 + b3 * n3);
    return Direction3(n);
}

Direction3 Sphere::get_normal_at_point(const Point3 &p) const {
    vec3 n = p - center;

    float len2 = n.x * n.x + n.y * n.y + n.z * n.z;
    if (len2 > 0.0f) {
        float invLen = 1.0f / std::sqrt(len2);
        n.x *= invLen;
        n.y *= invLen;
        n.z *= invLen;
    }

    return Direction3(n);
}

int Sphere::getMaterialID() const {
    return material_id;
}

int Triangle::getMaterialID() const {
    return material_id;
}
