################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2006-2009 Krell Institute  All Rights Reserved.
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
import shutil
import gc
#import gobject

import PY_Input



################################################################################
#
# pseudo_quote_check
#
# Python consumes quotes so we need to make pseudo 
# quotes earlier if we want the quotes to be passed
# on to lex/yacc. This routine finds and reconverts 
# them to real quotes.
#
################################################################################
def pseudo_quote_check(args):

    count = len(args)
    for ndx in range(count):
    	arg = args[ndx]
    	arg_len = len(args[ndx])
	
	if arg_len > 3:
	    if (arg[0:2] == "-z") and (arg[arg_len-2:arg_len] == "-z"):
	    	args[ndx] = '"' + arg[2:arg_len-2] + '"'

    return args

################################################################################
#
# cmd_parse
#
# Dummy function that I will eventually use to call 
# parser.
#
################################################################################
def cmd_parse(args):

    # Convert numeric values into strings
    count = len(args)
    for ndx in range(count):
	if type(args[ndx]) is types.IntType:
	    args[ndx] = str(args[ndx])
	if type(args[ndx]) is types.LongType:
	    args[ndx] = str(args[ndx])

    # Replace pseudo quotes with the real thing
    args = pseudo_quote_check(args)

    # combine all the argument strings into a
    # single space delimitted string to pass
    # down to the yacc parser.
    if count > 0:
        blank_delim = " "
        zusamen = blank_delim.join(args[:count])

        if myparse.cmd_is_assignment is not 0:
            PY_Input.SetAssign (1)
        return PY_Input.CallParser (zusamen)

    pass

################################################################################
#
# cmd_parse
#
# Find all parts that either end or begin with
# a colon (range) or comma (list) and join that
# part with part next to the character in question. 
# 
# This is for cases like:
#   -p 111 ,222, 333 : 444 , 555
# We want them to become:
#   -p 111,222,333:444,555
#
# I am going to assume that parts[i] represents the first
# element we care about and parts[count-1] the last. Thus
# we don't have to check the beginning of parts[i] and the
# ending of parts[count-1].
#
################################################################################
def compact_list_range(parts,i,count):

    zero_delim = ""

    i_start = i
    check_back = 1

    while i < count:
    	# Is this the last list item?
    	if i == count-1:
	    check_back = 0
	else:
    	    # Lump following item with current item
	    #print i, count, parts
    	    post_arg  = parts[i+1]
    	    post_len = len(post_arg)

	# Current lump
    	cur_arg  = parts[i]
    	cur_len = len(cur_arg)
	do_merge = 0

	if check_back == 1:
	    # comma check
    	    if cur_arg[cur_len-1] == ',' or post_arg[0] == ',':
	    	do_merge = 1
	    # colon check
    	    elif cur_arg[cur_len-1] == ':' or post_arg[0] == ':':
	    	# Merge this and following node
	    	do_merge = 1

	    if do_merge == 1:
	    	# Merge this and following node
		t_arg = parts[i]+parts[i+1]
		t_parts = parts[0:i+1]
		t_parts[i] = t_arg

		if i+2 < count:
    	    	    parts = t_parts+parts[i+2:count]
	    	else:
		    parts = t_parts
		
		# Merging the two elements reduces the size
		# of the python array we are processing.
		count = count-1
 
    	if do_merge == 0:
    	    i = i+1
	
    return (parts,i_start,count)

################################################################################
#
# cloak_list_range
#
# For options that can have lists and ranges
# we need to make sure that python doesn't
# throw out the commas and colons. 
#
# Changes current argument pointer.
#
################################################################################
def cloak_list_range(parts,i,count,is_name):

    arg = parts[i]

    # Check for prequoted symbols.
    if arg[0] == '"':
    	(parts,i,count) = match_quotes(parts,i,count)
    	return (parts,i,count)
	
    if type(arg) is types.StringType:
    	# Check to see if operand is name and cloak everything.
    	if is_name is 1:
    	    parts[i] = '"' + arg + '"'
	# Must be numerical. Only cloak delimeters.
	else:
	    # The assumption here is that the first character is always kosher
	    t_arg = arg[0]
	    arg_len = len(arg)
	    j = 1
	    while j < arg_len:
	    	c = arg[j]
		j=j+1

	    	if c == ',' or c == ':':
		    t_arg = t_arg + ',' + '"' + c + '"' + ',' 
		else:
		    t_arg = t_arg + c

    	    parts[i] = t_arg

    return (parts,i+1,count)

