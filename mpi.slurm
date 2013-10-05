#!/bin/sh
	
# Time limit is one minute.
# See "man sbatch" for other time formats.
#SBATCH --time=1

# Run 10 tasks on 2 nodes.
#SBATCH -N 2 -n 10

# Output goes to job.out, error messages to job.err.
#SBATCH --error=job.err --output=job.out

# at first compile it, to ensure there is a valid version
make compile

srun hostname
mpiexec ./predators.out