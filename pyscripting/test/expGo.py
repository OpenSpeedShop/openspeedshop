# expGo [ <expId_spec> || all ]

import openss

##################################################################
#
##################################################################

my_file = openss.FileList("../../usability/phaseII/fred")
my_viewtype = openss.ViewTypeList()
my_viewtype += "pcsamp"
exp1 = openss.expCreate(my_file,my_viewtype)

my_modifer = openss.ModifierList()
my_modifer += "all"

try:
    openss.expGo(exp1,my_modifer)
    openss.waitForGo()
except openss.error:
    print "expGo(exp1,my_modifer) failed as it should"

print "after expgo #1"

# If we don't pause we may get an exception
# if the experiment has already terminated.
openss.expGo(my_modifer)
openss.expPause()
print "after expgo #2"

openss.expGo(exp1)
openss.expPause()
print "after expgo #3"

openss.expGo()
openss.expPause()
print "after expgo #4"

openss.exit()
