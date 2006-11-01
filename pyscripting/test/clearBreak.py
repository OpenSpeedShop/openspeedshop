# clearBreak <breakId>

import openss

my_file = openss.FileList("../../usability/phaseII/fred")
pcsamp_viewtype = openss.ViewTypeList()
pcsamp_viewtype += "pcsamp"
exp1 = openss.expCreate(my_file,pcsamp_viewtype)

try :
    my_break = openss.setBreak(exp1,my_file,0x400144)
    openss.wait() # kluge for delayed exception
    output = openss.clearBreak(my_break)
    openss.wait() # kluge for delayed exception
    print output

except openss.error,message:
    print "ERROR: ", message

openss.exit()
