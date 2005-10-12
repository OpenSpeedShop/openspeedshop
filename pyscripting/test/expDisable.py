# expDisable [ <expId_spec> || all ]

import openss

exp1 = openss.ExpId(7)

my_modifer = openss.ModifierList("all")

#openss.expDisable(exp1,my_modifer)

openss.expDisable(my_modifer)

openss.expDisable(exp1)

openss.expDisable()

