################################################################################
# Copyright (c) 2007 The Krell Institute. All Rights Reserved.
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 2.1 of the License, or (at your option)
# any later version.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library; if not, write to the Free Software Foundation, Inc.,
# 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
################################################################################

import code
import os
import os.path
import re
import string
import sys
import types
import tempfile

import PY_Input


def launchDPCL():

################################################################################
# Load the user-specific Python startup file (if it exists)
################################################################################

	print "Enter launchDPCL specific processing\n" 

# 	Initialization
	print_caution = 0
	dpcld_command = []
	inet_addr_line = []
	config_line = []
	inet_addr = '\0'

#	 We need to find the myrnet IP address to use for starting the 
#	 dpcl daemons on the flash64 BProc system
	ifTmpFile = tempfile.mktemp()
	os.system('ifconfig >> ifTmpFile')
	config_input = open('ifTmpFile', 'r')

	while 1:
		# Read in a line of the ifconfig output saved to the tempifconfig file
		config_line = config_input.readline()
		if not config_line:
			failure_reason = 1
			break
#
#		Tweak this line with the string you are looking for 
#
		if config_line[0:5] == 'myri0':
#
#			Now we have the line we are interested in, read the line with the IP address
#
			inet_addr_line = config_input.readline()
			if not config_line:
				failure_reason = 2
				break
#
#			Did we find the phrase prior to the IP address?
#
			if inet_addr_line[10:20] == 'inet addr:':
#
#				Yes we find the phrase prior to the IP address - now extract the IP address
#
				i = 20;
				success = 0
				k = 0
				while (inet_addr_line[i] != ' '):
#
#					Indicate success to know after this loop
#
					success = 1
					i = i + 1
					k = k + 1
#
#					We are going to use the end index to the IP addr as inet_addr_k later
#
					inet_addr_k = k
				if success:
					inet_addr = inet_addr_line[20:i]
					#print "inet_addr=%s\n" % inet_addr
					break
	os.system('rm -rf ifTmpFile')
#
#	print "DpcldListenerPort=%s\n" % DpcldListenerPort
#
#	 Extract the hostname from the listener port variable
#
#	j = 0
#	while (DpcldListenerPort[j] != ':' and DpcldListenerPort[j] != '\n'):
#		j+=1
#	hostname = DpcldListenerPort[0:j]
#	print "hostname from DpcldListenerPort=%s\n" % hostname
#	print "after hostanme while DpcldListenerPort[j:]=%s\n" % DpcldListenerPort[j:]

#
#	 We know the port number is the last five characters, so extract them
#
	listener_port = DpcldListenerPort[-5:]
#	print "portnumber from DpcldListenerPort=%s\n" % listener_port
#
# 	The dpcld command for the llogin (frontend) node is: dpcld -p host:port
#
	dpcld_command = 'dpcld -p ' + DpcldListenerPort
	#print "dpcld_command=%s\n" % dpcld_command
	os.system(dpcld_command + '&')

# 	Trying to form a statement of this type/form
#	 bpsh 62-63 dpcld -p 141.111.5.182:15002
#	 bpsh <nodelist> dpcld -p <myr01_inet_addr>:<listener_port>
#
#
#	 Echo the node list into a file so we can extract them
#
	skip_backend = 0
	backend_nodes_present = 1
        ndirname = tempfile.mktemp()
	os.system('echo $NODES > ndirname')
	nodelist_input_file = open('ndirname', 'r')
	nodelist_line = nodelist_input_file.readline()
	if not nodelist_line:
		backend_nodes_present = 0
#
#
	if backend_nodes_present:
#		print "nodelist_line=%s\n" % nodelist_line
		j = 0
		while (nodelist_line[j] != '\n'):
			j+=1
#		print "nodelist_line, j=%d\n" % j
		if (j == 0):
			skip_backend = 1
# 
#		Ok, we should have the node list in nodelist[0,j] now
#
		if (skip_backend == 0):
			#print "nodelist_line=%s\n" % nodelist_line[0:j]
#
#			Trying to form a statement of this type/form
#			bpsh 62-63 dpcld -p 141.111.5.182:15002
#			bpsh <nodelist> dpcld -p <myr01_inet_addr>:<listener_port>
#			we have the pieces now lets form it
#
#			Form the command using the extracted pieces.  
#			This starts the dpcl daemons on the backend nodes
#
			bpsh_command = 'bpsh ' + nodelist_line[0:j] + ' dpcld -p ' + inet_addr[0:inet_addr_k] + ':' + listener_port
#
			#print "bpsh_command=%s\n" % bpsh_command
#
#			Execute the bpsh command to start the daemons on the backend nodex
#
			os.system(bpsh_command + '&')
		else:
			if (print_caution == 1):
				print "Cautionary message: No backend nodes were found. So, no dpcl daemons for backend nodes were started.\n" 
	else:
		if (print_caution == 1):
			print "Cautionary message: No backend nodes were found. So, no dpcl daemons for backend nodes were started.\n" 
	os.system('rm -rf ndirname')
#
#
#print "Enter site specific processing (site.py)\n\n" 
launchDPCL()
#print "Exit site specific processing (site.py)\n" 

