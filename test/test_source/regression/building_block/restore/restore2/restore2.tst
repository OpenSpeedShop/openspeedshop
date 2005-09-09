#!/bin/sh
rm -f input*
rm -f results.data
echo expCreate  -f ../../../../../executables/mutatee/cplus_version/mutatee usertime>> input1.script
echo expgo -x 1 >> input1.script 
echo expview stats -m usertime::inclusive_time >> input1.script
echo expSave -f results.data >> input1.script
echo exit >> input1.script
openss -batch < input1.script
echo exprestore -f results.data >> input2.script
echo expgo >> input2.script
echo expview stats -m usertime::inclusive_time >> input2.script
echo exit >> input2.script
openss -batch < input2.script
