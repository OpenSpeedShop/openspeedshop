################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2006-2012 Krell Institute All Rights Reserved.
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

from openss_classes import *
import types

from PYopenss import *

################################################################################
#
# is_single_type
#
# Determine if the argument is a single number
# or string (not a list or tuple).
#
################################################################################
def is_single_type(arg):

    	if type(arg) is types.IntType:
    	    return 1
    	elif type(arg) is types.LongType:
    	    return 1
    	elif type(arg) is types.StringType:
    	    return 1
    	else:
    	    return 0    

    	return 0

################################################################################
#
# deconstruct
#
# Convert a full Python API openss commandline into
# an OpenSpeedShop commandline format.
#
################################################################################
def deconstruct(cmd_name,*args):
    
    	count = len(args)
    	main_string = cmd_name
	
    	for ndx in range(count):
    	
    	    if isinstance(args[ndx],HostList):
	    	sublist = parse_list("-h",None,*args[ndx])
    	    elif isinstance(args[ndx],RankList):
	    	sublist = parse_list("-r",None,*args[ndx])
    	    elif isinstance(args[ndx],FileList):
	    	sublist = parse_list("-f","FileType",*args[ndx])
    	    elif isinstance(args[ndx],PidList):
	    	sublist = parse_list("-p",None,*args[ndx])
    	    elif isinstance(args[ndx],ThreadList):
	    	sublist = parse_list("-t",None,*args[ndx])
    	    elif isinstance(args[ndx],CVIdList):
	    	sublist = parse_list("-c",None,*args[ndx])
    	    elif isinstance(args[ndx],MetricList):
	    	sublist = parse_list("-m","MetricType",*args[ndx])
    	    elif isinstance(args[ndx],ExpTypeList):
	    	sublist = parse_list("",None,*args[ndx])
    	    elif isinstance(args[ndx],ViewTypeList):
	    	sublist = parse_list("",None,*args[ndx])
    	    elif isinstance(args[ndx],LineNoList):
	    	sublist = parse_list("-l",None,*args[ndx])
    	    elif isinstance(args[ndx],ModifierList):
	    	sublist = parse_list("-v",None,*args[ndx])
    	    elif isinstance(args[ndx],ParamList):
	    	sublist = parse_list("","ParamType",*args[ndx])
    	    elif isinstance(args[ndx],BreakList):
	    	sublist = parse_list("",None,*args[ndx])
    	    # single instance
    	    elif isinstance(args[ndx],ExpId):
	    	sublist = "-x "
	    	sublist += make_string(args[ndx],0)
    	    elif is_single_type(args[ndx]):
	    	sublist = make_string(args[ndx],0)
    	    else:
	    	sublist = parse_list("-j",None,*args[ndx])

	    main_string += " "+sublist
	
	return main_string

################################################################################
#
# parse_list
#
# This converts the python arguments for an option
# into a string that Matches the regular OpenSpeedShop
# grammar.
#
################################################################################
def parse_list(flag, is_special,*arglist):
    
    	if (len(arglist) == 1) and (type(arglist) is types.TupleType):
    	    arglist = arglist[0]

    	t_string = []

    	if is_single_type(arglist) == 1:
	    if is_special is "FileType":
    	    	t_string.append(make_string(arglist,1))
	    else:
    	    	t_string.append(make_string(arglist,0))
    	elif ((type(arglist) is types.ClassType) or
    	      (type(arglist) is types.ListType) or
	      (type(arglist) is types.TupleType)):
    	    count = len(arglist)

    	    for ndx in range(count):
	    	arg = arglist[ndx]
	    
    	    	if is_single_type(arg) == 1:
		    if is_special is "ParamType":
		    	raise SyntaxError,"param arguments are tuples of 2" 
		    elif is_special is "FileType":
    	    	    	t_string.append(make_string(arg,1))
		    else:
    	    	    	t_string.append(make_string(arg,0))
    	    	elif type(arg) is types.TupleType:
		    	if len(arg) != 2:
		    	    raise SyntaxError, "tuple is not of size 2"
		    	else:

    	    	    	    if is_single_type(arg[0]) == 1:
    	    	    	    	t1 = make_string(arg[0],0)
    	    	    	    else:
			    	raise SyntaxError,"first argument of tuple not simple type" 

    	    	    	    if is_single_type(arg[1]) == 1:
    	    	    	    	t2 = make_string(arg[1],0)
    	    	    	    else:
			    	raise SyntaxError,"second argument of tuple not simple type" 
			    
			    if is_special is "MetricType":
			    	t_string.append(t1+"::"+t2)
			    elif is_special is "ParamType":
			    	t_string.append(t1+"="+t2)
			    elif is_special is None:
			    	t_string.append(t1+":"+t2)
			    else:
		    	    	raise SyntaxError, "tuple is special, but not handled in parse_list()"
    	    	else:
	    	    	print type(arg)
	    	    	print type(arglist)
		    	raise SyntaxError, "argument not simple or tuple"
    	else:
    	    print type(arglist[0])
    	    print type(arglist)
    	    raise SyntaxError, "unexpected data type"

    	blank_delim = " "
    	comma_delim = ","
    	zusamen = comma_delim.join(t_string[:])

    	return flag+blank_delim+zusamen

################################################################################
#
# make_string
#
# It is expected that arg has passed is_single_type()
# before being sent here. Basically we want a string
# representation of the argument.
#
################################################################################
def make_string(arg, add_quotes):

    	if type(arg) is types.IntType:
    	    t_string = str(arg)
    	elif type(arg) is types.LongType:
    	    t_string = str(arg)
    	elif type(arg) is types.StringType:
    	    t_string = arg
    	elif isinstance(arg,ExpId):
    	    t_string = str(arg)
    	else:
    	    raise SyntaxError
	    
	if add_quotes == 1:
	    t_string = '"'+t_string+'"'

    	return t_string

################################################################
#
# dummy fronts for parser call so we can return information
# however made up until we get the real command object running.
#
################################################################
def return_none(args):
    	cmd_parse(args)
    	return

def return_int(args):
    	ret = cmd_parse(args)
    	return ret

def return_string(args):
    	ret = cmd_parse(args)
    	return ret

def return_int_list(args):
    	ret = cmd_parse(args)
    	return ret

def return_string_list(args):
    	ret = cmd_parse(args)
    	return ret

def return_list(args):
    	ret = cmd_parse(args)
    	return ret

################################################################################
#
# cmd_parse
#
# This is the call to the OpenSpeedShop dso.
# This is passing a commandline to be parsed
# and processed by the yacc parser and semantic 
# routines.
#
# This should have already been processed into
# a single string before this point.
#
################################################################################
def cmd_parse(zusamen):

    return EmbeddedParser(zusamen)

