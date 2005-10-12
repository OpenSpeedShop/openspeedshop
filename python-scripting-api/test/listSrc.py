# <listOf_filename> = listSrc [ <expId_spec> ] [ <target> ] 

import openss

my_id = openss.ExpId(openss.expCreate())
my_host = openss.HostList(["host1,host2"])

output = openss.listSrc()
output = openss.listSrc(my_id)
output = openss.listSrc(my_host)
output = openss.listSrc(my_id,my_host)

print output
