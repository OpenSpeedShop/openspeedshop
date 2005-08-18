# <ListOf_rankname> = listRanks [ <expId_spec> ] [ <target> ]

import oss

my_id = oss.ExpId(oss.expCreate())
my_host = oss.HostList(["host1,host2"])

output = oss.listRanks()
output = oss.listRanks(my_id)
output = oss.listRanks(my_host)

print output
