# <ListOf_hostname> = listHosts [ <expId_spec> ]


import openss

my_id = openss.ExpId(openss.expCreate())

output = openss.listHosts()
output = openss.listHosts(my_id)
print output
