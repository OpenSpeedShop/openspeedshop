# <ListOf_expType> = listTypes [ <expId_spec> || all ]

import oss

exp1 = oss.ExpId(7)

my_modifer = oss.ModifierList("all")

output = oss.listTypes(my_modifer)

output = oss.listTypes(exp1)

output = oss.listTypes()

print output
