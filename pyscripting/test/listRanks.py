# <ListOf_rankname> = listRanks [ <expId_spec> ] [ <target> ]

import openss

list_type = openss.ModifierList("status")

my_file = openss.FileList("../../usability/phaseIII/fred")

my_host = openss.HostList("localhost")
my_id = openss.expCreate(my_file,my_host)

output = openss.list(list_type)
print output
output = openss.list(list_type,my_id)
print output
output = openss.list(list_type,my_host)

print output

openss.exit()
