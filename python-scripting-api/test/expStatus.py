# expStatus [ <expId_spec> || all ]

import openss

exp1 = openss.ExpId(7)

my_modifer = openss.ModifierList("all")

openss.expStatus(my_modifer)

openss.expStatus(exp1)

openss.expPause()
