
# You will need to change these paths to fit your environment
setenv LD_LIBRARY_PATH /perftools/ROOT/lib:/tmp/OpenSpeedshop/lib/openspeedshop
setenv OPENSPEEDSHOP_PLUGIN_PATH /tmp/OpenSpeedshop/lib/openspeedshop

g++ test_main.cxx $OPENSPEEDSHOP_PLUGIN_PATH/posscli.so -g -o parser -I ..
g++ input.cxx -o gen_input
gen_input 

foreach file (*.input)
echo ""
echo $file
./parser $file > $file.out
end
