# <string> = listSrc [ <expId_spec> ] [ <target> ] [ <linenumber_list_spec> ]

import oss

my_id = oss.ExpId(oss.expCreate())
my_host = oss.HostList(["host1,host2"])
my_lineno = oss.LineNoList([1111,2222,3333])

output = oss.listSrc()
output = oss.listSrc(my_id)
output = oss.listSrc(my_host)
output = oss.listSrc(my_id,my_host)

output = oss.listSrc(my_lineno)
output = oss.listSrc(my_id,my_lineno)
output = oss.listSrc(my_host,my_lineno)
output = oss.listSrc(my_id,my_host,my_lineno)

print output
