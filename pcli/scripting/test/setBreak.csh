# <breakId> = setbreak [ <expId_spec> ] [ <target> ] <address_description>

import oss

my_host = oss.HostList()
my_host += ["bosco","111.222.333.444"]

my_file_bosco = oss.FileList("bosco")

exp1 = oss.ExpId(7)

output = oss.oss_setBreak(exp1,my_file_bosco,0x30000000)
output = oss.oss_setBreak(exp1,0x40000000)
output = oss.oss_setBreak(my_file_bosco,0x50000000)
output = oss.oss_setBreak(0x60000000)

print output
