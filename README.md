CSCI-5451 Ray Tracer (MPI Version)

This repository contains the parallel MPI implementation of the CSCI-5451 ray tracer project.
The program distributes ray-scene computation across multiple MPI ranks and aggregates the final image on rank 0.

Steps to compile and test on Plate server
1. Login to Plate
ssh <your-username>@csel-plate01.cselabs.umn.edu


(You may also use plate02–04.)

2. Clone the repository
git clone https://github.com/<team-repo>/CSCI-5451-Ray-Tracer.git

3. Navigate to the project directory
cd CSCI-5451-Ray-Tracer

4. Compile the MPI version
mpicxx -O3 -march=native -ffast-math -std=c++17 \
    main.cpp rayTrace.cpp scene.cpp lighting.cpp intersect.cpp primitive.cpp \
    -IInclude -IInclude/Image \
    -o ray_mpi


This produces the executable:

ray_mpi

5. Run a quick test (fast)

Triangle scenes (recommended sanity tests):

mpirun -np 16 ./ray_mpi Tests/TriangleExamples/test_reasonable.txt

mpirun -np 16 ./ray_mpi Tests/TriangleExamples/triangle.txt


These tests complete quickly (sub-second runtime).

6. Run the automated batch script (timing experiments)

A helper script is included to run selected scenes with np = 16, 32, 64.

chmod +x run_mpi_batch.sh
nohup ./run_mpi_batch.sh > logs/run.log 2>&1 &


Monitor progress:

tail -f logs/run.log


The script runs the following scenes:

Tests/TriangleExamples/test_reasonable.txt

Tests/TriangleExamples/triangle.txt

Tests/cat.txt

Tests/character.txt

Tests/island.txt

Tests/lily.txt

Test/sword.txt

Test/spaceship.txt

(Zip files and large scenes are automatically excluded.)

7. Example timing output

The program reports per-rank timing and the global maximum across all MPI ranks:

[TIMING][MPI] total (max over ranks): 45659.477 ms
