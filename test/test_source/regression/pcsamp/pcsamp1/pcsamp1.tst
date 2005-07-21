echo expcreate -f ../../../../executables/mutatee/c_version/mutatee pcsamp >> input.script
echo expgo >> input.script
echo expview stats5 -m time >>input.script
openss -batch < input.script
