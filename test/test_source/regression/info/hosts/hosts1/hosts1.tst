#!/bin/sh
rm -f input.script
rm -f *.expected*
hostname >> hosts1.tst.expected
echo expCreate  -f ../../../../../executables/mutatee/cplus_version/mutatee pcsamp >> input.script
echo listhosts >> input.script
echo exit >> input.script
openss -batch < input.script
