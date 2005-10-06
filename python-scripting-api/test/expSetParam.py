# expSetParam  [ <expId_spec> ] <expParam> = <expParamValue> [ , <expParam> = <expParamValue> ]+

import oss

my_expid = oss.ExpId(7)

my_param_list = oss.ParamList()
#my_param_list += "exclusive"
my_param_list += ("parmtype_1","parmval_1")
my_param_list += ("parmtype_2","parmval_2")
my_param_list += ("parmtype_3","parmval_3")

oss.expSetParam(my_expid,my_param_list)
