echo expcreate -f ../../../../executables/mutatee/f77_version/mutatee hwctime >> input.script
echo expgo >> input.script
echo expview stats -m inclusive_overflows >>input.script
openss -batch < input.script
