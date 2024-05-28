#!/bin/bash
#SBATCH --job-name=龙葵
#SBATCH --partition=thcp1
#SBATCH --nodes=16
#SBATCH --ntasks=16
#SBATCH --output=out/out_%j.out
#SBATCH --error=err/err_%j.err
export OMP_NUM_THREADS=64
yhrun  ~/Yusheng_Gao/GYS_Contraction/baryon_pi_rescattering_32Dfine/build/qlat.x
