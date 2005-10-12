# expSetParam  [ <expId_spec> ] <expParam> = <expParamValue> [ , <expParam> = <expParamValue> ]+

import openss

my_expid = openss.ExpId(7)

my_param_list = openss.ParamList()
#my_param_list += "exclusive"
my_param_list += ("parmtype_1","parmval_1")
my_param_list += ("parmtype_2","parmval_2")
my_param_list += ("parmtype_3","parmval_3")

openss.expSetParam(my_expid,my_param_list)
