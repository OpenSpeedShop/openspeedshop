# clearBreak <breakId>

import oss

my_file_bosco = oss.FileList("bosco")
exp1 = oss.ExpId(7)
my_break = oss.oss_setBreak(exp1,my_file_bosco,0x400144)

output = oss.oss_clearBreak(my_break)

print output
