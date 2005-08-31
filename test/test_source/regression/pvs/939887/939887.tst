#!/bin/sh
rm -f input.script
../../../../executables/forever_cxx/forever >> file &
ls -l file
cat file
read i < file
rm file
../../../../executables/forever_cxx/forever >> file &
ls -l file
cat file
read j < file
rm file
echo expCreate  -p $i >> input.script
echo expattach -x 1 pcsamp >> input.script
echo expgo -x -1 >> input.script
echo expattach -x 1 -p $j >> input.script
echo expview stats5 -m time >> input.script
echo exit >> input.script
openss -batch < input.script
kill $i $j

