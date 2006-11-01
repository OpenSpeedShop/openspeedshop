# <ListOf_breakId> = listBreaks [ <expId_spec> || all ]

import openss

list_type = openss.ModifierList("breaks")

my_file = openss.FileList("../../usability/phaseII/fred 90")
my_viewtype = openss.ViewTypeList("pcsamp")
exp1 = openss.expCreate(my_file,my_viewtype)

my_modifer = openss.ModifierList("all")

output = openss.list(list_type,my_modifer)
print output

output = openss.list(list_type,exp1)
print output

output = openss.list(list_type)
print output

openss.exit()
