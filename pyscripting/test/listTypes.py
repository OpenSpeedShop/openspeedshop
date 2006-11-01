# <ListOf_expType> = listTypes [ <expId_spec> || all ]

import openss

my_file = openss.FileList("../../usability/phaseII/fred")

# Define and run pcsamp experiment
pcsamp_viewtype = openss.ViewTypeList()
pcsamp_viewtype += "pcsamp"
pcsamp_expid = openss.expCreate(my_file,pcsamp_viewtype)
openss.expGo()
openss.wait()

user_viewtype = openss.ViewTypeList()
user_viewtype += "usertime"
user_expid = openss.expCreate(my_file,user_viewtype)
openss.expGo()
openss.wait()

list_type = openss.ModifierList("types")

my_modifer = openss.ModifierList("all")
output = openss.list(list_type,my_modifer)
print output

output = openss.list(list_type)
print output

output = openss.list(list_type,pcsamp_expid)
print output
