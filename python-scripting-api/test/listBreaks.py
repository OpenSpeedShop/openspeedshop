# <ListOf_breakId> = listBreaks [ <expId_spec> || all ]

import oss

exp1 = oss.ExpId(7)

my_modifer = oss.ModifierList("all")

output = oss.listBreaks(my_modifer)

output = oss.listBreaks(exp1)

output = oss.listBreaks()

print output
