#!/bin/bash
#SBATCH --mem=200G
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=128    # <- match to OMP_NUM_THREADS
#SBATCH --partition=cpu-interactive     # <- or one of: gpuA100x4 gpuA40x4 gpuA100x8 gpuMI100x8
#SBATCH --account=bcev-delta-cpu    # <- match to a "Project" returned by the "accounts" command
#SBATCH --job-name=sieve2
#SBATCH --time=00:10:00      # hh:mm:ss for the job
#SBATCH --output=./logs/sieve2_%j.out    # <- output file

set -v

cd /projects/bcev/fye1/repo/HPC_project

export OMP_NUM_THREADS=128


# mpicc -o sieve1 sieve1.c -lm
# for np in 1 2 4 8 16 32 64 128; do
#     echo "Running with ${np} processes:" >> ./results/sieve1_results.txt
#     srun -n ${np} ./sieve1 1000000000 >> ./results/sieve1_results.txt
#     echo "--------------------------------" >> ./results/sieve1_results.txt
# done

mpicc -o sieve2 sieve2.c -lm
for np in 1 2 4 8 16 32 64 128; do
    echo "Running with ${np} processes:" >> ./results/sieve2_results2.txt
    srun -n ${np} ./sieve2 1000000000 >> ./results/sieve2_results2.txt
    echo "--------------------------------" >> ./results/sieve2_results2.txt
done

# mpicc -o sieve3 sieve3.c -lm
# for np in 1 2 4 8 16 32 64 128; do
#     echo "Running with ${np} processes:" >> ./results/sieve3_results.txt
#     srun -n ${np} ./sieve3 1000000000 >> ./results/sieve3_results.txt
#     echo "--------------------------------" >> ./results/sieve3_results.txt
# done

# mpicc -o sieve4 sieve4.c -lm
# for np in 1 2 4 8 16 32 64 128; do
#     echo "Running with ${np} processes:" >> ./results/sieve4_results.txt
#     srun -n ${np} ./sieve4 1000000000 8388608 >> ./results/sieve4_results.txt
#     echo "--------------------------------" >> ./results/sieve4_results.txt
# done

