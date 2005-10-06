# expPause [ <expId_spec> || all ]

import oss

exp1 = oss.ExpId(7)

my_modifer = oss.ModifierList("all")

#oss.expPause(exp1,my_modifer)

oss.expPause(my_modifer)

oss.expPause(exp1)

oss.expPause()
