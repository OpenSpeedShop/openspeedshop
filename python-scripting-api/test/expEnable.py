# expEnable [ <expId_spec> || all ]

import openss

exp1 = openss.ExpId(7)

my_modifer = openss.ModifierList("all")

openss.expEnable(my_modifer)

openss.expEnable(exp1)

openss.expEnable()
