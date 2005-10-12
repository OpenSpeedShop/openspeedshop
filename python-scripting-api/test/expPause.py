# expPause [ <expId_spec> || all ]

import openss

exp1 = openss.ExpId(7)

my_modifer = openss.ModifierList("all")

#openss.expPause(exp1,my_modifer)

openss.expPause(my_modifer)

openss.expPause(exp1)

openss.expPause()
