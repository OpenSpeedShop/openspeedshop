#!/bin/sh
rm -f input.script
echo expCreate  -f ../../../../../executables/mutatee/cplus_version/mutatee usertime >> input.script
echo listmetrics >> input.script
echo exit >> input.script
openss -batch < input.script
