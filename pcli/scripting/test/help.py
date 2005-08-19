# help [<help_modifier_list>] [<keyword>]

import oss

my_modifer = oss.ModifierList("brief")
my_modifer+= "examples"

output = oss.oss_help()
output = oss.oss_help(my_modifer)
output = oss.oss_help(my_modifer,"expAttach")
output = oss.oss_help("expAttach")

print output
