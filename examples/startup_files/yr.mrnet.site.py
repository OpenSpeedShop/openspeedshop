##===========================================================================##
#  Copyright (C) 2008 Los Alamos National Security, LLC. All Rights Reserved. #
#               Author: Samuel K. Gutierrez - samuel[at]lanl.gov              #
##===========================================================================##

#=============================================================================#
# This program is free software; you can redistribute it and/or modify it     #
# under the terms of the GNU General Public License as published by the Free  #
# Software Foundation; either version 2 of the License, or (at your option)   #
# any later version.                                                          #
#                                                                             #
# This program is distributed in the hope that it will be useful, but WITHOUT #
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       #
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for   #
# more details.                                                               #
#                                                                             #
# You should have received a copy of the GNU General Public License along     #
# with this program; if not, write to the Free Software Foundation, Inc., 59  #
# Temple Place, Suite 330, Boston, MA  02111-1307 USA                         #
#=============================================================================#

#Yellowrail MRNet site.py

import commands
from re import compile
from sets import Set
from sys import stdout
from os import path

# Flag indicating if debugging output will be displayed.
debug = True

def getRawAllocatedNodesString():
    if os.environ.has_key('PBS_JOBID'):
        #Get basic qstat information and force output of allocated nodes to be
        #on the same line as the job ID.
        qstatnodeinfo = commands.getoutput('qstat -f $PBS_JOBID -n1')
        if debug:
            print ' '
            #print qstatnodeinfo
        #Last item in list should always be a string containing a list of 
        #allocated nodes in the form: 'yra051/7+yra051/6+yra051...
        #yra051/0+yra087/7+yra087...yra087/0
    else:
        raise RuntimeError('fatal error...PBS_JOBID not defined.')
    return qstatnodeinfo.split(' ')[-1]

def getAllocatedNodesString():
    regex = re.compile('yra[^/*]*')
    dupnodeslist = regex.findall(getRawAllocatedNodesString())
    
    #Return string without duplicate node entries.
    nodelist = list(Set(dupnodeslist))
    
    #Append .lanl.gov
    """
	for i in range(len(nodeslist)):
        nodeslist[i] += '.lanl.gov'
    """
    nodelist.sort()
    return string.join(nodelist)

def prepENV():
    if debug:
        sys.stdout.write('Checking for OPENSS_MRNET_TOPOLOGY_FILE...')
    if os.environ.has_key('OPENSS_MRNET_TOPOLOGY_FILE'):
        if debug:
            print 'defined'
        
        #Backup User-defined OPENSS_MRNET_TOPOLOGY_FILE environment variable.
        #This is probably not needed, but I'll keep it around for now.
        backuptopenv = os.environ['OPENSS_MRNET_TOPOLOGY_FILE']
        
        #Get topology file path for redefinition of OPENSS_MRNET_TOPOLOGY_FILE.
        #If OPENSS_MRNET_TOPOLOGY_FILE is defined, assume that user would like
        #all OpenSpeedShop-related topology files created in the same directory.
        usertoproot = os.path.dirname(backuptopenv)

        #Overwrite OPENSS_MRNET_TOPOLOGY_FILE with:
        #/user/defined/top/path/HOSTNAME.<openssPID>.top
        newtopenv = usertoproot + os.sep + os.uname()[1].split('.')[0] + \
        '.%d.top' % (os.getpid())
        os.environ['OPENSS_MRNET_TOPOLOGY_FILE'] = newtopenv
        
        if debug:
            print 'Backup OPENSS_MRNET_TOPOLOGY_FILE: ' + backuptopenv
            print ('Changing OPENSS_MRNET_TOPOLOGY_FILE: ' +
                    os.environ['OPENSS_MRNET_TOPOLOGY_FILE'])
    else:
        if debug:
            print 'not defined'
        #If OPENSS_MRNET_TOPOLOGY_FILE is not defined, 
        #then default to $HOME/<openssPID>.top
        os.environ['OPENSS_MRNET_TOPOLOGY_FILE'] = \
            (os.environ['HOME'] + os.sep + os.uname()[1].split('.')[0] + \
            '.%d.top') % (os.getpid())
        if debug:
            print 'Defining OPENSS_MRNET_TOPOLOGY_FILE: ' + \
                    os.environ['OPENSS_MRNET_TOPOLOGY_FILE']

