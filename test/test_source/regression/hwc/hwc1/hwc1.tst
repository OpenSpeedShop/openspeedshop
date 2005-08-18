echo expcreate -f ../../../../executables/mutatee/c_version/mutatee hwc >> input.script
echo expgo >> input.script
echo expview  >>input.script
openss -batch < input.script
