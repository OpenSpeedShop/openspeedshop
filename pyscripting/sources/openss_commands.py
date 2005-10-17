from openss_classes import *
from openss_parse import *


#    """OpenSpeedShop scripting language."""

##################################################
# expAttach
##################################################
def expAttach(*arglist):

    """
    Attach applications or collectors to an experiment.
    	- The attached applications will not execute until 
	an B{expGo} command is issued.

    If I{ModifierList} object with B{mpi} is selected, all
    the threads that are part of a running application will
    be included in the experiment.

    If I{ModifierList} object with B{mpi} is not present,
    only those threads that are explicitly listed in
    I{Target} will be included in the experiment.

    If there is no I{ExpId} provided, information is 
    attached to the focused experiment.

    The I{Target} is used to restrict the knowledge 
    of the experiment or data collection to certain portions 
    of an appliaction.

    It is ambiguous to use both the -f and -p options.
    	- The -f option implies that an executable is to 
	be loaded into the B{OpenSS} tool from a file.
    	- The use of -p option implies that the B{OpenSS} tool 
	is to attach to a program that is already executing.

    If I{Target} is provided without I{ExpTypeList}, 
    those applications are attached to all the expTypes 
    that are already attached of the experiment.

    If I{ExpTypeList} is provided without I{Target}, 
    the specified data collectors are used to instrument all 
    the executable routines that are already attached to 
    the indicated experiment.

    If both I{Target} and I{ExpTypeList} are provided, 
    only those portions of the application in the I{Target} 
    are linked to the specified collectors in I{ExpTypeList}.

    If neither I{Target} nor I{ExpTypeList} is provided, 
    the command does nothing.

    Example::
    	my_file = openss.FileList("file_1")
    	my_exptype = openss.ExpTypeList("pcsamp")
	my_expid = openss.expCreate()
	openss.expAttach(my_expid,my_file,my_exptype)

    @param arglist: up to 4 optional class objects:
    	- I{ModifierList} object with B{mpi}
	- I{ExpId} object 
	- I{Target} object lists
	    - I{ClusterList}
	    - I{FileList}
	    - I{HostList}
	    - I{PidList}
	    - I{TreadList}
	    - I{RankList}
    	- I{ExpTypeList}
    
    """

    cmd_string = deconstruct("expAttach",*arglist)
    return return_none(cmd_string)

##################################################
# expClose
##################################################
def expClose(*arglist):

    """
    Close/Terminate the experiment specified by I{ExpId}.
        - The B{OpenSS} tool is NOT terminated. Use exit to terminate 
	  the session.
        - All data collectors attached to the experiment are removed 
	  from the attached applications.
        - All attached applications are removed from the experiment.
        - If I{ModifierList}("B{kill}") is not provided, attached 
	  applications will continue executing.
        - If I{ModifierList}("B{kill}") is provided, attached 
	  applications will be terminated.
        - The database file that was used to save information about 
	  the applications and the collected data, is deleted if it 
	  is a temporary file, or closed if it is a user defined file. 
	  Use B{expSave} for saving information.

    If there is no I{ExpId} provided, the focused experiment will be 
    closed.

    Because important information may accidently be lost, this command 
    should only be issued when the user is certain that more performance 
    measurements are not needed and that the collected data measurements 
    will be saved if there is any chance that the user will want to take 
    another look at the results.

    Use of I{ModifierList}("B{all}") will cause all defined experiments 
    to be deleted.


    B{Examples}::
	my_expid = openss.expCreate()
    	all_mod = openss.ModifierList("all")
	kill_mod = openss.ModifierList("kill")

	openss.expclose(all_mod,my_expid)
	    -or-
	openss.expclose(all_mod,kill_mod)
	    -or-
	openss.expclose(all_mod)
	    -or-
	openss.expclose()

    @param arglist: up to 2 optional class objects:
    	- I{ModifierList} object with B{kill}
	- I{ExpId} object or I{ModifierList} object with B{all}
    

    """

    cmd_string = deconstruct("expClose",*arglist)
    return return_none(cmd_string)

