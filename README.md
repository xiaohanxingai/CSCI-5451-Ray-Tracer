# CSCI-5451 Ray Tracer (MPI)

This repository contains the MPI-Parallel ray tracer implementation.

## Steps to compile and test on PLATE server

1. Login to Plate
   ```bash
   ssh <your-username>@csel-plate01.cselabs.umn.edu
   ```

2. Clone the repository
   ```bash
   git clone --branch MPI --single-branch https://github.com/Agamal17/CSCI-5451-Ray-Tracer.git
   ```
   
3. Navigate to the project directory
   ```bash
   cd CSCI-5451-Ray-Tracer
   ```
   
4. Cross Verify branch using command
   ```bash
   git branch
   ```

5. Compile the code
   ```bash
   mpicxx -O3 -march=native -ffast-math -std=c++17 main.cpp rayTrace.cpp scene.cpp lighting.cpp intersect.cpp primitive.cpp -IInclude -IInclude/Image -o raytracer_mpi
   ```

6. Run a quick test (recommended)
   ```bash
   mpirun -np 64 ./raytracer_mpi Tests/InterestingScences/arm-top.txt
   ```

7. Run a complex scene
   ```bash
   mpirun -np 64 ./raytracer_mpi Tests/InterestingScences/dragon.txt
   ```