################################################################################
#
# match_quotes
#
# Find range of parts that are bounded by quotes
# and consolodate into one part bounded by quotes
# so it can get past python processing and passed
# on whole to lex/yacc. 
#
# expcreate -f "bosco 24u394 fasf"
#
# Changes argument list and current argument index.
#
################################################################################
def match_quotes(parts,i,count):

    blank_delim = " "

    i_start = i
    while i < count:
    	arg  = parts[i]
    	arg_len = len(arg)
    	if arg[arg_len-1] ==    '"':
	    # check if single part 
	    if i_start == i:
	    	return (parts,i+1,count)
	    else:
    	    	t_arg = blank_delim.join(parts[i_start:i+1])
		t_arg = t_arg[1:len(t_arg)-1]

	    	if i_start == 0:
	    	    t_parts[0] = t_arg
		else:
		    t_parts_2 = [" "," "]
		    t_parts_2[0] = t_arg
	    	    t_parts = parts[0:i_start] +  t_parts_2[0:1]

    	    	if i+1 < count:
		    t_parts = t_parts+parts[i+1:count]
		count = count - (i-i_start)

    	    	t_parts[i_start] = '"-z' + t_parts[i_start] + '-z"'

		return (t_parts,i_start+1,count)

	i = i+1

    return (parts,i_start+1,count)

################################################################################
#
# quote_arg
# 
# Surround a list element with quotes to prevent python
# from evaluating it as a symbol, operator or value.
#
# Increments current argument index
#
################################################################################
def quote_arg(args,i):
    args[i] = '"' + args[i] + '"'

    return (args,i+1)

################################################################################
#
# dummy fronts for parser call so we can return information
# however made up until we get the real command object running.
#
################################################################################
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
# Do_quit
#
# Quit python 
#
################################################################################
def Do_quit(args):

    PY_Input.Set_Terminate_PY()
    

##################################################################
#
# check_compound_stmt
#
# Determine if this statement is the start of a Python compound
# statement.
#
##################################################################
def check_compound_stmt (line, compound_dict):
    
    parts = line.split()
    count = len(parts)

    if count > 0:
        # Look to see if this keyword is the start of a compound statement
	t_part = compound_dict.get(parts[0])

        if t_part is not None:
	    return 1
        else:
            return 0
    else:
        return 0

##################################################################
#
# leading_blanks
#
# Determine how many leading blanks there are on the statement.
#
##################################################################
def leading_blanks (line):
    
    parts = line.split()
    count = len(parts)

    if count > 0:
        return line.rindex(parts[0])
    else:
        return 0

##################################################################
#
# Process_ILO
#
# Get the actual input line and execute it immediately.
# We need to strip off leading blanks before executing it.
#
##################################################################
def Process_ILO (arg):
    d_line = PY_Input.ReadILO(arg)
    p_line = d_line.strip()
    r_line = myparse.process(p_line)
    myparse.runsource(r_line, "stderr")
    return

##################################################################
#
# no_python_command_processing
#
# There is no python simulation, but we still want
# to lowercase the OSS command to be user friendly.
#
##################################################################
def no_python_command_processing(line):

    blank_delim = " "
    parts = line.split()
    t_part = string.lower(parts[0])
    parts[0] = t_part
    new_line = blank_delim.join(parts)

    return new_line