##################################################
# expCreate
##################################################
def expCreate(*arglist):

    """
    Start the process of defining an experiment:
    	- Define a new experiment identifier.
    	- Set the current focus to the new identifier.
    	- Return the experiment identification identifier.
    	- The experiment will not execute until an B{expGo} 
    	  command is issued.

    If I{ModifierList} object with B{mpi} is selected, all
    the threads that are part of a running application will
    be included in the experiment.

    If I{ModifierList} object with B{mpi} is not present,
    only those threads that are explicitly listed in
    I{Target} will be included in the experiment.

    The I{Target} will associate the specified executable 
    with the experiment being defined.

    The experiment type argument, I{ExpTypeList}, can be used 
    to specify the types of data that will be collected during 
    the experiment.

    Missing arguments can be supplied later with the B{expAttach} 
    command.

    An B{expCreate} command with no arguments will still return a 
    new I{ExpId} and will set the focus.

    B{Examples}::
    	my_file = openss.FileList("file_1")
    	my_exptype = openss.ExpTypeList("usertime")
    	my_expid = openss.expCreate(my_file,my_exptype)


    @param arglist: up to 3 optional class objects:
    	- I{ModifierList} object with B{mpi}
	- I{Target} object lists
	    - I{ClusterList}
	    - I{FileList}
	    - I{HostList}
	    - I{PidList}
	    - I{TreadList}
	    - I{RankList}
    	- I{ExpTypeList}

    """

    cmd_string = deconstruct("expCreate",*arglist)

    expid = ExpId(return_int(cmd_string))

    return expid

##################################################
# expDetach
##################################################
def expDetach(*arglist):

    """
    - Detach applications from collectors for an experiment.
        - The remaining applications will not execute until 
	  an B{expGo} command is issued.
        - The experiment retains knowledge of both the 
	  applications and the collectors and they can be 
	  referenced on later commands.
        - All previously collected data samples will be 
	  retained.
        - No new data, of the specified type, will be collected 
	  for the specified application.
    - If there is no I{ExpId} provided, the focused experiment 
      is used.
    - If I{Target} is provided without I{ExpTypeList}, all 
      collectors are detached for those applications.
    - The use of I{Target}" allows control over the parts of 
      an executable program that will no longer generate 
      performance data.
        - The absence of any I{HostList} specification will 
	  cause all the hosts in the specifed experiment to 
	  stop measuring performance.
        - The default cluster contains only localhost and can 
	  be specified by using -h localhost.
        - Use of I{FileList} is not supported.
        - Use of I{PidList} will result in only the Threads 
	  associated with that Pid to stop measuring 
	  performance.
        - Use of I{TreadList} will result in only that 
	  specific Thread to stop measuring performance.
    - If both I{Target} and I{ExpTypeList} are provided, only 
      the specified collectores in I{ExpTypeList} are detached 
      from just those applications described in I{Target}.
    - If I{ExpTypeList} is provided without I{Target}, the 
      specified data collectors are detached from all the 
      executable routines that are ttached to the indicated 
      experiment.
    - If neither I{Target} nor I{ExpTypeList} is provided, 
      the command does nothing.


    Example::
    	my_file = openss.FileList("file_1")
    	my_exptype = openss.ExpTypeList("pcsamp")
	my_expid = openss.expCreate()
	openss.expAttach(my_expid,my_file,my_exptype)
	openss.expDetach(my_expid,my_exptype)


    @param arglist: up to 3 optional class objects:
	- I{ExpId} object 
	- I{Target} object lists
	    - I{ClusterList}
	    - I{HostList}
	    - I{PidList}
	    - I{TreadList}
	    - I{RankList}
    	- I{ExpTypeList}
    
    """

    cmd_string = deconstruct("expDetach",*arglist)
    return return_none(cmd_string)

##################################################
# expDisable
##################################################
def expDisable(*arglist):

    """
    - Turn off data collection.
        - The experiment remains defined.
        - Data collection is "turned off" and no new 
    	  information is saved.
        - Instrumentation is disabled but may not be 
	  removed from the application.
        - Previously collected data is still available 
	  and can be viewed or saved.
        - The attached applications will not continue 
	  executing until an B{expGo} command is issued.
    - See related commands:
        - B{expEnable} can be used to restart data collection.
        - B{expSave} can be used to save previously 
	  collected data.
        - B{expClose} can be used to destroy the experiment 
	  and free all data space used to save the collected 
	  data.
    - I{ModifierList}("B{all}") will result in data 
      collection being stopped for all defined experiments.
    - If this command is issued with no arguments, 
      data collection is stopped for the focused experiment.


    Example1::
    	#Disable the currently focused experiment
	openss.expDisable()
	
    Example2::
    	#Disable all active experiments
	my_modifier = openss.ModifierList("all")
	openss.expDisable(my_modifier)
	
    Example3::
    	#Disable a specified experiment (7)
	my_exp = openss.ExpId(7)
	openss.expDisable(my_exp)

    @param arglist: 1 of 2 optional class objects:
	- I{ModifierList}("B{all}") 
    	- I{ExpId}
    
    """

    cmd_string = deconstruct("expDisable",*arglist)
    return return_none(cmd_string)

