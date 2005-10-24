# <ListOf_statusType> = listStatus [ <expId_spec> || all ]

import openss

my_file = openss.FileList("../../usability/phaseIII/fred")

# Define and run pcsamp experiment
pcsamp_viewtype = openss.ViewTypeList()
pcsamp_viewtype += "pcsamp"
pcsamp_expid = openss.expCreate(my_file,pcsamp_viewtype)
openss.expGo()
openss.wait_for_go()

user_viewtype = openss.ViewTypeList()
user_viewtype += "usertime"
user_expid = openss.expCreate(my_file,user_viewtype)

output = openss.listStatus(user_expid)
print output
output = openss.listStatus(pcsamp_expid)
print output

openss.expGo()
openss.wait_for_go()

#my_modifer = openss.ModifierList("all")
#output = openss.listStatus(my_modifer)

output = openss.listStatus(user_expid)
print output
