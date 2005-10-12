# <expId> = expFocus [ <expId_spec> ]

import openss

my_expid = openss.ExpId(7)

exp1 = openss.expFocus(my_expid)

exp1 = openss.expFocus()

print exp1
