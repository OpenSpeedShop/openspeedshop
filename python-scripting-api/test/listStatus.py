# <ListOf_statusType> = listStatus [ <expId_spec> || all ]

import openss

exp1 = openss.ExpId(7)

my_modifer = openss.ModifierList("all")

output = openss.listStatus(my_modifer)

output = openss.listStatus(exp1)

output = openss.listStatus()

print output
