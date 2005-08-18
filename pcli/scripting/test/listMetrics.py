# <ListOf_expMetric> = listMetrics [ <expId_spec> || all || <expType_list> ]

import oss

my_id = oss.ExpId(oss.expCreate())
my_exptype = oss.ExpTypeList("pcsamp")
my_modifer = oss.ModifierList("all")

output = oss.listMetrics()
output = oss.listMetrics(my_id)
output = oss.listMetrics(my_exptype)
output = oss.listMetrics(my_modifer)

print output
