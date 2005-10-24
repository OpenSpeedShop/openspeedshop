# <ListOf_expType> = listTypes [ <expId_spec> || all ]

import openss

my_file = openss.FileList("../../usability/phaseIII/fred")

# Define and run pcsamp experiment
pcsamp_viewtype = openss.ViewTypeList()
pcsamp_viewtype += "pcsamp"
pcsamp_expid = openss.expCreate(my_file,pcsamp_viewtype)
openss.expGo()

user_viewtype = openss.ViewTypeList()
user_viewtype += "usertime"
user_expid = openss.expCreate(my_file,user_viewtype)
openss.expGo()

#my_modifer = openss.ModifierList("all")
#output = openss.listTypes(my_modifer)

output = openss.listTypes()
print output

output = openss.listTypes(pcsamp_expid)
print output
