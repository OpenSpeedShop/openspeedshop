# offline case
#module load openss
# original syntax prior to convenience scripts
openss -offline -f "./smg2000 -n 40 40 40" pcsamp 

# convenience script syntax examples

osspcsamp "./smg2000 -n 65 65 65"
ossusertime "./smg2000 -n 65 65 65"
osshwc "./smg2000 -n 65 65 65"
osshwc "./smg2000 -n 65 65 65" PAPI_L1_DCM 500000
osshwcsamp "./smg2000 -n 65 65 65" 
osshwcsamp "./smg2000 -n 65 65 65" PAPI_L1_DCM 
osshwcsamp "./smg2000 -n 65 65 65" L2_LD:PREFETCH
osshwctime "./smg2000 -n 65 65 65" 
osshwctime "./smg2000 -n 65 65 65" PAPI_L1_DCM 750000
ossio "./smg2000 -n 65 65 65" 
ossiot "./smg2000 -n 65 65 65" 

