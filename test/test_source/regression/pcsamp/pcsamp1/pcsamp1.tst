echo expcreate -f $PWD/mutatee pcsamp >> input.script
echo expgo >> input.script
echo expview vtop5 >>input.script
/scratch/dew/bin/openss -batch < input.script
