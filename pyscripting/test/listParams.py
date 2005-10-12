# <ListOf_expParam> = listParams [ <expId_spec> || all || <expType_list> ]

import openss

my_id = openss.ExpId(openss.expCreate())
my_exptype = openss.ExpTypeList("pcsamp")
my_modifer = openss.ModifierList("all")

output = openss.listParams()
output = openss.listParams(my_id)
output = openss.listParams(my_exptype)
output = openss.listParams(my_modifer)

print output
