# expGo [ <expId_spec> || all ]

import openss

##################################################################
#
##################################################################

my_file = openss.FileList("../../usability/phaseIII/fred")
my_viewtype = openss.ViewTypeList()
my_viewtype += "pcsamp"
exp1 = openss.expCreate(my_file,my_viewtype)

print "after expcreate: exp1 = ",exp1

my_modifer = openss.ModifierList()
my_modifer += "all"

try:
    openss.expGo(exp1,my_modifer)
    openss.waitForGo()
except openss.error:
    print "expGo(exp1,my_modifer) failed as it should"

print "after expgo #1"

openss.expGo(my_modifer)

print "after expgo #2"

openss.expGo(exp1)

print "after expgo #3"

openss.expGo()

print "after expgo #4"

openss.exit()
