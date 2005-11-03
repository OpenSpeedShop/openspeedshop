# <ListOf_expParam> = listParams [ <expId_spec> || all || <expType_list> ]

import openss

my_file = openss.FileList("../../usability/phaseIII/fred")
my_exptype = openss.ExpTypeList("pcsamp")
my_expid = openss.expCreate(my_file,my_exptype)

my_exptype = openss.ExpTypeList("pcsamp")
my_modifer = openss.ModifierList("params")

output = openss.list(my_modifer)
print output

output = openss.list(my_modifer,my_expid)
print output

output = openss.list(my_modifer,my_exptype)
print output

my_modifer += "all"
output = openss.list(my_modifer)
print output
