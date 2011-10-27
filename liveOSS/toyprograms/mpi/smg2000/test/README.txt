# offline case
#module load openmpi-1.5.3
#module load openss
# Syntax for running smg2000 with openmpi
osspcsamp "mpirun -np 2 smg2000 -n 65 65 65"
ossusertime "mpirun -np 2 smg2000 -n 65 65 65"
osshwc "mpirun -np 2 smg2000 -n 65 65 65"
osshwcsamp "mpirun -np 2 smg2000 -n 65 65 65"
osshwctime "mpirun -np 2 smg2000 -n 65 65 65"
ossio "mpirun -np 2 smg2000 -n 65 65 65"
ossiot "mpirun -np 2 smg2000 -n 65 65 65"
ossmpi "mpirun -np 2 smg2000 -n 5 5 5"
ossmpit "mpirun -np 2 smg2000 -n 5 5 5"
ossmpiotf "mpirun -np 2 smg2000 -n 5 5 5"
ossfpe "mpirun -np 2 smg2000 -n 65 65 65"


