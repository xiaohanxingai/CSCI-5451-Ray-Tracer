#pragma once
#include <vector>
#include <string>
#include "types.h"
#include "primitive.h"
#include "lighting.h"

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

    std::vector<Light*> lights;

    // primitives
    std::vector<Sphere*>   spheres;
    std::vector<Triangle*>   triangles;

    // triangle data
    std::vector<Point3>     vertices;  // positions
    std::vector<Direction3> normals;   // per-vertex normals
};

// parse scene file and fill scene + output image info
Scene parseSceneFile(const std::string &filename,
                     int &img_width,
                     int &img_height,
                     std::string &imgName);
