import code
import os
import os.path
import re
import string
import sys
import types
import tempfile

import PY_Input


def LDPCL(template="*"):
    StartDPCLwithLibs(template,os.environ['OPENSS_PREFIX'],os.environ['OPENSS_PREFIX'])

def DPCL():
    print "Starting DPCL daemon on local node"
    LDPCL("*")

def SDPCL():
#    print "CPUs is",os.environ['SLURM_CPUS_ON_NODE']
    if (os.environ.has_key("SLURM_JOBID")):
        LDPCL("srun -c 8 -l *")
    elif (os.environ.has_key("PBS_JOBID")):
	skip_backend = 0
	backend_nodes_present = 1
	ndirname = tempfile.mktemp()
	os.system('echo $PBS_NODEFILE > ndirname')
	nodelist_input_file = open('ndirname', 'r')
	nodelist_line = nodelist_input_file.readline()
	if not nodelist_line:
		backend_nodes_present = 0
#
#
	if backend_nodes_present:
		print "nodelist_line=%s\n" % nodelist_line
		j = 0
		while (nodelist_line[j] != '\n'):
			j+=1

		ndirname_file = open(nodelist_line[0:j], 'r')
		j = 0
		ndirname_line = ndirname_file.readline()
#		print "before while, ndirname_line=%s\n" % ndirname_line
		while (ndirname_line):
			ndirname_line = ndirname_file.readline()
#			print "in while, ndirname_line=%s\n" % ndirname_line
			j+=1
		print "ndirname_line, j=%d\n" % j
		if (j == 0):
			skip_backend = 1
#			print "skip_backend=%d\n" % skip_backend

		if (skip_backend == 0):
			k = j // 2
	        	LDPCL("mpirun -np "+str(k)+" *")
    		else:
	        	DPCL()
    else:
        print "No Job ID defined"

def PDPCL(sid):
    print "Starting DPCL daemons for SLURM job ",sid
    LDPCL("srun -c 8 --jobid "+str(sid)+" *")


if (os.environ.has_key("SLURM_JOBID")):
   SDPCL()
elif (os.environ.has_key("PBS_JOBID")):
   SDPCL()
else:
   DPCL()



