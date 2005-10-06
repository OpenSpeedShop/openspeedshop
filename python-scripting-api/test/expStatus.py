# expStatus [ <expId_spec> || all ]

import oss

exp1 = oss.ExpId(7)

my_modifer = oss.ModifierList("all")

oss.expStatus(my_modifer)

oss.expStatus(exp1)

oss.expPause()
