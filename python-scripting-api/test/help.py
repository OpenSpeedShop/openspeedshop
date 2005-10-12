# help [<help_modifier_list>] [<keyword>]

import oss
oss.OSS_Init()

my_modifer = oss.ModifierList("brief")
my_modifer+= "examples"

output = oss.oss_help(my_modifer,"expAttach")
output = oss.oss_help(my_modifer)
output = oss.oss_help("expAttach")
output = oss.oss_help()

print output
