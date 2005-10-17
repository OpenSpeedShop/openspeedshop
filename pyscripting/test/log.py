# log [ <file_spec> ]

import openss

my_file = openss.FileList("myexp.log")

output = openss.log(my_file)
output = openss.log()

print output
