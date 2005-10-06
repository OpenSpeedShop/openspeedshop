# expClose [ <expId_spec> || all ] [ kill ]

import oss

exp1 = oss.ExpId(7)

my_modifer = oss.ModifierList("all")

oss.expClose(exp1,my_modifer)

my_modifer = oss.ModifierList("kill")

oss.expClose(my_modifer)

oss.expClose(exp1)

oss.expClose()

