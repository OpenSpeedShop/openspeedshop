# <ListOf_expType> = listTypes [ <expId_spec> || all ]

import openss

exp1 = openss.ExpId(7)

my_modifer = openss.ModifierList("all")

output = openss.listTypes(my_modifer)

output = openss.listTypes(exp1)

output = openss.listTypes()

print output
