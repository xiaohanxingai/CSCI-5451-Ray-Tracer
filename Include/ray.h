#pragma once

#include "types.h"

struct Ray {
    Point3     origin;
    Direction3 dir;

    Ray(Point3 origin, Direction3 dir) : origin(origin), dir(dir) {}
    Ray() = default;
};

