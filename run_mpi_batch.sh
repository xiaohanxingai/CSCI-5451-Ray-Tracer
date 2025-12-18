#!/bin/bash
set -u

# -------- basic setup --------
mkdir -p logs

timestamp=$(date +%Y%m%d-%H%M%S)
LOG="logs/mpi_selected_${timestamp}.log"

echo "MPI selected tests started at $(date)" | tee -a "$LOG"
echo "Host: $(hostname), CWD: $(pwd)"        | tee -a "$LOG"
echo ""                                      | tee -a "$LOG"

# ============================================================
#                  SELECTED TRIANGLE SCENES
# ============================================================

TRI_SCENES=(
    "Tests/TriangleExamples/test_reasonable.txt"
    "Tests/TriangleExamples/triangle.txt"
    # Uncomment if needed:
    # "Tests/TriangleExamples/outdoor.txt"
)

# ============================================================
#          SELECTED INDIVIDUAL SCENES (non-zip)
# ============================================================

EXTRA_SCENES=(
    "Tests/sword.txt"
    "Tests/spaceship.txt"
)

# ============================================================
#                MERGE BOTH LISTS INTO ALL_SCENES
# ============================================================

ALL_SCENES=("${TRI_SCENES[@]}" "${EXTRA_SCENES[@]}")

PROCS_LIST=(64)

echo "Scenes included:" | tee -a "$LOG"
for s in "${ALL_SCENES[@]}"; do
    echo "  $s" | tee -a "$LOG"
done
echo "Process counts: ${PROCS_LIST[*]}" | tee -a "$LOG"
echo "" | tee -a "$LOG"

# ============================================================
#                        RUN TESTS
# ============================================================

for np in "${PROCS_LIST[@]}"; do
    echo "==== Running tests: np=${np} ====" | tee -a "$LOG"
    for scene in "${ALL_SCENES[@]}"; do
        echo "-- [$(date +%H:%M:%S)] scene=${scene}" | tee -a "$LOG"
        mpirun -np "${np}" ./ray_mpi "${scene}" >> "$LOG" 2>&1
        echo "" | tee -a "$LOG"
    done
done

echo "All selected tests finished at $(date)" | tee -a "$LOG"
echo "Log saved to $LOG"