##################################################
# expEnable
##################################################
def expEnable(*arglist):

    """
    - Turn on any instrumentation that was turned off with an 
      B{expDisable} command.
        - Existing instrumentation is enabled.
        - Data collection is "turned on" and new information 
	  will be saved.
        - New information will be merged with any previously 
	  collected data.
        - The attached applications will not continue executing 
	  until an B{expGo} command is issued.
    - I{ModifierList}("B{all}") will result in data collection 
      being restarted for all defined experiments.
    - If this command is issued with no arguments, data 
      collection for the focused experiment is restarted.


    Example1::
    	#Enable the currently focused experiment
	openss.expEnable()
	
    Example2::
    	#Enable all experiments
	my_modifier = openss.ModifierList("all")
	openss.expEnable(my_modifier)
	
    Example3::
    	#Enable a specified experiment (7)
	my_exp = openss.ExpId(7)
	openss.expEnable(my_exp)

    @param arglist: 1 of 2 optional class objects:
	- I{ModifierList}("B{all}") 
    	- I{ExpId}
    

    """

    cmd_string = deconstruct("expEnable",*arglist)
    return return_none(cmd_string)

##################################################
# expFocus
##################################################
def expFocus(*arglist):

    """
    - Make the given experiment id the current, focused 
      experiment.
    - In many commands I{ExpId} is optional. When omitted, 
      the currently "focused" experiment is used.
    - If no argument is present on this command, return the 
      focused experiment.
    - If the I{ExpId} argument has an experiment id of 
      zero (0), then clear the experiment focus.
        - A command that uses an experiment id of zero is 
	  illegal and will produce a fatal error.
    - This command does not change the execution state of 
      an experiment.

    Example1::
    	#Get the Id of the currently focused experiment
	cur_id = openss.expEnable()
	
    Example2::
    	#Change the focus to experiment #7
	my_exp = openss.ExpId(7)
	cur_id = openss.expEnable(my_exp)


    @param arglist: 1 optional class object:
    	- I{ExpId}
    """

    cmd_string = deconstruct("expFocus",*arglist)
    return return_int(cmd_string)

##################################################
# expGo
##################################################
def expGo(*arglist):

    """
    - Run the experiment and collect performance data.
    - If the experiment has already begun execution, this 
      command causes execution to continue from where it 
      was interrupted. - The experiment will continue to 
      run until either
        - the application terminates, or
        - the application executes a programmed halt, or
        - the user issues another exp... command that refers to
	  this experiment Id. or
        - the B{OpenSS} tool terminates due to an exit command,
	  an external interrupt, or abnormal, internal event.
    - Except when executed in batch mode, the execution of the 
      application and the collection of performance data does 
      not, automatically, prevent other commands from being 
      issued
        - A following command that depends on the result of 
	  this experiment may not be able to execute and may 
	  block further commands from being issued.
        - Independent commands may be issued and completed 
	  before this experiment completes.
        - The status of any experiment can be determined 
	  through the B{listStatus} command. Current available
	  status strings are:
    	    - "B{Unknown}":  	An unknown status was returned.
    	    - "B{Paused}":   	The process is in a "paused" state.
    	    - "B{Running}":  	The process is still running.
    	    - "B{Terminated}":	The process has finished running.
    	    - "B{Error}":    	Something went wrong.
        - Depending on the type of data collection that is 
	  taking place, it may be possible to view the results 
	  before the application completes.
    - The absence of the I{ExpId} and I{ModifierList}("B{all}") will result in only the focused experiment being run.
    - The use of I{ExpId} will result in only that experiment 
      being run.
    - The use of I{ModifierList}("B{all}") will result in 
      every defined experiment being run.

    Example1::
    	#Run the currently focused experiment
	openss.expGo()
	
    Example2::
    	#Run all experiments
	my_modifier = openss.ModifierList("all")
	openss.expGo(my_modifier)
	
    Example3::
    	#Run a specified experiment (7)
	my_exp = openss.ExpId(7)
	openss.expGo(my_exp)

    @param arglist: 1 of 2 optional class objects:
	- I{ModifierList}("B{all}") 
    	- I{ExpId}
    

    """

    cmd_string = deconstruct("expGo",*arglist)
    return return_none(cmd_string)

##################################################
# expPause
##################################################
def expPause(*arglist):

    """
    - Temporarily halt the experiment identified by the input 
      experiment id.
    - If I{ExpId} is not provided, the focused experiment is 
      used.
    - If I{ModifierList}("B{all}") is present, every defined 
      experiment is halted.

      expPause [ I{ModifierList}("B{all}") || I{ExpId} ] 


    Example1::
    	#Halt the currently focused experiment
	openss.expPause()
	
    Example2::
    	#Halt all experiments
	my_modifier = openss.ModifierList("all")
	openss.expPause(my_modifier)
	
    Example3::
    	#Halt a specified experiment (7)
	my_exp = openss.ExpId(7)
	openss.expPause(my_exp)

    @param arglist: 1 of 2 optional class objects:
	- I{ModifierList}("B{all}") 
    	- I{ExpId}
    
    """

    cmd_string = deconstruct("expPause",*arglist)
    return return_none(cmd_string)

