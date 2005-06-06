
# You will need to change these paths to fit your environment
setenv LD_LIBRARY_PATH /perftools/ROOT/lib:/home/jcarter/workarea/bits/lib
setenv OPENSS_PLUGIN_PATH /home/jcarter/workarea/bits/lib/openspeedshop

g++ test_main.cxx \
    $OPENSS_PLUGIN_PATH/../libopenss-cli.so \
    -g -o parser \
    -I .. \
    -I ../../base \
    -I ../../message \
    -I ../../libopenss-framework \
    -I ../../objects  >& out.j

    g++ \
    input.cxx \
    -o gen_input

gen_input 

foreach file (*.input)
echo ""
echo $file
./parser $file > $file.out
end
