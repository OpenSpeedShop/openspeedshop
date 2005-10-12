# <ListOf_expMetric> = listMetrics [ <expId_spec> || all || <expType_list> ]

import openss

my_id = openss.ExpId(openss.expCreate())
my_exptype = openss.ExpTypeList("pcsamp")
my_modifer = openss.ModifierList("all")

output = openss.listMetrics()
output = openss.listMetrics(my_id)
output = openss.listMetrics(my_exptype)
output = openss.listMetrics(my_modifer)

print output
