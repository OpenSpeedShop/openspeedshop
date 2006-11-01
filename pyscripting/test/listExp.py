# <listOf_expId> = listExp

import openss

list_type = openss.ModifierList("exptypes")
    
#try:
#    output = openss.list(list_type)
#    print "expect None"
#    print output
#except openss.error,message:
#    print message

my_file = openss.FileList("../../usability/phaseII/fred")

# Define pcsamp experiment
pcsamp_viewtype = openss.ViewTypeList()

pcsamp_viewtype += "pcsamp"

pcsamp_expid = openss.expCreate(my_file,pcsamp_viewtype)

# Define usertime experiment
user_viewtype = openss.ViewTypeList()
user_viewtype += "usertime"
user_expid = openss.expCreate(my_file,user_viewtype)

output = openss.list(list_type)
print "expect usertime"
print output

output = openss.list(list_type,pcsamp_expid)
print "expect pcsamp"
print output

output = openss.list(list_type,user_expid)
print "expect usertime"
print output

