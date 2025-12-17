# CSCI-5451 Ray Tracer (Base)

This repository contains the base sequential ray tracer implementation.

Steps to compile and test on Plate server:

1. Login to Plate
   ssh <your-username>@csel-plate01.cselabs.umn.edu

2. Clone the repository
   git clone https://github.com/Agamal17/CSCI-5451-Ray-Tracer.git

3. Navigate to the project directory
   cd CSCI-5451-Ray-Tracer

4. Compile the code
    g++ -std=c++17 -O2 main.cpp rayTrace.cpp scene.cpp lighting.cpp intersect.cpp primitive.cpp -I Include -o raytracer

5. Run a quick test (recommended)
   ./raytracer Tests/arm-top.txt

6. Run a complex scene (slow, ~40+ minutes)
   ./raytracer Tests/InterestingScenes/dragon.txt