##################################################################
#
# assignment_splitting
#
# We need to know if this is an assignment command
# and if so the parts should be separate from each other.
#
# it would be cool if we only had to search for "="
# in the input line, but there can be assignments within
# openss commands them selves that we don't want to muck
# with.
#
# This code is way too complicated for what we are trying
# to accomplish and needs to be rewritten.
#
##################################################################
def assignment_splitting(line):

    myparse.cmd_is_assignment = 0

    blank_delim = " "
    parts = line.split()
    count = len(parts)
    python_needs_result = 0
    
    n = string.count(line,'=')
    if n == None:
    	return line
    
    if count is 0:
    	return line

    # Check for <result> = <command>
    if count > 2 and parts[1] == '=':
            # Don't process target of assignment
            myparse.cmd_is_assignment = 1
            python_needs_result = 1

    	    #print "First: ",parts
    	    new_line = blank_delim.join(parts)
    	    #print "First #2: ",new_line
    	    return new_line

    # Check for <result>= <command>
    str_len = len(parts[0])
    if parts[0][str_len-1] == '=':
    	t_parts = parts[0].split("=")
    	tt_parts = [t_parts[0]]
   	tt_parts.append('=')
	if count > 1:
    	    tt_parts = tt_parts + parts[1:]
    	parts = tt_parts
    	myparse.cmd_is_assignment = 1
    	python_needs_result = 1

    	#print "Second #4: ",parts
    	new_line = blank_delim.join(parts)
    	return new_line

    # Check for <result> =<command>
    if count > 1 and parts[1][0] is "=":
	    t_parts = parts[1].split("=")
	    t_len = len(t_parts)
	    if t_len > 1:
	    	tt_parts = [parts[0]]
	    	tt_parts.append('=')
		tt_parts.append(t_parts[t_len-1])
		if count > 2:
		    tt_parts = tt_parts + parts[2:]

		parts = tt_parts
		myparse.cmd_is_assignment = 1
		python_needs_result = 1

		#print "Third: ",parts
    	    	new_line = blank_delim.join(parts)
    	    	return new_line

    # Check for <result>=<command>
    else:
	    t_parts = parts[0].split("=")
	    t_len = len(t_parts)
	    if t_len > 1:
	    	tt_parts = [t_parts[0]]
		
		tt_parts.append('=')
		tt_parts.append(t_parts[1])

    	    	if count > 1:
		    tt_parts = tt_parts + parts[1:]
		parts = tt_parts
		myparse.cmd_is_assignment = 1
		python_needs_result = 1

		#print "Fourth: ",parts
    	    	new_line = blank_delim.join(parts)
    	    	return new_line

    return line

