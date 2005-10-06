# <ListOf_threadname> = listThreads [ <expId_spec> ] [ <host_list_spec> ]

import oss

my_id = oss.ExpId(oss.expCreate())
my_host = oss.HostList(["host1,host2"])

output = oss.listThreads()
output = oss.listThreads(my_id)
output = oss.listThreads(my_host)
output = oss.listThreads(my_id,my_host)

print output
