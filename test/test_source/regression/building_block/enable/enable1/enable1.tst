#!/bin/sh
rm -f input.script
../../../../../executables/forever_cxx/forever >> file &
ls -l file
cat file
read i < file
rm file
echo expCreate  -p $i >> input.script
echo expattach -x 1 pcsamp >> input.script
echo expdisable >> input.script
echo expstatus >> input.script
echo expenable >> input.script
echo expstatus >> input.script
echo exit >> input.script
openss -batch < input.script
