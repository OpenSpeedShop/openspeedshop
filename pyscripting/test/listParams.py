# <ListOf_expParam> = listParams [ <expId_spec> || all || <expType_list> ]

import openss

my_file = openss.FileList("../../usability/phaseII/fred")
my_exptype = openss.ExpTypeList("pcsamp")
my_expid = openss.expCreate(my_file,my_exptype)

my_exptype = openss.ExpTypeList("pcsamp")
my_modifier = openss.ModifierList("params")

output = openss.list(my_modifier)
print output

output = openss.list(my_modifier,my_expid)
print output

output = openss.list(my_modifier,my_exptype)
print output

my_modifier += "all"
output = openss.list(my_modifier)
print output

openss.exit()
