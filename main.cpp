// For Visual Studios
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES 
#endif

// Images Lib includes:
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "Include/image_lib.h"
#include "Include/vec3.h"
#include "Include/scene.h"
#include "Include/ray.h"
#include "Include/intersect.h"
#include "Include/lighting.h"

#include <iostream>
#include <string>

// Simple test function for part1 components

/* NOTE: Some variables were changed
void test_sphere_parts(const Scene &scene) {
    std::cout << "=== Test Xiaohan's parts ===\n";
    std::cout << "image size : " << img_width << " x " << img_height << "\n";
    std::cout << "output file: " << imgName << "\n";
    std::cout << "camera_pos : (" << scene.camera_pos.x << ", "
                                   << scene.camera_pos.y << ", "
                                   << scene.camera_pos.z << ")\n";
    std::cout << "num spheres: " << scene.spheres.size() << "\n";

    if (!scene.spheres.empty()) {
        int cx = img_width  / 2;
        int cy = img_height / 2;
        Ray ray = generateCameraRay(scene, img_width, img_height, cx, cy);

        float closest = 1e30f;
        bool hit_any = false;
        for (const Sphere &s : scene.spheres) {
            float t;
            if (intersectSphere(s, ray, 1e-3f, 1e9f, t)) {
                if (t < closest) {
                    closest = t;
                    hit_any = true;
                }
            }
        }

        if (hit_any) {
            std::cout << "center pixel ray hits a sphere, t = " << closest << "\n";
        } else {
            std::cout << "center pixel ray misses all spheres\n";
        }
    }
    std::cout << "=== End test ===\n";
}
 */

void test_triangle_parts(const Scene &scene)
{
    std::cout << "=== Test Chandan's triangle parts ===\n";

    if (scene.triangles.empty()) {
        std::cout << "no triangles in this scene\n";
        std::cout << "=== End triangle test ===\n";
        return;
    }

    const Triangle &tri = scene.triangles[0];

    const vec3 &v0 = tri.v1;
    const vec3 &v1 = tri.v2;
    const vec3 &v2 = tri.v3;

    vec3 faceN = tri.triPlane;
    faceN = normalize3(faceN);

    std::cout << "face normal = ("
              << faceN.x << ", " << faceN.y << ", " << faceN.z << ")\n";

    vec3 centroid = (1.0f / 3.0f) * (v0 + v1 + v2);

    Ray ray;
    ray.origin = centroid - 5.0f * faceN;
    ray.dir    = faceN; // already normalized

    double dist = rayTriangleIntersect(ray, tri);
    double INF  = std::numeric_limits<double>::infinity();

    if (dist >= INF) {
        std::cout << "triangle test: ray did NOT hit the first triangle\n";
        std::cout << "=== End triangle test ===\n";
        return;
    }

    vec3 hitPoint = ray.origin + ray.dir * (float)dist;

    std::cout << "triangle test: ray HIT the first triangle\n";
    std::cout << "  distance = " << dist << "\n";
    std::cout << "  hitPoint = ("
              << hitPoint.x << ", " << hitPoint.y << ", " << hitPoint.z << ")\n";

    vec3 N = tri.get_normal_at_point(hitPoint);

    if (dot3(N, ray.dir) > 0.0f) N = -1.0f * N;

    std::cout << "  normal@hit = ("
              << N.x << ", " << N.y << ", " << N.z << ")\n";

    std::cout << "=== End triangle test ===\n";
}

Color rayTrace(const Ray &ray, int max_depth, Scene& scene) {
    // Base Case: Stop the recursion if max depth is reached
    if (max_depth <= 0) {
        return Color(0, 0, 0);
    }

    bool b_hit = false;
    HitInfo hit;

    b_hit = FindIntersection(ray, hit);
    if (b_hit) {
        return ApplyLighting(scene, ray, hit, max_depth);
    }

    return scene.background;
}

int main(int argc, char** argv) {
    // Read command line parameters to get scene file
    if (argc < 2) {
        std::cout << "Usage: ./a.out scenefile\n";
        return 0;
    }

    std::string sceneFileName = argv[1];
    int img_width, img_height;
    std::string imgName;
    // parser: build a Scene and get output image info
    Scene scene = parseSceneFile(sceneFileName, img_width, img_height, imgName);

    Image outputImg = Image(img_width, img_height);
    float imgW = img_width, imgH = img_height;
    float halfW = imgW / 2, halfH = imgH / 2;
    float d = halfH / tanf(scene.camera_fov_ha * (M_PI / 180.0f));

    for (int i = 0; i < img_width; i++) {
        for (int j = 0; j < img_height; j++) {
            float u = halfW - i + 0.5;
            float v = halfH - j + 0.5;
            Point3 p = scene.camera_pos - d * scene.camera_fwd + u * scene.camera_right + v * scene.camera_up;
            Ray ray(scene.camera_pos, p - scene.camera_pos);
            Color result =  rayTrace(ray, scene.max_depth, scene);
            outputImg.getPixel(i, j) = result;
        }
    }

    outputImg.write(imgName.c_str());

    return 0;
}