##################################################################
#
# preParseArgs
#
# Determine if there is an O/SS command in the line and
# if so, repackage the command as a python function call.
#
# If any of the arguments are O/SS reserved symbols I cloak
# them with quotes to prevent python from choking on them. We
# want the user to use the python name space for name resolution
# for their variables. This leads to the question of what am I
# to do if we have quoted strings a legal O/SS command arguments.
# For now I will assume that we won't allow that and I can just
# uncloak all quoted strings later down the pipe.
#
##################################################################
def preParseArgs(line, command_dict, str_opts_dict, num_opts_dict):
    
    line = assignment_splitting(line)

    parts = line.split()
    count = len(parts)
    blank_delim = " "
    global_dict = globals()

    for ndx in range(count):

	    
        # Find the function for this command in the command dictionary
    	function = None
	t_part = string.lower(parts[ndx])
        x = global_dict.get(string.lower(parts[ndx]))
        if x is not None:
    	    if type(x) is types.StringType:
    	    	t_part = string.lower(x)
    	    	parts[ndx] = t_part

    	function = command_dict.get(t_part)

        if function is not None:
            # Yes!  This is an OSS command!
            parts[ndx] = t_part
            func_ndx = ndx

	    i = ndx+1
	    
	    #Get rid of spaces within lists and ranges
	    #print parts, i, count
	    parts,i,count = compact_list_range(parts,i,count)
	    #print parts, i, count
	    while i < count:

                # Yes, I know the following is stupid and should
                # be done in one or 2 elegant python statements
                # that for the life of me I can't figure out what
                # are so until it is pointed out to me what the 
                # good and wholesome solution is, we will have to
                # live with this.
	      t_part = string.lower(parts[i])

	      if t_part[0] == '"':
	      	(parts,i,count) = match_quotes(parts,i,count)
	      else:
 		    # Look for HELP arguments for commands
                    t_arg = command_dict.get(t_part)
		    if t_arg is not None:
		    	parts[i] = t_part
			(parts,i) = quote_arg(parts,i)
                    else:
		    	# Look for dash arguments
			t_arg = str_opts_dict.get(parts[i])
			if t_arg is not None:
			    (parts,i) = quote_arg(parts,i)
    	    	    	    # Don't crash if no argument
    	    	    	    if i < count-1:
			    	# cloak sensitive characters and strings
				(parts,i,count) = cloak_list_range(parts,i,count,1)

			else:
			    t_arg = num_opts_dict.get(parts[i])
			    if t_arg is not None:
				(parts,i) = quote_arg(parts,i)
    	    	    	    	# Don't crash if no argument
    	    	    	    	if i < count-1:
			    	    # cloak sensitive characters and strings
				    (parts,i,count) = cloak_list_range(parts,i,count,0)
			    else:
			    	# oh well, just cloak it for now
				# this is because of experiment type lists
				# don't have an associated dash option
    	    	    	    	(parts,i) = quote_arg(parts,i)		    	

            parts[func_ndx] = '"' + parts[func_ndx] + '"'
            line = makePythonCall( function, parts[func_ndx:])

            # Check for leading assignment words
            if func_ndx is not 0:
                leading = blank_delim.join(parts[:func_ndx])
                line = leading + blank_delim + line

    	    #print line
            return line
            
        ndx = ndx+1

    return line

##################################################################
#
# parseArgs
#
#
##################################################################
def parseArgs(args):
        return args.split()

##################################################################
#
# makePythonCall
#
#
##################################################################
def makePythonCall(func, args):
    return "%s([%s])" % (func, ", ".join(args))

##################################################################
#
# Delay_ILO_Processing
#
# Create a call to "Process_ILO" with a pointer to the ILO as
# an argument.  This will cause the original line to be fetched
# and executed when Python decides that it's time to execute the
# command.
#
# The call to "Process_ILO" is adjusted to look like it is a
# normal Python command in the scope of the nesting statement.
#
##################################################################
def Delay_ILO_Processing(pad):
    arg = PY_Input.Save_ILO()
    d_line = "%s(\"%s\")" % ("Process_ILO", arg)
    n_line = d_line.rjust(pad + len(d_line))
    return n_line


