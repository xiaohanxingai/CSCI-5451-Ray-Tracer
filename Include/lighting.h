#include "scene.h"
#include "ray.h"
#include "image_lib.h"

Color ApplyLighting(const Scene& scene,
                    const Ray &ray,
                    const HitInfo &hit,
                    int depth);