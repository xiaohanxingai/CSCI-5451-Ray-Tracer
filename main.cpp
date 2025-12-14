// For Visual Studios
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES 
#endif

// Images Lib includes:
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "Include/Image/image_lib.h"
#include "Include/ray.h"
#include "Include/rayTrace.h"
#include "Include/scene.h"
#include <iostream>
#include <string>

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
    const Scene scene = parseSceneFile(sceneFileName, img_width, img_height, imgName);

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
            Color result = rayTrace(ray, scene.max_depth, scene);
            outputImg.getPixel(i, j) = result;
        }
    }
    outputImg.write(imgName.c_str());

    // Free heap allocations
    for (Sphere* s : scene.spheres) {
        delete s;
    }
    for (Triangle* t : scene.triangles) {
        delete t;
    }
    for (Material* m : scene.materials) {
        delete m;
    }
    for (Light* l: scene.lights){
        delete l;
    }


    return 0;
}

