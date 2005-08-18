# expEnable [ <expId_spec> || all ]

import oss

exp1 = oss.ExpId(7)

my_modifer = oss.ModifierList("all")

oss.expEnable(exp1,my_modifer)

oss.expEnable(my_modifer)

oss.expEnable(exp1)

oss.expEnable()