##################################################################
#
#                   CLASS CLI
#
#
##################################################################
class CLI(code.InteractiveConsole):
    """Simple test of a Python interpreter augmented with custom commands."""

    global cmd_is_assignment

    ################################################################################
    #
    # write
    #
    # Replacement for standard python code:write function 
    #
    ################################################################################
    def my_showtraceback(self):
    	code.InteractiveConsole.showtraceback()
	arg = PY_Input.ParseError()
	#print "Jack"

    ##################################################################
    #
    # commands
    #
    # The command:function dictionary for O/SS.
    #
    ##################################################################
    commands = { \
        "expattach"     : "return_none",
        "expclone"      : "return_int",
        "expclose"      : "return_none",
        "expcont"       : "return_none",
        "expcreate"     : "return_int",
        "expcompare"	: "return_list",
        "expdata"   	: "return_list",
        "expdetach"     : "return_none",
        "expdisable"    : "return_none",
        "expenable"     : "return_none",
        "expfocus"      : "return_int",
        "expgo"         : "return_none",
        "run"	    	: "return_none",
        "exppause"      : "return_none",
        "exprestore"    : "return_int",
        "expsave"       : "return_none",
        "expsetargs"    : "return_none",
        "expsetparam"   : "return_none",
        "expstatus" 	: "return_list",
        "expview"       : "return_list",
	
        "view"	    	: "return_list",
	
	"cviewcluster"	: "return_list",
	"cviewcreate"	: "return_int",
	"cviewdelete"	: "return_none",
	"cviewinfo"   	: "return_list",
	"cview"   	: "return_list",

        "list"  	: "return_list",

        "clearbreak"    : "return_none",
        "echo"          : "return_list",
        "exit"          : "return_none",
        "opengui"       : "return_none",
        "help"          : "return_list",
        "history"       : "return_list",
        "info"          : "return_list",
        "log"           : "return_none",
        "playback"      : "return_none",
        "record"        : "return_none",
        "setbreak"      : "return_int",
        "wait"	    	: "return_none",
        "quit"          : "Do_quit" \
        }
    
    ##################################################################
    #
    # o_ss_subopts
    #
    # The single letter string options for O/SS.
    #
    ##################################################################
    o_ss_str_subopts = { \
        "-h"             : "suboption:host_list",
        "-m"             : "suboption:metric_list",
        "-v"             : "suboption:modifier_list",
        "-c"             : "suboption:compare_view_list",
        "-f"             : "suboption:file_list", \
        }

    ##################################################################
    #
    # o_ss_subopts
    #
    # The single letter numerical options for O/SS.
    #
    ##################################################################
    o_ss_num_subopts = { \
        "-r"             : "suboption:rank",
        "-p"             : "suboption:pid_list",
        "-t"             : "suboption:thread_list",
        "-l"             : "suboption:line_number", \
        }

    ##################################################################
    #
    # python_compound_stmts
    #
    # These keywords imply delayed processing
    #
    # The design that supports delayed processing of statements inside
    # nested expressions will only work if all the Python statements
    # that can cause a scope change are detected.
    #
    ##################################################################
    python_compound_stmts = { \
        "if"             : 1,
        "while"          : 1,
        "for"            : 1,
        "try"            : 1,
        "def"            : 1,
        "elif"           : 1,
        "else"           : 1,
        "except"         : 1,\
        }

    ##################################################################
    #
    # __init__
    #
    #
    ##################################################################
    def __init__(self, locals = None):

        # Call super-class initializer
        code.InteractiveConsole.__init__(self, locals, "<console>")
        
        # Compile regular expression for finding commmands
        #self.regexp = re.compile( '[a-zA-Z_]*')
        #self.sub_regexp = re.compile('rhfptx')
        
    ##################################################################
    #
    # interact
    #
    # This will read and process input lines from within
    # my main application as though on a python commandline.
    #
    ##################################################################
    def interact(self):
	gc.disable();
        if os.environ.has_key("OPENSS_DEBUG_CLI_PYTHON_GC"):
            gc.set_debug(gc.DEBUG_LEAK)
            gc.get_objects()
        nesting_depth = 0

        # Set the primary and secondary prompts
        sys.ps1 = ">>> "
        sys.ps2 = "... "
        
        # Input Loop
        is_more = 0

        while 1:
            try :
		# Turn off garbage collection
		gc.disable();
                if os.environ.has_key("OPENSS_DEBUG_CLI_PYTHON_GC"):
                    gc.set_debug(gc.DEBUG_LEAK)
                    gc.get_objects()
	         
    	    	if PY_Input.Terminate_PY() == 1:
                  self.write("\n")
                  self.resetbuffer()
                  return
	         
                # Display the appropriate prompt
                if not sys.stdin.isatty():
                    prompt = ""
                elif is_more:
                    prompt = sys.ps2
                else:
                    prompt = sys.ps1

                # Read the next line of input
                line = PY_Input.ReadLine (is_more)

                if not line:
                   line = 'EOF'

    	    	# Scripting
    	    	if PY_Input.Do_Scripting() == 1:
                    if check_compound_stmt(line, self.python_compound_stmts):
    	    	    	nesting_depth = nesting_depth + 1
                    elif is_more and not line.isspace():
    	    	    	lb = leading_blanks(line)
                        if lb is not 0:
    	    	    	    d_line = Delay_ILO_Processing(lb)
    	    	    	    line = d_line
                        else:
                    	    # Insert a dummy line.  Why is this needed?
                    	    self.push("")
                    	    line = self.process(line)
                    else:
                    	line = self.process(line)

    	    	    # Push lines onto input stack for execution.
    	    	    # Python will decide when to execute the line
    	    	    # but we had better be correct about when the
                    # execution will be delayed.
    	    	    if line or is_more:
    	    	    	is_more = self.push(line)

    	    	# Non-Scripting
		else:
    	    	    count = len(line)
		    if count > 3:
    	    	    	arg = PY_Input.Save_ILO()
    	    	    	d_line = PY_Input.ReadILO(arg)
			#count = len(d_line)
			#print d_line," ",count
			d_line = no_python_command_processing(d_line)
			# This is a kluge to have limited comments.
			if d_line[0] != "#":
		    	    PY_Input.CallParser(d_line)

		# Turn on garbage collection
		# gc.enable();
                if os.environ.has_key("OPENSS_DEBUG_CLI_PYTHON_GC"):
                    gc.set_debug(gc.DEBUG_LEAK)
                    gc.get_objects()
	         
            # Handle CTRL-C
            except KeyboardInterrupt:                
                self.write("\nKeyboardInterrupt\n")
                is_more = 0
                self.resetbuffer()

            # Handle CTRL-D
            except EOFError:
                #print "eof or error", line
                self.write("\n")
                self.resetbuffer()
                return


    ##################################################################
    #
    # do_input
    #
    # This function will allow Python to do it's own reading of
    # input from stdin.
    #
    ##################################################################
    def do_flat_input(self):

        self.interact()
        pass

    ##################################################################
    #
    # do_input
    #
    # This function will allow Python to do it's own reading of
    # input from stdin.
    #
    ##################################################################
    def do_scripting_input(self):
	# gc.setdebug(gc.DEBUG_LEAK)
	# gc.get_objects()

        self.interact()
        pass


    ##################################################################
    #
    # process
    #
    # This will determine if the input command is either
    # from my application's command language or a python
    # construct.
    #
    ##################################################################
    def process(self, line):

        # Attempt to match line against our command regular expression

        temp_line = string.lstrip(line)
        len_1 = len(line)
        len_2 = len(temp_line)

        white_spaces = len_1-len_2
        if white_spaces:
            front_padding = line[0:white_spaces]

        t_line = preParseArgs(temp_line,
	    	    	      self.commands,
			      self.o_ss_str_subopts,
			      self.o_ss_num_subopts)
        
        if t_line is not temp_line:
            line = t_line

            # Prepend leading white space indentation
            if white_spaces:
                line = front_padding + t_line

        # Return the line to be processed by Python
        return line