##################################################
# expRestore
##################################################
def expRestore(*arglist):

    """
    - Restore an experiment's definition and previously saved 
      data from a database that was saved with an expSave 
      command.
        - A new experiment is created and intitialize with 
	  this information.
        - This command also sets the focus to the new 
	  experiment's Identifier.
        - The new experiment will be in the same state as 
	  that established with the B{expDisable} command.
        - If the original applications are still running, 
	  the user can reconnect to them, and reinsert 
	  instrumentation into them, through the use of the 
	  B{expEnable} command. Doing this will interrupt the 
	  executing application and an B{expGo} command will be 
	  required before the application continues and new 
	  data samples can be collected.
        - Previously collected data can be looked at with the 
	  B{expView} command without reconnecting to the original 
	  applications.
        - If an B{expGo} command is issued, new data samples will 
	  be placed into the database specified by I{FileList}. 
	  A different database can be specified with the 
	  B{expSave} command.

      I{ExpId} = expRestore <file_spec> 

    Example::
    	#Restore an experiment
	my_file = openss.FileList("stored_data.db")
	my_exp = openss.expRestore(my_file)

    @param arglist: 1 class object:
    	- I{FileList}

    """

    cmd_string = deconstruct("expRestore",*arglist)
    return return_int(cmd_string)

##################################################
# expSave
##################################################
def expSave(*arglist):

    """
    - Save the experiment information for further analysis.
        - For each experiment, information about the executable 
	  programs that are instrumented, information about the 
	  instrumentation that is used and any performance data that 
	  is collected are saved in a database.
        - The database is created by the B{OpenSS} tool in a temporary 
	  location and will be deleted when an B{expClose} command is 
	  issued for the experiment, or when an exit command is issued 
	  that terminates the B{OpenSS} tool.
        - The entire database can be saved with the expSave command 
	  and can be reloaded into the B{OpenSS} tool with the 
	  B{expRestore} command.
        - The database is in a special format that supports the 
	  opertaions that the B{OpenSS} tool must perform.
    - Without I{ModifierList}("B{copy}")
        - If the original, temporary database file is still in use, 
	  it is copied to the specified file name and the B{OpenSS} 
	  tool will use the new file to collect any new information 
	  about the experiment. When the experiment completes, this 
	  database will be retained.
        - If a previous B{expSave} command has caused the experiment's 
	  information to be directed to a user defined database
        - The contents of the previous database - without any of the 
	  data samples - is copied to the new database.
        - The previous database is closed.
        - The new database is used by the B{OpenSS} tool to collect 
	  any new information that is added to or collected for, the 
	  experiment.
    - With I{ModifierList}("B{copy}"), the given location receives a 
      copy of the current state of the database.
        - The current contents of the database is duplicated and saved 
	  to the given location.
        - No new information will be added to the given location.
        - New information will be added to the original database.
    - This command does not change the execution state of the 
      experiment, although it may temporarily suspend execution while 
      the command is being executed.

    Example1::
    	#Store the currently focused experiment
	my_file = openss.FileList("stored_data.db")
	openss.expSave(my_file)
	
    Example2::
    	#Copy the currently focused experiment
	my_file = openss.FileList("stored_data.db")
	my_modifier = openss.ModifierList("copy")
	openss.expSave(my_modifier,my_file)
	
    Example3::
    	#Store a specified experiment (7) to file
	my_file = openss.FileList("stored_data.db")
	my_exp = openss.ExpId(7)
	openss.expSave(my_exp,my_file)

    @param arglist: From 1 to 3 class objects:
    	- I{ModifierList}("B{copy}") (optional)
	- I{ExpId}                   (optional)
    	- I{FileList}                (required)

    """

    cmd_string = deconstruct("expSave",*arglist)
    return return_none(cmd_string)

