
# You will need to change these paths to fit your environment
setenv LD_LIBRARY_PATH /home/jcarter/workarea/bits/lib
setenv OPENSS_PLUGIN_PATH /home/jcarter/workarea/bits/lib/openspeedshop

make -f parse_test.make

gen_input 

foreach file (*.input)
echo ""
echo $file
./parser $file > $file.out
end
