# expGo [ <expId_spec> || all ]

import openss

exp1 = openss.ExpId(7)

my_modifer = openss.ModifierList("all")

#openss.expGo(exp1,my_modifer)

openss.expGo(my_modifer)

openss.expGo(exp1)

openss.expGo()
