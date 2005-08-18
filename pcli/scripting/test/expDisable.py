# expDisable [ <expId_spec> || all ]

import oss

exp1 = oss.ExpId(7)

my_modifer = oss.ModifierList("all")

oss.expDisable(exp1,my_modifer)

oss.expDisable(my_modifer)

oss.expDisable(exp1)

oss.expDisable()

