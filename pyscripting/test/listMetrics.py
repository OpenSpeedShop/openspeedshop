# <ListOf_expMetric> = listMetrics [ <expId_spec> || all || <expType_list> ]

import openss

list_type = openss.ModifierList("metrics")

my_file = openss.FileList("../../usability/phaseII/fred")
my_exptype = openss.ViewTypeList("pcsamp")
my_id = openss.expCreate(my_file,my_exptype)

my_modifer = openss.ModifierList("all")

output = openss.list(list_type)
print output
output = openss.list(list_type,my_id)
print output
output = openss.list(list_type,my_exptype)
print output
output = openss.list(list_type,my_modifer)
print output

openss.exit()