##################################################################
#
##################################################################
myparse = CLI(globals())

##################################################################
#
# Stand_Alone
#
# This fucntion can be used to bypass the standard SpeedShop input
# mechanism and will allow Python to run as a stand-alone
# application that is able to execute SpeedShop commands.
#
##################################################################
def Stand_Alone():

        myparse.interact()
        pass



################################################################################
# DPCL Daemon Startup Function
################################################################################

def StartDPCLwithLibs(template="*", installed="/usr", libs="" ):
    """Start DPCL daemon(s) for Open|SpeedShop.

    Start the DPCL daemon(s) and attach it/them to this Open|SpeedShop session.
    Assembles a command for setting all the necessary environment variables and
    lauching the daemon(s). That command is then executed using system().

    The parameter 'installed' indicates the directory in which DPCL/Dyninst is
    installed and is used to configure the environment. The parameter 'template'
    specifies the format of the command to be used to start the daemon. This
    string should contain a single '*' symbol that is replaced with the actual
    command for starting the daemon. For example, if MPI is to be used to invoke
    the daemon on 128 nodes, the template might be 'mpirun -np 128 *' which
    would then be replaced with 'mpirun -np 128 env ... dpcld -p ...' before
    actual execution.
    """

    # Locate the DPCL dameon
    daemon = installed + "/bin/dpcld"
    if not os.path.isfile(daemon):
        raise RuntimeError("Failed to locate the DPCL Daemon.")

    if libs!="":
        basedir=libs + "/lib64"
        if not os.path.isdir(basedir):
            basedir = libs + "/lib"
        if not os.path.isdir(basedir):
            raise RuntimeError("Failed to locate the base library directory.")
        basedir2=":"+basedir
    else:
        basedir2=""

    # Locate the DPCL library directory
    libdir = installed + "/lib64"
    if not os.path.isdir(libdir):
        libdir = installed + "/lib"
    if not os.path.isdir(libdir):
        raise RuntimeError("Failed to locate the DPCL library directory.")

    # Form the command that will start the daemons
    if template.count("*") is not 1:
        raise RuntimeError("Template string must contain exactly one * symbol.")
    command = "env" + \
              " LD_LIBRARY_PATH=" + libdir + basedir2 +\
              " DYNINSTAPI_RT_LIB=" + libdir + "/libdyninstAPI_RT.so.1" \
              " DPCL_RT_LIB=" + libdir + "/libdpclRT.so.1" + \
              " " + daemon + " -p " + DpcldListenerPort
    command = string.replace(template, "*", command)
    command = command + " &"

    # Execute the command to start the daemon(s)
    print "\nStarting DPCL daemon(s) using the command \"%s\"...\n" % command
    os.system(command)

