# <ListOf_threadname> = listThreads [ <expId_spec> ] [ <host_list_spec> ]

import openss

my_id = openss.ExpId(openss.expCreate())
my_host = openss.HostList(["host1,host2"])

output = openss.listThreads()
output = openss.listThreads(my_id)
output = openss.listThreads(my_host)
output = openss.listThreads(my_id,my_host)

print output
