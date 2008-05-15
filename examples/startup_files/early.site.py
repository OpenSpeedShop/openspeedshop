import code
import os
import os.path
import re
import string
import sys
import types
import tempfile

import PY_Input


def LLNLDPCL(template="*"):
#	print "LLNLDPCL template= ",template
	StartDPCLwithLibs(template,os.environ['PERS_OPENSS_LIBS'],os.environ['OPENSS_BASE'])

def DPCL():
#	print "Starting DPCL daemon on local node"
	LLNLDPCL("*")

def SDPCL():
	if (os.environ.has_key("SLURM_JOBID")):
		LLNLDPCL("srun -c 2 *")
	else:
		print "No Job ID defined"

def PDPCL(sid):
#	print "Starting DPCL daemons for SLURM job ",sid
	LLNLDPCL("srun -c 2 --jobid "+str(sid)+" *")

backend_nodes_present = 1
print_caution = 1
ndirname = tempfile.mktemp()
if (os.environ.has_key("SLURM_JOBID")):
	os.system('echo $SLURM_JOBID > ndirname1')
	nodelist_input_file1 = open('ndirname1', 'r')
	nodelist_line1 = nodelist_input_file1.readline()
os.system('uname -n > ndirname2')
if not (os.environ.has_key("SLURM_JOBID")):
#	print "no backend nodes present "
	backend_nodes_present = 0
	nodelist_input_file2 = open('ndirname2', 'r')
	nodelist_line2 = nodelist_input_file2.readline()
else:
	backend_nodes_present = 1
#	print "backend_nodes_present= ",backend_nodes_present

#
#
if backend_nodes_present:
#	print "nodelist_line1= ",nodelist_line1
	PDPCL(int(nodelist_line1,0))
	os.system('rm -rf ndirname1')
else:
#	print "nodelist_line2= ",nodelist_line2
	DPCL()
	os.system('rm -rf ndirname2')
