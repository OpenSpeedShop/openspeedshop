##***************************************************************************##
#*****************************************************************************#
#                Copyright (2008) Los Alamos National Laboratory              #
#                Author: Samuel K. Gutierrez - samuel<at>lanl.gov             #
#                                                                             #
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
# with this program; if not, write to the Free Software Foundation, Inc.,     #
# 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA                     #
#*****************************************************************************#
##***************************************************************************##

#Yellowrail MRNet site.py

import commands
import re
from sets import Set

debug = False

def getRawAllocatedNodesString():
    if os.environ.has_key('PBS_JOBID'):
        #Get basic qstat information and force output of allocated nodes to be
        #on the same line as the job ID.
        qstatnodeinfo = commands.getoutput('qstat -f $PBS_JOBID -n1')
        if debug:
            print qstatnodeinfo
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
    nodeslist = list(Set(dupnodeslist))
    #Append .lanl.gov
    """
    for i in range(len(nodeslist)):
        nodeslist[i] += '.lanl.gov'
    """
    nodeslist.sort()
    return string.join(nodeslist)

def generateMRNetTopologyString(degree, numleaves):
    mrntstr = ('echo "' + getAllocatedNodesString() + '" | ' 
    + 'mrnet_topgen -b %dx%d' % (degree, numleaves))
    
    #Capture generated MRNet topology string
    mrntopstr = commands.getoutput(mrntstr)
    
    if debug:
        print mrntopstr

    print mrntopstr
    
    #Make certain OPENSS_MRNET_TOPOLOGY_FILE is defined
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

def getAllocatedNodeCount():
    rlnodeinfo = commands.getoutput('qstat -f $PBS_JOBID | ' + 
    'grep Resource_List.nodes')
    return int(rlnodeinfo.split(' ')[-1].split(':')[0])
     
def getAllocatedNodePPNCount():
    rlnodeinfo = commands.getoutput('qstat -f $PBS_JOBID | ' +
    'grep Resource_List.nodes')
    return int(rlnodeinfo.split(' ')[-1].split('=')[-1])
        
def main():
    #FIXME
    generateMRNetTopologyString(getAllocatedNodeCount() - 1, 
    getAllocatedNodeCount() - 1)
if __name__ == '__main__' :
     main()
