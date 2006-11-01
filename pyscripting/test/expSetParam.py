# expSetParam  [ <expId_spec> ] <expParam> = <expParamValue> [ , <expParam> = <expParamValue> ]+

# This test is really not done yet. 
#
# I need to have more parameter variations for
# the general purpose experiments to make this
# testable on every system.

import openss
import os

my_file = openss.FileList("../../usability/phaseII/fred 90")
my_viewtype = openss.ViewTypeList("pcsamp")
my_expid = openss.expCreate(my_file,my_viewtype)

my_param_list = openss.ParamList()
#my_param_list += "exclusive"
my_param_list += ("parmtype_1","parmval_1")
my_param_list += ("parmtype_2","parmval_2")
my_param_list += ("parmtype_3","parmval_3")

# There is a bug (#572) where asynchronous CLI 
# behavior will allow the expSetParam to return
# before the error handling within openss determines
# that there is an error and throws an exception.
#
# That is why we have the series of try/except segments.
#
print "before #1"
try :
    openss.expSetParam(my_expid,my_param_list)
    os.system("sleep 3")
except openss.error,message:
    print "#1 expected ERROR: ", message

print "before #2"
try :
    openss.list(openss.ModifierList("exptypes"))
except openss.error,message:
    print "#2 expected ERROR: ", message


print "before #3"
try :
    os.system("sleep 3")
except openss.error,message:
    print "#3 expected ERROR: ", message

my_modifier = openss.ModifierList("params")
output = openss.list(my_modifier)
print output

print "before #4"
try :
    my_param_list = ("pcsamp::sampling_rate=2000")
    openss.expSetParam(my_expid,my_param_list)

    #my_modifier += "all"
    output = openss.list(my_modifier)
    print output
except openss.error,message:
    print "#4 expected ERROR: ", message

openss.exit()
