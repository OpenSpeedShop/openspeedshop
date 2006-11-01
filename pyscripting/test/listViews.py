# <ListOf_viewType> = list -v exptype [ <expId_spec> || all || <expType_list> ]

import openss

my_file = openss.FileList("../../usability/phaseII/fred 900")
my_exptype = openss.ExpTypeList("pcsamp")
my_expid = openss.expCreate(my_file,my_exptype)

my_modifer = openss.ModifierList("all")
list_type = openss.ModifierList("views")

output = openss.list(my_modifer,list_type)
print output

output = openss.list(my_expid,list_type)
print output

output = openss.list(my_exptype,list_type)
print output

output = openss.list(list_type)
print output

openss.exit()