##################################################
# expSetParam
##################################################
def expSetParam(*arglist):

    """
    - Change the value an experiment's argument.
    - A parameter's setting will be unique within the experiment.
    - The parameter names can be used by other experiments.
    - This command does not change the execution state of 
      the experiment, although it may temporarily suspend 
      execution while the command is being executed.

    Example1::
    	# Create a hardware counting experiment
    	my_expid = openss.expCreate("hwc")

    	my_param_list = openss.ParamList()
	# Set which counter event we want
    	my_param_list += ("event","PAPI_HW_INT")
	# Set the sampling rate
    	my_param_list += ("sampling_rate",2000)
    	# Set the paramerter list to the experiment
    	openss.expSetParam(my_expid,my_param_list)
	# Attach the test program to the experiment
    	my_prog = openss.FileList("bosco.exe")
	openss.expAttach(my_expid,my_prog)
	# Run the experiment
	openss.expGo()

    @param arglist: From 1 to 2 class objects:
	- I{ExpId}                   (optional)
    	- I{ParamList}               (required)

    """

    cmd_string = deconstruct("expSetParam",*arglist)
    return return_none(cmd_string)

##################################################
# expStatus
##################################################
def expStatus(*arglist):

    """
    - Report the current state of user defined experiments.
        - The absence of any option will result in information 
	  being reported for the focused experiment.
        - The use of I{ExpId} will result in information being 
	  reported for the specified experiment.
        - The use of I{ModifierList}("B{all}") will result in 
	  information being reported for all the known experiments.

    Example1::
    	#Status for the focused experiment
	openss.expStatus()
	
    Example2::
    	#Status for all experiments
	my_modifier = openss.ModifierList("all")
	openss.expStatus(my_modifier)
	
    Example3::
    	#Status for a specified experiment (7)
	my_exp = openss.ExpId(7)
	openss.expStatus(my_exp)

    @param arglist: 0 to 2 optional class objects:
	- I{ModifierList}("B{all}") 
    	- I{ExpId}
    
    """

    cmd_string = deconstruct("expStatus",*arglist)
    return return_none(cmd_string)

##################################################
# expView
##################################################
def expView(*arglist):

    """
    - Display the performance data that has been collected for an 
      experiment.
    - If this command is issued with no I{ExpId}, the focused 
      experiment is used.
    - If this command is issued with no I{ViewType}, the specified 
      experiment is examined and an attempt is made to find a 
      I{ViewType} with the same name as the I{ExpType} used in the 
      experiment. If there is no matching I{ViewType}, or if there 
      is more than one I{ExpType} used in the experiment, a generic 
      view is requested that will display all the metrics that were 
      generated for the experiment.
    - This command does not change the execution state of the 
      experiment, although the nature of the requested data may 
      require that the experiment complete execution before the 
      information can be provided.
    - The use of the I{MetricList} option allows control over the 
      data that is included in the report.
        - Only the metrics specified in the list will be included in 
	  the report.
        - The meterics will be reported in the order that they occur 
	  in the list.
        - The report will be sorted in descending order of the first 
	  metric in the list.
    - The use of the I{Target} option acts like a filter on the 
      output.
        - The absence of any I{HostList} specification will cause all 
	  the information available for all hosts in the specifed 
	  experiment to be included.
        - The default cluster contains only localhost and can be 
	  specified by using -h localhost.
        - Use of I{FileList} is not supported.
        - Use of I{PidList} will result in only the Threads associated 
	  with that Pid being included for the selected hosts.
        - Use of I{TreadList} will result in only that specific Thread 
	  being included, if it exists on the selected hosts.

      expView [ I{ExpId} ] [ <viewType> ] [ -m <expMetric_list> ] [ I{Target} ] 

    Example::
    	my_file = openss.FileList("bosco")
	my_viewtype = openss.ViewTypeList("pcsamp")
    	my_metric_list = openss.MetricList()
    	my_metric_list += ("pcsamp","inclusive")

	ret = openss.expView(my_expid,my_viewtype,my_metric_list)


    @param arglist: up to 4 optional class objects:
	- I{ExpId} object 
    	- I{ViewType}
    	- I{MetricList}
	- I{Target} object lists
	    - I{ClusterList}
	    - I{FileList}
	    - I{HostList}
	    - I{PidList}
	    - I{TreadList}
	    - I{RankList}
    
    """

    cmd_string = deconstruct("expView",*arglist)
    return return_int_list(cmd_string)

##################################################
# listBreaks
##################################################
def listBreaks(*arglist):

    """
    - List the breakpoints that have been set by the user.
        - The absence of any option will cause all the breakpoints 
	  for the focused experiment to be listed.
        - The use of I{ExpId} will cause all the breakpoints for 
	  the specified experiment to be listed.
        - The use of I{ModifierList}("B{all}") will cause all the 
	  known breakpoints, for all of the experiments defined by 
	  the user, to be listed.

    Example1::
    	#List breaks set in the currently focused experiment
	int_list = openss.listBreaks()
	
    Example2::
    	#List breaks set in all active experiments
	my_modifier = openss.ModifierList("all")
	int_list = openss.listBreaks(my_modifier)
	
    Example3::
    	#List breaks set in a specified experiment (7)
	my_exp = openss.ExpId(7)
	int_list = openss.listBreaks(my_exp)

    @param arglist: 1 of 2 optional class objects:
	- I{ModifierList}("B{all}") (optional)
    	- I{ExpId} (optional)
    
    """

    cmd_string = deconstruct("listBreaks",*arglist)
    return return_int_list(cmd_string)

