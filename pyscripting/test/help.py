# help [<help_modifier_list>] [<keyword>]

import openss
#openss.OSS_Init()

my_modifer = openss.ModifierList("brief")
my_modifer+= "examples"

output = openss.openss_help(my_modifer,"expAttach")
output = openss.openss_help(my_modifer)
output = openss.openss_help("expAttach")
output = openss.openss_help()

print output
