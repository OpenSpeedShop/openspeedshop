# record [ <file_spec> ]

import oss

my_file = oss.FileList("myexp.oss")

output = oss.oss_record(my_file)
output = oss.oss_record()

print output
