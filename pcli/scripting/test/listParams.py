# <ListOf_expParam> = listParams [ <expId_spec> || all || <expType_list> ]

import oss

my_id = oss.ExpId(oss.expCreate())
my_exptype = oss.ExpTypeList("pcsamp")
my_modifer = oss.ModifierList("all")

output = oss.listParams()
output = oss.listParams(my_id)
output = oss.listParams(my_exptype)
output = oss.listParams(my_modifer)

print output
