# expClose [ <expId_spec> || all ] [ kill ]

import openss

exp1 = openss.ExpId(7)

my_modifer = openss.ModifierList("all")

openss.expClose(exp1,my_modifer)

my_modifer = openss.ModifierList("kill")

openss.expClose(my_modifer)

openss.expClose(exp1)

openss.expClose()

