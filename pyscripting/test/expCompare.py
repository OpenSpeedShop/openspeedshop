#!/usr/bin/python
# expCompare [ <expId_spec> ] [ <viewType> ] [ -m <expMetric_list> ] [ <target_list> ]
import openss

my_file = openss.FileList("../../usability/phaseII/fred 900")
my_exptype = openss.ExpTypeList("usertime")
my_expid_1 = openss.expCreate(my_file,my_exptype)


openss.expGo()
openss.wait()

my_file = openss.FileList("../../usability/phaseII/fred 1500")
my_expid_2 = openss.expCreate(my_file,my_exptype)

my_viewtype = openss.ViewTypeList("usertime")
my_metric_list = openss.MetricList(["time"])
my_modifier = openss.ModifierList("function")
my_ids = openss.ExpId

data = openss.expCompare(my_modifier,my_viewtype,my_metric_list)
openss.dumpInfo(data)

openss.exit()

