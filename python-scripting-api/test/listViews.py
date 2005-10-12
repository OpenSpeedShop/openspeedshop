# <ListOf_viewType> = listViews [ <expId_spec> || all || <expType_list> ]

import openss

exp1 = openss.ExpId(7)
my_modifer = openss.ModifierList("all")
my_exptype = openss.ExpTypeList("pcsamp")

output = openss.listViews(my_modifer)

output = openss.listViews(exp1)

output = openss.listViews(my_exptype)

output = openss.listViews()

print output
