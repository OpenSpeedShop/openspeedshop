echo expcreate -f ../../../../executables/mutatee/64bit/c_version/mutatee usertime >> input.script
echo expgo >> input.script
echo expview stats -m usertime::inclusive_time >>input.script
openss -batch < input.script
