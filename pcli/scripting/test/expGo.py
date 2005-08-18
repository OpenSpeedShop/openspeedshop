# expGo [ <expId_spec> || all ]

import oss

exp1 = oss.ExpId(7)

my_modifer = oss.ModifierList("all")

oss.expGo(exp1,my_modifer)

oss.expGo(my_modifer)

oss.expGo(exp1)

oss.expGo()
