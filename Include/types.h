#pragma once

#include "vec3.h"

// A 3D point in space
struct Point3 : public vec3 {
    using vec3::vec3;         
    Point3() = default;
    Point3(const vec3& v) : vec3(v) {}  
};

// A 3D direction (or normal)
struct Direction3 : public vec3 {
    using vec3::vec3;
    Direction3() = default;
    Direction3(const vec3& v) : vec3(v) {}
};

// A RGB color
struct Color3 : public vec3 {
    using vec3::vec3;
    Color3() = default;
    Color3(const vec3& v) : vec3(v) {}
};