##################################################
# listExp
##################################################
def listExp(*arglist):

    """
    - List the experiments that have been defined.

    Example::
    	int_list = openss.listExp()

    @param arglist: None

    """

    cmd_string = deconstruct("listExp",*arglist)
    return return_int_list(cmd_string)

##################################################
# listHosts
##################################################
def listHosts(*arglist):

    """
    - Lists the hosts that define the specified cluster.
        - The absence of any options will cause all the hosts 
	  that have been included in the focused experiment to 
	  be listed.
        - The I{ExpId} option will cause all the hosts that have 
	  been included in the selected experiment to be listed.

      <ListOf_hostname> = listHosts [ I{ExpId} ] 

    Example1::
    	#List hosts for currently focused experiment (7)
    	int_list = openss.listHosts()

    Example2::
    	#List hosts for a specified experiment (7)
	my_exp = openss.ExpId(7)
	int_list = openss.listHosts(my_exp)

    @param arglist: 1 optional class object:
    	- I{ExpId} (optional)
    
    """

    cmd_string = deconstruct("listHosts",*arglist)
    return return_string_list(cmd_string)

##################################################
# listMetrics
##################################################
def listMetrics(*arglist):

    """
    - Retrieve the metrics that are associated with a 
      I{ViewTypeList} object.
        - If no options are selected, metrics for only the data 
	  collectors that are part of the focused experiment will 
	  be listed.
        - The use of I{ExpId} will cause the metrics that are 
	  associated with the set of data collectors that are 
	  part of the specified experiment to be listed.
        - The use of I{ModifierList}("B{all}") will cause all the 
	  metrics for all available collectors to be listed.
        - The use of the "<viewType_list>" option will result in a 
	  listing of only those metrics associated with the specific 
	  data collectors in the list.

      <ListOf_expMetric> = listMetrics [ I{ModifierList}("B{all}") || I{ExpId} || I{ExpTypeList} ] 

    """

    cmd_string = deconstruct("listMetrics",*arglist)
    return return_string_list(cmd_string)

##################################################
# listObj
##################################################
def listObj(*arglist):

    """
    - List the objects of the applications that are part of the 
      specified experiment.
        - If I{ExpId} is not provided, the focused experiment is used.
    - The listing can be restricted with the use of a <target> 
      specification.
    - Any component described in the <target> specification must be 
      part of the selected experiment.
    - If <target> is not provided, information will be provided for all 
      portions of all applications that are attached to the experiment.

      <ListOf_filename> = listObj [ I{ExpId} ] [ <target> ] 

    """

    cmd_string = deconstruct("listObj",*arglist)
    return return_string_list(cmd_string)

##################################################
# listParams
##################################################
def listParams(*arglist):

    """
    - Retrieve the parameters that are associated with a particular 
      <viewType> or set of <viewType>s.
        - If no options are selected, parameters for only the data 
	  collectors that are part of the focused experiment will be 
	  listed.
        - The use of I{ExpId} will cause the parameters that are 
	  associated with the set of data collectors that are part of 
	  the specified experiment to be listed.
        - The use of I{ModifierList}("B{all}") will cause all the 
	  parameters for all available collectors to be listed.
        - The use of the "<viewType>" option will result in a listing 
	  of only those parameters associated with that specific data 
	  collector.

      <ListOf_expParam> = listParams [ I{ModifierList}("B{all}") || I{ExpId} || I{ExpTypeList} ] 

    """

    cmd_string = deconstruct("listParams",*arglist)
    return return_string_list(cmd_string)

##################################################
# listPids
##################################################
def listPids(*arglist):

    """
    - List running processes associated with a specific experiment and,
      optionally, on a specific machine.
        - If no options are supplied, the Pids that are referenced in 
	  the focused experiment are listed.
        - If the "I{ExpId}" option is supplied, all the Pids that are 
	  part of the specified experiment are listed.
    - The use of the I{HostList} option acts like a filter on the output.
        - The absence of any I{HostList} specification will cause all 
	  pids in the specified experiment to be included.
        - The default cluster contains only localhost and can be 
	  specified by using I{HostList}.

      <ListOf_pidname> = listPids [ I{ExpId} ] [ <host_list_spec> ] 

    """

    cmd_string = deconstruct("listPids",*arglist)
    return return_int_list(cmd_string)