def StartDPCL(template="*", installed="/usr" ):
    StartDPCLwithLibs(template,installed,"")


################################################################################
# Load the site-specific Python startup file (if it exists)
################################################################################

site_specific_startup_file = OpenssInstallDir + "/lib/openspeedshop/site.py"
if os.path.isfile(site_specific_startup_file):
    execfile(site_specific_startup_file)

site_specific_startup_file = OpenssInstallDir + "/lib64/openspeedshop/site.py"
if os.path.isfile(site_specific_startup_file):
    execfile(site_specific_startup_file)

################################################################################
# Load the offline-specific Python startup file (if it exists)
################################################################################

site_specific_startup_file = OpenssInstallDir + "/lib/openspeedshop/offline.py"
if os.path.isfile(site_specific_startup_file):
    execfile(site_specific_startup_file)

site_specific_startup_file = OpenssInstallDir + "/lib64/openspeedshop/offline.py"
if os.path.isfile(site_specific_startup_file):
    execfile(site_specific_startup_file)

site_specific_startup_file = OpenssInstallDir + "/lib64/openspeedshop/cbtf-offline.py"
if os.path.isfile(site_specific_startup_file):
    execfile(site_specific_startup_file)

################################################################################
# Load the mrnet-specific Python startup file (if it exists)
################################################################################

# JEG commented this out on May 16 2015 - it causes osscompare to not
# work on compute node partitions because we are trying to read from 
# the nodes in the allocation.  We do not use this version (mrnet) any longer.

#site_specific_startup_file = OpenssInstallDir + "/lib/openspeedshop/mrnet.py"
#if os.path.isfile(site_specific_startup_file):
#    execfile(site_specific_startup_file)
#
#site_specific_startup_file = OpenssInstallDir + "/lib64/openspeedshop/mrnet.py"
#if os.path.isfile(site_specific_startup_file):
#    execfile(site_specific_startup_file)

################################################################################
# Load the user-specific Python startup file (if it exists)
################################################################################

if os.environ.has_key("HOME"):
    user_specific_startup_file = os.environ["HOME"] + "/.openss.py"
    if os.path.isfile(user_specific_startup_file):
        execfile(user_specific_startup_file)
