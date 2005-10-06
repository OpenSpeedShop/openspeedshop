# <ListOf_hostname> = listHosts [ <expId_spec> ]


import oss

my_id = oss.ExpId(oss.expCreate())

output = oss.listHosts()
output = oss.listHosts(my_id)
print output
