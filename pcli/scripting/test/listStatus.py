# <ListOf_statusType> = listStatus [ <expId_spec> || all ]

import oss

exp1 = oss.ExpId(7)

my_modifer = oss.ModifierList("all")

output = oss.listStatus(my_modifer)

output = oss.listStatus(exp1)

output = oss.listStatus()

print output
