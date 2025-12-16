#pragma once
#include <cmath>
#include <algorithm>
#include <iostream>

using std::sqrt;
using std::min;
using std::max;

//Small vector library
// Represents a vector as 3 doubles

struct vec3{
  double x,y,z;

  vec3(double x, double y, double z) : x(x), y(y), z(z) {}
  vec3() : x(0), y(0), z(0) {}
  vec3 operator-() const {
        return vec3(-x, -y, -z);
    }
  //Clamp each component (used to clamp pixel colors)
  vec3 clampTo1(){
    return vec3(fmin(x,1),fmin(y,1),fmin(z,1));
  }

  //Compute vector length (you may also want length squared)
  double length(){
    return sqrt(x*x+y*y+z*z);
  }

  //Create a unit-length vector
  vec3 normalized(){
    double len = sqrt(x*x+y*y+z*z);
    return vec3(x/len,y/len,z/len);
  }

};

//Multiply double and vector
inline vec3 operator*(double f, vec3 a){
  return vec3(a.x*f,a.y*f,a.z*f);
}

//Vector-vector dot product
inline double dot(vec3 a, vec3 b){
  return a.x*b.x + a.y*b.y + a.z*b.z;
}

//Vector-vector cross product
inline vec3 cross(vec3 a, vec3 b){
  return vec3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y-a.y*b.x);
}

//Vector addition
inline vec3 operator+(vec3 a, vec3 b){
  return vec3(a.x+b.x, a.y+b.y, a.z+b.z);
}

//Vector subtraction
inline vec3 operator-(vec3 a, vec3 b){
  return vec3(a.x-b.x, a.y-b.y, a.z-b.z);
}

// Useful for optimization (avoids expensive sqrt)
inline double length_squared(const vec3& v) {
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

// Allow vec * double
inline vec3 operator*(const vec3& a, double f) {
    return vec3(a.x * f, a.y * f, a.z * f);
}

// Element-wise (Hadamard) multiplication
inline vec3 operator*(const vec3& a, const vec3& b) {
    return vec3(a.x * b.x, a.y * b.y, a.z * b.z);
}

inline std::ostream& operator<<(std::ostream& os, const vec3& v) {
  os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
  return os;
}