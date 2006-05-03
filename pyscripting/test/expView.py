#!/usr/bin/python
# expView [ <expId_spec> ] [ <viewType> ] [ -m <expMetric_list> ] [ <target_list> ]
import openss

my_file = openss.FileList("../../usability/phaseII/fred 900")
my_exptype = openss.ExpTypeList("pcsamp")
my_expid = openss.expCreate(my_file,my_exptype)

my_viewtype = openss.ViewTypeList("pcsamp")
my_metric_list = openss.MetricList(["pcsamp","time"])


openss.expGo()
wait_mod = openss.ModifierList("terminate")
#openss.wait(wait_mod)
openss.wait()
#openss.waitForGo()
openss.dumpView()


#openss.expView(my_expid)

#openss.expView(my_expid,my_viewtype)

#ret = openss.expView(my_expid,my_viewtype,my_metric_list)

# Dump the raw return data.
#print ret
