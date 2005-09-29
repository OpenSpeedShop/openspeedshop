#!/bin/sh
rm -f input.script
echo expcreate -f ../../../../../executables/mutatee/c_version/mutatee hwc >> input.script
echo expgo >> input.script
echo expclose -v kill,all >> input.script
echo exit >> input.script
openss -batch < input.script
