# <ListOf_rankname> = listRanks [ <expId_spec> ] [ <target> ]

import openss

my_id = openss.ExpId(openss.expCreate())
my_host = openss.HostList(["host1,host2"])

output = openss.listRanks()
output = openss.listRanks(my_id)
output = openss.listRanks(my_host)

print output
