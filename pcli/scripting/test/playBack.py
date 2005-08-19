# playBack <file_spec>

import oss

my_file = oss.FileList("myexp.oss")

output = oss.oss_playBack(my_file)
output = oss.oss_playBack()

print output