## haveTopgen()
# Returns True if mrnet_topgen is present in $OPENSS_PREFIX/bin.
# Returns False otherwise.
def haveTopgen():
    return os.path.isfile(os.environ['OPENSS_PREFIX'] + os.sep + 'bin' + \
                        os.sep + 'mrnet_topgen')

def generateMRNetTopologyFile(degree, numleaves):
    mrntstr = 'echo "' + getAllocatedNodesString() + '" | ' + \
				'mrnet_topgen -b %dx%d' % (degree, numleaves)
    
    #Capture generated MRNet topology string
    mrntopstr = commands.getoutput(mrntstr)
    
    if debug:
        print mrntopstr

    print mrntopstr
    
    #Make certain OPENSS_MRNET_TOPOLOGY_FILE is present
    if os.environ.has_key('OPENSS_MRNET_TOPOLOGY_FILE'):
        try:
            topfile = open(os.environ['OPENSS_MRNET_TOPOLOGY_FILE'], 'w')
            topfile.write(mrntopstr)
            topfile.close()
        except:
            exc_info = sys.exc_info()
            print exc_info[1]
            print ('an error was encountered during MRNet topology file ' + 
            'generation...')
            sys.exit()
    else:
        raise RuntimeError('OPENSS_MRNET_TOPOLOGY_FILE environment variable ' +
        'not defined...')

def createTopologyFile(topologyString):
    #Make certain OPENSS_MRNET_TOPOLOGY_FILE is present
    if os.environ.has_key('OPENSS_MRNET_TOPOLOGY_FILE'):
        try:
            topfile = open(os.environ['OPENSS_MRNET_TOPOLOGY_FILE'], 'w')
            topfile.write(topologyString)
            topfile.close()
        except:
            exc_info = sys.exc_info()
            print exc_info[1]
            print ('an error was encountered during MRNet topology file ' +
            'generation...')
            sys.exit()
    else:
        raise RuntimeError('OPENSS_MRNET_TOPOLOGY_FILE environment variable ' +
        'not defined...')
    

def getAllocatedNodeCount():
    rlnodeinfo = commands.getoutput('qstat -f $PBS_JOBID | ' + 
    'grep Resource_List.nodes')
    return int(rlnodeinfo.split(' ')[-1].split(':')[0])
     
def getAllocatedNodePPNCount():
    rlnodeinfo = commands.getoutput('qstat -f $PBS_JOBID | ' +
    'grep Resource_List.nodes')
    
    return int(rlnodeinfo.split(' ')[-1].split('=')[-1])

##generateSimpleTopologyString() 
def generateSimpleTopologyString():
    #Strip .lanl.gov
    hostname = os.uname()[1]
    hostname = hostname.split('.')[0]
    
    return hostname + ':0 => \n' + '  ' + hostname + ':1 ;'

#TODO:FIXME 
def generateSimpleBETopologyString():
    nodelist = getAllocatedNodesString().split(' ')
    
    topstring = nodelist[0] + ':0 =>\n  ' + nodelist[0] + ':1'

    for node in nodelist[1::1]:
        topstring += '\n  ' + node + ':0'
    
    topstring += ' ;'
    return topstring

##main()
def main():
    #Assuming presence of PBS_JOBID is a good 
    #indicator that we are on compute nodes
    if os.environ.has_key('PBS_JOBID'):
        if debug:
            print 'On Compute Node...'
        prepENV()
        print generateSimpleBETopologyString()
        #TODO:FIXME Simple hack to get things going
        createTopologyFile(generateSimpleBETopologyString()) 
    #If PBS_JOBID is not present, then we
    #better be on a compile node...
    else:
        if debug:
            print 'On Compile Node...'
        #Make sure we have mrnet_topgen before we continue. 
        #It's not used here, but will be.??
        prepENV()
        print generateSimpleTopologyString()
		#TODO: FIXME Simple hack to get things going
        createTopologyFile(generateSimpleTopologyString())

if __name__ == '__main__' :
     main()