##################################################
# listRanks
##################################################
def listRanks(*arglist):

    """
    - List the mpi ranks associated with a specific experiment, a 
      specific Pid or on a specific machine.
        - If no options are supplied, all the Ranks that are referenced 
	  in the focused experiment are listed.
        - If the "I{ExpId}" option is supplied, all the Ranks that are 
	  part of the specified experiment are listed.
    - The use of the "<target>" option acts like a filter on the output.
        - The absence of any I{HostList} specification will cause all 
	  Ranks in the specified experiment to be included.
        - The default cluster contains only localhost and can be 
	  specified by using I{HostList}.
        - Use of I{FileList} is not supported.
        - Use of I{PidList} will result in only the Ranks associated 
	  with that Pid being included for the selected hosts.
        - Use of I{RankList} will result in only that specific Rank 
	  being included, if it exists on the selected hosts.

      <ListOf_rankname> = listRanks [ I{ExpId} ] [ <target> ] 

    """

    cmd_string = deconstruct("listRanks",*arglist)
    return return_int_list(cmd_string)

##################################################
# listSrc
##################################################
def listSrc(*arglist):

    """
    - List the source filenames of the modules that are part of the 
      specified experiment.
    - If I{ExpId} is not provided, the focused experiment is used.
    - The listing can be restricted with the use of a <target> 
      specification.
    - Any component described in the <target> specification must be part
      of the selected experiment.
    - I{FileList} can be used to provide a list of object modules 
      that will be searched for relevant source files.
    - If <target> is not provided, information will be provided for all
      portions of all applications that are attached to the experiment.

      <listOf_filename> = listSrc [ I{ExpId} ] [ <target> ] 

    """

    cmd_string = deconstruct("listSrc",*arglist)
    return return_string_list(cmd_string)

##################################################
# listStatus
##################################################
def listStatus(*arglist):

    """
    - List the status of experiments.
        - If no option is selected, return the status of the focused 
	  experiment.
        - The use of I{ExpId} will cause the status of the specified 
	  experiment to be returned.
        - The use of I{ModifierList}("B{all}") will cause the status 
	  of all the defined experiments to be returned.
	- The return value is a list of strings representing
	  the status of the current focused experiment process.
	  Current available status strings are:
    	    - "B{Unknown}":  	An unknown status was returned.
    	    - "B{Paused}":   	The process is in a "paused" state.
    	    - "B{Running}":  	The process is still running.
    	    - "B{Terminated}":	The process has finished running.
    	    - "B{Error}":    	Something went wrong.


      <ListOf_statusType> = listStatus [ I{ModifierList}("B{all}") || I{ExpId} ] 

    """

    cmd_string = deconstruct("listStatus",*arglist)
    return return_string_list(cmd_string)

##################################################
# listThreads
##################################################
def listThreads(*arglist):

    """
    - List the Threads associated with a specific experiment, a 
      specific Pid or on a specific machine.
        - If no options are supplied, the Threads that are referenced 
	  in the focused experiment are listed.
        - If the "I{ExpId}" option is supplied, all the Threads that 
	  are part of the specified experiment are listed.
    - The use of the "<target>" option acts like a filter on the output.
        - The absence of any I{HostList} specification will cause all 
	  Threads in the specified experiment to be included.
        - The default cluster contains only localhost and can be 
	  specified by using I{HostList}.
        - Use of I{FileList} is not supported.
        - Use of I{PidList} will result in only the Threads associated 
	  with that Pid being included for the selected hosts.
        - Use of I{TreadList} will result in only that specific Thread 
	  being included, if it exists on the selected hosts.

      <ListOf_threadname> = listThreads [ I{ExpId} ] [ <target> ] 

    """

    cmd_string = deconstruct("listThreads",*arglist)
    return return_int_list(cmd_string)

##################################################
# listTypes
##################################################
def listTypes(*arglist):

    """
    - List the available performance measurement utilities that can be 
      used to collect data in an experiment.
        - If no option is selected, list the utilities that are attached
	  to the focused experiment.
        - The use of I{ExpId} will cause the utilities that are attached
	  to the specified experiment to be listed.
        - The use of I{ModifierList}("B{all}") will cause all the 
	  popenssible performance measurement utilities that can be used 
	  in experiments to be listed.

      <ListOf_expType> = listTypes [ I{ModifierList}("B{all}") || I{ExpId} ] 

    """

    cmd_string = deconstruct("listTypes",*arglist)
    return return_string_list(cmd_string)

