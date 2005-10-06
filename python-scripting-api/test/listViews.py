# <ListOf_viewType> = listViews [ <expId_spec> || all || <expType_list> ]

import oss

exp1 = oss.ExpId(7)
my_modifer = oss.ModifierList("all")
my_exptype = oss.ExpTypeList("pcsamp")

output = oss.listViews(my_modifer)

output = oss.listViews(exp1)

output = oss.listViews(my_exptype)

output = oss.listViews()

print output
