################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
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
import re
import string
import sys
import types

import PY_Input
global terminate_SS
global cmd_is_assignment

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

    #print "args: ",args
    # Replace pseudo quotes with the real thing
    args = pseudo_quote_check(args)

    # combine all the argument strings into a
    # single space delimitted string to pass
    # down to the yacc parser.
    if count > 0:
        blank_delim = " "
        zusamen = blank_delim.join(args[:count])
        #print "zusamen=" , zusamen
        #print "cmd_is_assignment", cmd_is_assignment
        if myparse.cmd_is_assignment is not 0:
            PY_Input.SetAssign (1)
        return PY_Input.CallParser (zusamen)

    pass

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

    #print 'in cloak_list(',arg,')'
    arg = parts[i]

    # Check for prequoated symbols.
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
	    #print arg_len
	    j = 1
	    while j < arg_len:
	    	c = arg[j]
		j=j+1

	    	if c == ',' or c == ':':
		    t_arg = t_arg + ',' + '"' + c + '"' + ',' 
		else:
		    t_arg = t_arg + c

    	    parts[i] = t_arg

    #print arg
    #return arg
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

    #print "first", " ",parts, " ",i," ",count
    i_start = i
    while i < count:
    	arg  = parts[i]
    	arg_len = len(arg)
	#print arg_len," ",arg," ",[arg_len-1]
    	if arg[arg_len-1] ==    '"':
	    # check if single part 
	    if i_start == i:
	    	return (parts,i+1,count)
	    else:
    	    	t_arg = blank_delim.join(parts[i_start:i+1])
		t_arg = t_arg[1:len(t_arg)-1]

		#print "t_arg: ", t_arg
		
	    	if i_start == 0:
	    	    t_parts[0] = t_arg
		else:
		    t_parts_2 = [" "," "]
		    t_parts_2[0] = t_arg
	    	    t_parts = parts[0:i_start] +  t_parts_2[0:1]

    	    	if i+1 < count:
		    t_parts = t_parts+parts[i+1:count]
		count = count - (i-i_start)

    	    	#print "second", " ",t_parts, " ",i_start+1," ",count
    	    	t_parts[i_start] = '"-z' + t_parts[i_start] + '-z"'

		return (t_parts,i_start+1,count)

	i = i+1

    #print "last", " ",parts, " ",i_start+1," ",count
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

    myparse.terminate_SS = 1

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
    
    parts = line.split()
    count = len(parts)
    blank_delim = " "
    global_dict = globals()
    myparse.cmd_is_assignment = 0
    python_needs_result = 0

    for ndx in range(count):

        if ndx is 0 and count > 2 and parts[1] == '=':
            # Don't process target of assignment
            myparse.cmd_is_assignment = 1
            python_needs_result = 1
            continue

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
	    while i < count:

                # Yes, I know the following is stupid and should
                # be done in one or 2 elegant python statements
                # that for the life of me I can't figure out what
                # are so until it is pointed out to me what the 
                # good and wholesome solution is, we will have to
                # live with this.
	      t_part = string.lower(parts[i])

	      #t_arg = arg_dict.get(t_part)
	      if t_part[0] == '"':
	      	(parts,i,count) = match_quotes(parts,i,count)
	      else:
                #if t_arg is not None:
		#    parts[i] = t_part
		#    (parts,i) = quote_arg(parts,i)

		#else:
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

    ################################################################################
    #
    # write
    #
    # Replacement for standard python code:write function 
    #
    ################################################################################
    def showtraceback(self):

	arg = PY_Input.ParseError()

    ################################################################################
    #
    # Do_quit
    #
    # Quit python 
    #
    ################################################################################
    def Do_quit(args):

    	myparse.terminate_SS = 1

    ##################################################################
    #
    # commands
    #
    # The command:function dictionary for O/SS.
    #
    ##################################################################
    commands = { \
        "expattach"     : "return_none",
        "expclose"      : "return_none",
        "expcreate"     : "return_int",
        "expdetach"     : "return_none",
        "expdisable"    : "return_none",
        "expenable"     : "return_none",
        "expfocus"      : "return_int",
        "expgo"         : "return_none",
        "exppause"      : "return_none",
        "exprestore"    : "return_int",
        "expsave"       : "return_none",
        "expsetparam"   : "return_none",
        "expstatus" 	: "return_list",
        "expview"       : "return_list",

        "list"  	: "return_list",
        "listexp"       : "return_list",
        "listhosts"     : "return_list",
        "listmetrics"   : "return_list",
        "listobj"       : "return_list",
        "listparams"    : "return_list",
        "listpids"      : "return_list",
        "listranks" 	: "return_list",
        "listsrc"       : "return_string",
        "liststatus"	: "return_list",
        "listthreads" 	: "return_list",
        "listtypes" 	: "return_list",
        "listviews" 	: "return_list",

        "clearbreak"    : "return_none",
        "exit"          : "return_none",
        "opengui"       : "return_none",
        "help"          : "return_list",
        "history"       : "return_list",
        "log"           : "return_none",
        "playback"      : "return_none",
        "record"        : "return_none",
        "setbreak"      : "return_int",
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
        "-c"             : "suboption:cluster_list",
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
        self.regexp = re.compile( '[a-zA-Z_]*')
        self.sub_regexp = re.compile('rhfptx')
        
    ##################################################################
    #
    # interact
    #
    # This will read and process input lines from within
    # my main application as though on a python commandline.
    #
    ##################################################################
    def interact(self,scripting):
        myparse.terminate_SS = 0
        nesting_depth = 0

        # Set the primary and secondary prompts
        sys.ps1 = ">>> "
        sys.ps2 = "... "
        
        # Input Loop
        is_more = 0

        while 1:
            try :
                if myparse.terminate_SS:
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
    	    	if scripting == 1:
                    if check_compound_stmt(line, self.python_compound_stmts):
    	    	    	nesting_depth = nesting_depth + 1
                    elif is_more and not line.isspace():
    	    	    	lb = leading_blanks(line)
    	    	    	d_line = Delay_ILO_Processing(lb)
    	    	    	line = d_line
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
			# count = len(d_line)
			# print d_line," ",count
                        if myparse.cmd_is_assignment is not 0:
                            PY_Input.SetAssign (1)
		    	PY_Input.CallParser(d_line)

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

        self.interact(0)
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

        self.interact(1)
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

