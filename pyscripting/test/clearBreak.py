# clearBreak <breakId>

import openss

my_file_bosco = openss.FileList("bosco")
exp1 = openss.ExpId(7)
my_break = openss.openss_setBreak(exp1,my_file_bosco,0x400144)

output = openss.openss_clearBreak(my_break)

print output
