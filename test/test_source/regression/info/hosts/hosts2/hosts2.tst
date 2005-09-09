#!/bin/sh
rm -f input.script
rm -f *.expected*
hostname >> hosts2.tst.expected
echo expCreate  -f ../../../../../executables/mutatee/cplus_version/mutatee usertime >> input.script
echo listhosts >> input.script
echo exit >> input.script
openss -batch < input.script
