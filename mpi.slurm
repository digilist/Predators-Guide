#!/bin/sh

#SBATCH --time=10
#SBATCH -N 1 -n 1
#SBATCH --error=output/job_1_1.err --output=output/job_1_1.out

srun hostname
mpiexec ./predators.out
