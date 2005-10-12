# <ListOf_breakId> = listBreaks [ <expId_spec> || all ]

import openss

exp1 = openss.ExpId(7)

my_modifer = openss.ModifierList("all")

output = openss.listBreaks(my_modifer)

output = openss.listBreaks(exp1)

output = openss.listBreaks()

print output
