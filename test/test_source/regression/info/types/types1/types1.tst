#!/bin/sh
rm -f input.script
echo expCreate  -f ../../../../../executables/mutatee/cplus_version/mutatee pcsamp >> input.script
echo listTypes >> input.script
echo exit >> input.script
openss -batch < input.script
