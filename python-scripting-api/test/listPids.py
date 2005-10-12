# <ListOf_pidname> = listPids [ <expId_spec> ] [ <host_list_spec> ]

import openss

my_id = openss.ExpId(openss.expCreate())
my_host = openss.HostList(["host1,host2"])

output = openss.listPids()
output = openss.listPids(my_id)
output = openss.listPids(my_host)

print output
