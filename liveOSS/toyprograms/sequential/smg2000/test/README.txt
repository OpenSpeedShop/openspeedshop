# offline case
#module load openmpi-1.5.3
#module load openss
# original syntax prior to convenience scripts
openss -offline -f "mpirun -np 2 smg2000 -n 40 40 40" pcsamp 

# convenience script syntax examples

osspcsamp "mpirun -np 2 ./smg2000 -n 65 65 65"
ossusertime "mpirun -np 2 ./smg2000 -n 65 65 65"
osshwc "mpirun -np 2 ./smg2000 -n 65 65 65"
osshwc "mpirun -np 2 ./smg2000 -n 65 65 65" PAPI_L1_DCM 500000
osshwcsamp "mpirun -np 2 ./smg2000 -n 65 65 65" 
osshwcsamp "mpirun -np 2 ./smg2000 -n 65 65 65" PAPI_L1_DCM 
osshwcsamp "mpirun -np 2 ./smg2000 -n 65 65 65" L2_LD:PREFETCH
osshwctime "mpirun -np 2 ./smg2000 -n 65 65 65" 
osshwctime "mpirun -np 2 ./smg2000 -n 65 65 65" PAPI_L1_DCM 750000
ossio "mpirun -np 2 ./smg2000 -n 65 65 65" 
ossiot "mpirun -np 2 ./smg2000 -n 65 65 65" 
ossmpi "mpirun -np 2 ./smg2000 -n 10 10 10" 
ossmpit "mpirun -np 2 ./smg2000 -n 10 10 10" 
ossmpiotf "mpirun -np 2 ./smg2000 -n 10 10 10" 

