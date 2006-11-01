# <expId> = expRestore  <file_spec>

import openss

data_file = "file_1.dat"
my_data_file = openss.FileList(data_file)

# Run and save and experiment to read in.
my_file = openss.FileList("../../usability/phaseII/fred 900")
my_viewtype = openss.ViewTypeList("pcsamp")
my_expid = openss.expCreate(my_file,my_viewtype)
openss.expGo()
openss.wait()
openss.expSave(my_data_file)

try:
    my_file = openss.FileList("bad_name")
    exp1 = openss.expRestore(my_file)
    print exp1
except openss.error,message:
    print "Expected error: ", message

try:
    exp1 = openss.expRestore(my_data_file)
    print exp1
    info = openss.expView(exp1)
    print info
except openss.error,message:
    print "Unexpected error: ", message

openss.exit()
   

