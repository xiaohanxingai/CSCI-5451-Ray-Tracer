# CSCI-5451 Ray Tracer (OpenMP)

This repository contains the OpenMP-parallel ray tracer implementation.

## Steps to compile and test on Plate server

1. Login to Plate
   ```bash
   ssh <your-username>@csel-plate01.cselabs.umn.edu
   ```

2. Clone the repository
   ```bash
   git clone https://github.com/Agamal17/CSCI-5451-Ray-Tracer.git
   ```

3. Navigate to the project directory
   ```bash
   cd CSCI-5451-Ray-Tracer
   ```

4. Compile the OpenMP version
   ```bash
   g++ -std=c++17 -O2 -fopenmp main.cpp rayTrace.cpp scene.cpp lighting.cpp intersect.cpp primitive.cpp -I Include -o raytracer_omp
   ```

5. Set OpenMP environment variables (recommended)
   ```bash
   export OMP_PROC_BIND=spread
   export OMP_PLACES=cores
   export OMP_NUM_THREADS=64
   ```

6. Run a quick test
   ```bash
   ./raytracer_omp Tests/InterestingScenes/arm-top.txt
   ```

7. Run a complex scene (slower, but faster than base)
   ```bash
   ./raytracer_omp Tests/InterestingScenes/dragon.txt
   ```
