import code
import re
import string
import sys
import types

import PY_Input

################################################################################
#
# cmd_parse
#
# Dummy function that I will eventually use to call 
# parser.
#
################################################################################
def cmd_parse(args):

    #print args

    # Convert numeric values into strings
    count = len(args)
    for ndx in range(count):
	if type(args[ndx]) is types.IntType:
	    args[ndx] = str(args[ndx])
	if type(args[ndx]) is types.LongType:
	    args[ndx] = str(args[ndx])

    #print args

    # combine all the argument strings into a
    # single space delimitted string to pass
    # down to the yacc parser.
    if count > 0:
    	blank_delim = " "
    	zusamen = blank_delim.join(args[:count])
	#print zusamen
    	PY_Input.CallParser (zusamen)
    pass

################################################################################
#
# cloak_list_range
#
# For options that can have lists and ranges
# we need to make sure that python doesn't
# throw out the commas and colons. 
#
################################################################################
def cloak_list_range(arg):

    #print 'in cloak_list(',arg,')'
    if type(arg) is types.StringType:
    	arg = '"' + arg + '"'

    return arg

################################################################################
#
# Do_quit
#
# Quit python 
#
################################################################################
def Do_quit(args):

    #print "Exit Python"
    raise SystemExit

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
def preParseArgs(line, command_dict, arg_dict):
    
    parts = line.split()
    count = len(parts)
    blank_delim = " "

    for ndx in range(count):
        # Find the function for this command in the command dictionary
        function = command_dict.get(parts[ndx])

        if function is not None:
            func_ndx = ndx
            for i in range(ndx+1,count):

                # Yes, I know the following is stupid and should
                # be done in one or 2 elegant python statements
                # that for the life of me I can't figure out what
                # are so until it is pointed out to me what the 
                # good and wholesome solution is, we will have to
                # live with this.
                t_arg = arg_dict.get(parts[i])
                if t_arg is not None:
                    #print t_arg
                    parts[i] = '"' + parts[i] + '"'
                else:
                    t_str = parts[i]
                    if len(t_str) is 2:
                        if t_str[0] is '-':
                            # This could be done more effeciently
                            for t_char in ['r','h','f','p','t','x']:
                                if t_char is t_str[1]:
                                    parts[i] = '"' + parts[i] + '"'
				    parts[i+1] = cloak_list_range(parts[i+1])
                                    break
                            
                i = i+1

            # line = makePythonCall("myparse." + function, parts[func_ndx+1:])
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
#                   CLASS CLI
#
#
##################################################################
class CLI(code.InteractiveConsole):
    """Simple test of a Python interpreter augmented with custom commands."""

    ##################################################################
    #
    # commands
    #
    # The command:function dictionary for O/SS.
    #
    ##################################################################
    commands = { \
        "expAttach"     : "cmd_parse",
        "expClose"      : "cmd_parse",
        "expCont"       : "cmd_parse",
        "expCreate"     : "cmd_parse",
        "expDetach"     : "cmd_parse",
        "expFocus"      : "cmd_parse",
        "expPause"      : "cmd_parse",
        "expRestore"    : "cmd_parse",
        "expRun"        : "cmd_parse",
        "expSave"       : "cmd_parse",
        "expSetParam"   : "cmd_parse",
        "expStop"       : "cmd_parse",
        "expView"       : "cmd_parse",
        "listExp"       : "cmd_parse",
        "listHosts"     : "cmd_parse",
        "listObj"       : "cmd_parse",
        "listPids"      : "cmd_parse",
        "listSrc"       : "cmd_parse",
        "listMetrics"   : "cmd_parse",
        "listParams"    : "cmd_parse",
        "listReports"   : "cmd_parse",
        "listBreaks"    : "cmd_parse",
        "clearBreak"    : "cmd_parse",
        "exit"          : "Do_quit",
        "openGui"       : "cmd_parse",
        "help"          : "cmd_parse",
        "history"       : "cmd_parse",
        "log"           : "cmd_parse",
        "playback"      : "cmd_parse",
        "record"        : "cmd_parse",
        "setbreak"      : "cmd_parse",
        "quit"          : "Do_quit" \
        }
    
    ##################################################################
    #
    # o_ss_reserver
    #
    # The reserved word dictionary for O/SS.
    #
    ##################################################################
    o_ss_reserved = { \
        "vTopN"         : "viewtype:top_n",
        "vExclTime"     : "viewtype:exclusive_time",
        "vIo"           : "viewtype:io_time",
        "vfPe"          : "viewtype:floatingpoint_exceptions",
        "vHwc"          : "viewtype:hardware_counters",
        "pcsamp"        : "exp_type:pc_sampling",
        "usertime"      : "exp_type:user_time",
        "mpi"           : "exp_type:mpi",
        "fpe"           : "exp_type:floating_point_exceptions",
        "hwc"           : "exp_type:hardware_counters",
        "io"            : "exp_type:input_output",
        "displayMode"   : "exp_param_display_mode:",
        "displayMedia"  : "exp_param_display_media:",
        "displayRefreshRate"    : "exp_param_display_refresh_rate:",
        "expSaveFile"   : "exp_param_:experiment_save_file",
        "samplingRate"  : "exp_param_sampling_rate:" \
        }

    ##################################################################
    #
    # o_ss_subopts
    #
    # The single letter options for O/SS.
    #
    ##################################################################
    o_ss_subopts = { \
        "r"             : "suboption:rank",
        "h"             : "suboption:host_list",
        "f"             : "suboption:file_list",
        "p"             : "suboption:pid_list",
        "t"             : "suboption:thread_list",
        "l"             : "suboption:line_number",
        "x"             : "suboption:experiment_id", \
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
    def interact(self):

        # Set the primary and secondary prompts
        sys.ps1 = ">>> "
        sys.ps2 = "... "
        
        # Input Loop
        is_more = 0

        while 1:
            try :
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

                # TODO: add logging of input line here...
                
                # Process complete lines
                if 1:
                   # line = "myparse." + self.process(line)
                   line = self.process(line)

                # Push incomplete lines onto input stack
                if line or is_more:
                    is_more = self.push(line)

            # Handle CTRL-C
            except KeyboardInterrupt:                
                self.write("\nKeyboardInterrupt\n")
                is_more = 0
                self.resetbuffer()

            # Handle CTRL-D
            except EOFError:
                #print "eof or error", line
                self.write("\n")
                is_more = 0
                self.resetbuffer()
                return
                raise SystemExit


    ##################################################################
    #
    # Do_Parse
    #
    # This function will allow Python to do it's own reading of
    # input from stdin.
    #
    ##################################################################
    def do_input(self):

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

        t_line = preParseArgs(temp_line,self.commands,self.o_ss_reserved)
        
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

