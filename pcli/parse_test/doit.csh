setenv LD_LIBRARY_PATH /perftools/ROOT/lib:/tmp/OpenSpeedshop/lib/openspeedshop
g++ test_main.cxx /tmp/OpenSpeedshop/lib/openspeedshop/posscli.so -g -o parser -I ..
g++ input.cxx -o gen_input
gen_input > out.j

foreach file (*.input)
echo ""
echo $file
./parser $file > $file.out
end
