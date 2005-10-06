# <expId> = expFocus [ <expId_spec> ]

import oss

my_expid = oss.ExpId(7)

exp1 = oss.expFocus(my_expid)

exp1 = oss.expFocus()

print exp1
