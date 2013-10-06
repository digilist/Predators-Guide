#!/bin/bash

# just a little tool to simplify the batch processing

N=$1
n=$2

slurm=$( cat <<EOF
#!/bin/sh

#SBATCH --time=10
#SBATCH -N $N -n $n
#SBATCH --error=output/job_${N}_${n}.err --output=output/job_${N}_${n}.out

srun hostname
mpiexec ./predators.out
EOF
)

echo "$slurm" > mpi.slurm

touch output/job_${N}_${n}.out

make compile
sbatch mpi.slurm
