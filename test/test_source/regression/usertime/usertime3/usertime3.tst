echo expcreate -f ../../../../executables/mutatee/64bit/cplus_version/mutatee usertime >> input.script
echo expgo >> input.script
echo expview stats -m usertime::time >>input.script
openss -batch < input.script