##################################################
# listViews
##################################################
def listViews(*arglist):

    """
    - Retrieve the views that are available for a particular <viewType> or list of <viewType>s.
        - If no options are selected, the reports for only the data 
	  collectors that are part of the focused experiment will be 
	  listed.
        - The use of I{ExpId} will cause the the reports that are 
	  associated with the set of data collectors that are part of 
	  the specified experiment to be listed.
        - The use of I{ModifierList}("B{all}") will cause all the the 
	  reports for all available collectors to be listed.
        - The use of the "<viewType_list>" option will result in a 
	  listing of only those reports associated with the specific 
	  data collectors in the list.

      <ListOf_viewType> = listViews [ I{ModifierList}("B{all}") || I{ExpId} || I{ExpTypeList} ] 


    """

    cmd_string = deconstruct("listViews",*arglist)
    return return_string_list(cmd_string)

##################################################
# clearBreak
##################################################
def clearBreak(*arglist):

    """
    - Remove a breakpoint.
    - This command does not change the execution state of an experiment,
      although it may be temporarily suspended during execution of 
      the command.

      clearBreak <breakId> 

    """

    cmd_string = deconstruct("clearBreak",*arglist)
    return return_none(cmd_string)

##################################################
# exit
##################################################
def exit(*arglist):

    """
    - Terminate all experiments and the session.
    	- Suspend execution of all executables
    	- Suspend data collection.
    	- Remove instrumentation from all applications.
    	- Release all applications from control of the B{OpenSS} tool
    	- Allow all applications to continue executing.
    	- Close all database files.
    	- Delete all temporary database files.
    	- Close the GUI and Command windows.
    	- Terminate execution of the B{OpenSS} tool.

    Example::
    	openss.exit()

    @param arglist: None

    """

    cmd_string = deconstruct("exit",*arglist)
    return return_none(cmd_string)

##################################################
# help
##################################################
def help(*arglist):

    """
    - Request information about a topic.
    - The detail of information can be controlled with the optional 
      <verbosity_spec>.

      <string> = help [ <verbosity_list_spec> ] [ <string> ] 

    """

    cmd_string = deconstruct("help",*arglist)
    return return_string(cmd_string)

##################################################
# history
##################################################
def history(*arglist):

    """
    - Print a list of previously executed commands.
    - If the optional integer is provided, the command will list 
      that number of previous commands.
    - If no integer is provided, the command will list a default 
      number of previous commands.

      <string> = history [ <int> ] 

    """

    cmd_string = deconstruct("history",*arglist)
    return return_string(cmd_string)

##################################################
# log
##################################################
def log(*arglist):

    """
    - Begin echoing executed commands and their results to a file.
    - Stop echoing if no file is specified on a log command.
    - This is primarly intended to be an internal debug aid for the B{OpenSS} tool developer since the generated files can quickly become huge.

      log [ <file_spec> ] 

    """

    cmd_string = deconstruct("log",*arglist)
    return return_none(cmd_string)

##################################################
# openGui
##################################################
def openGui(*arglist):

    """
    - Open the Graphical User Interface, if it is not already open.

      openGui 

    Example::
    	openss.openGui()

    @param arglist: None

    """

    cmd_string = deconstruct("openGui",*arglist)
    return return_none(cmd_string)

##################################################
# playBack
##################################################
def playBack(*arglist):

    """
    - Read and execute commands from a file.

      playBack <file_spec> 

    """

    cmd_string = deconstruct("playBack",*arglist)
    return return_none(cmd_string)

##################################################
# record
##################################################
def record(*arglist):

    """
    - Begin echoing executed commands to a file.
        - Recording is preformed relative to the input source that 
	  issues this command.
        - This design causes commands issued by the GUI to be recorded 
	  seperately from commands issued by the Command window.
    - Commands read from a file specified on a succeeding playback 
      command will not be echoed.
    - Stop echoing if no file is specified on a record command.
    - Stop echoing if the end of the input file containing the 
      original record command is encoutnered.

      record [ <file_spec> ] 

    """

    cmd_string = deconstruct("record",*arglist)
    return return_none(cmd_string)

##################################################
# setBreak
##################################################
def setBreak(*arglist):

    """
    - Enter a breakpoint, which will halt the application when reached.
    - If I{ExpId} is not provided, the focused experiment is used.
    - The break location is specified through the combination of 
      the <target> and <address_description> arguments.
    - The break location must be a location in the specified experiment.
    - If <target> is not provided, the <address_description> must 
      be valid on every host and executable attached to the experiment.

      <breakId> = setBreak [ I{ExpId} ] [ <target> ] <address_description>
    """

    cmd_string = deconstruct("setBreak",*arglist)
    return return_int(cmd_string)

