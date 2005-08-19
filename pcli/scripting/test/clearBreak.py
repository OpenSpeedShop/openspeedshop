# clearBreak <breakId>

import oss

my_modifer = oss.ModifierList("all")

my_breaks = oss.BreakList(oss.listBreaks())

output = oss.oss_clearBreak(my_modifer)

output = oss.oss_clearBreak(my_breaks)

print output
