echo expcreate -f ../../../../executables/mutatee/f77_version/mutatee hwc >> input.script
echo expgo >> input.script
echo expview  >>input.script
openss -batch < input.script
