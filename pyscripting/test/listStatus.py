# <ListOf_statusType> = listStatus [ <expId_spec> || all ]

import openss

my_file = openss.FileList("../../usability/phaseII/fred")

list_type = openss.ModifierList("status")

# Define and run pcsamp experiment
pcsamp_viewtype = openss.ViewTypeList()
pcsamp_viewtype += "pcsamp"
pcsamp_expid = openss.expCreate(my_file,pcsamp_viewtype)
openss.expGo()
output = openss.list(list_type,pcsamp_expid)
print "expect Running"
print output
openss.wait()

user_viewtype = openss.ViewTypeList()
user_viewtype += "usertime"
user_expid = openss.expCreate(my_file,user_viewtype)

output = openss.list(list_type,user_expid)
print "expect Paused"
print output
output = openss.list(list_type,pcsamp_expid)
print "expect Terminated"
print output

openss.expGo()
openss.wait()

#my_modifer = openss.ModifierList("all")
#output = openss.listStatus(my_modifer)

output = openss.list(list_type,user_expid)
print "expect Terminated"
print output
