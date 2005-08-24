# <listOf_filename> = listSrc [ <expId_spec> ] [ <target> ] 

import oss

my_id = oss.ExpId(oss.expCreate())
my_host = oss.HostList(["host1,host2"])

output = oss.listSrc()
output = oss.listSrc(my_id)
output = oss.listSrc(my_host)
output = oss.listSrc(my_id,my_host)

print output
