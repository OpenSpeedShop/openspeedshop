#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "support.h"

command_t command;

char *cmd_strtab[CMD_MAX] = {
    "CMD_EXP_ATTACH",
    "CMD_EXP_CLOSE",
    "CMD_EXP_CONT",
    "CMD_EXP_CREATE",
    "CMD_EXP_DETACH",
    "CMD_EXP_FOCUS",
    "CMD_EXP_PAUSE",
    "CMD_EXP_RESTOR",
    "CMD_EXP_RUN",
    "CMD_EXP_SAVEFILE",
    "CMD_EXP_SETPARAM",
    "CMD_EXP_STOP",
    "CMD_EXP_VIEW",
    "CMD_LIST_EXP",
    "CMD_LIST_HOSTS",
    "CMD_LIST_OBJ",
    "CMD_LIST_PIDS",
    "CMD_LIST_SRC",
    "CMD_LIST_METRICS",
    "CMD_LIST_PARAMS",
    "CMD_LIST_REPORTS",
    "CMD_LIST_BREAKS",
    "CMD_CLEAR_BREAK",
    "CMD_EXIT",
    "CMD_OPEN_GUI",
    "CMD_HELP",
    "CMD_HISTORY",
    "CMD_LOG",
    "CMD_PLAYBACK",
    "CMD_RECORD",
    "CMD_SETBREAK"
};

static void  reset_command(void);

/**
 * Function: cmd_check_array
 * 
 * Check to see if the data array in question is full
 * and needs to be reallocated. This routine assumes 
 * That the input structure describing the array contents
 * is of a standard form understood by this routine.
 *     
 * @param   p_desc    	Pointer to array descpriptor.
 *
 * @return  void.
 *
 */
static void
cmd_check_array(cmd_array_desc_t *p_desc)
{

    if (p_desc->cur_node == p_desc->max_node) {
    	int newsize;
    	int oldsize = p_desc->max_node * p_desc->entry_size;
	void *bits = (void *)p_desc->table;
    
    	if (!p_desc->entry_size) {
	    printf(
    	    	    "You forgot to initialize entry_size "
    	    	    "for table descriptor! \n");
    	}

    	if (p_desc->max_node) {
	    p_desc->max_node *= 2;
	}
	else {
	    p_desc->max_node = TABLE_START;
	}

	newsize = p_desc->max_node * p_desc->entry_size;
	if (bits)
	    bits = (unsigned int *)malloc(newsize);
	else
	    bits = (unsigned int *)realloc(bits, newsize);
	if (bits  == NULL)
	    printf("cmd_check_array(): can't grow array");

    	/* zero out the new part of the table */
    	memset((char *)(bits)+oldsize,0,newsize-oldsize);
	p_desc->table = bits;
    }
    return;
}

#if 0
/**
 * Function: xxx
 * 
 * @param   xxx    	xxx.
 *
 * @return  void.
 *
 */
static void
xxx(int indent_ndx)
{

}
#endif

/**
 * Function: set_error
 * 
 * @param   input    	string that caused error.
 * @param   help_str	string to get help with.
 *
 * @return  void.
 *
 */
void
set_error(char *input, char *help_str)
{

#if 1
    printf("ERROR: Illegal argument for command %s\n",
    	    help_str);
#else
    printf("ERROR: Illegal argument %s for command %s\n",
    	    input, help_str);
#endif
    push_help(help_str);
    dump_help(&command);
    
    /* This will prevent further messages */
    command.error_found = TRUE;
    reset_command();
}

/**
 * Function: set_command_type
 * 
 * 
 *     
 * @param   type command type.
 *
 * @return  void.
 *
 */
void 
set_command_type(oss_cmd_enum type)
{
    command.type = type;
}

/**
 * Function: set_exp_id
 * 
 * 
 *     
 * @param   exp_id experiment ID.
 *
 * @return  void.
 *
 */
void 
set_exp_id(int exp_id)
{
    command.exp_id = exp_id;
}

/**
 * Function: push_string
 * 
 * 
 *     
 * @param   name    string to store.
 * @param   tag     which command string comes from.
 *
 * @return  void.
 *
 */
/* These will get moved to specfic routines
    when and if we get to it */
void
push_string(char *name, oss_name_enum tag)
{

    char *tname = (char *)malloc(strlen(name)+1);
    name_tab_t *name_tab;
    
    strcpy(tname,name);
    
    cmd_check_array(&command.name_table);
    name_tab = (name_tab_t *)command.name_table.table;
    name_tab[command.name_table.cur_node].name = tname;
    name_tab[command.name_table.cur_node].tag = tag;
    command.name_table.cur_node++;
}

/**
 * Function: push_address
 * 
 * 
 *     
 * @param   addr    address to be stored.
 *
 * @return  void.
 *
 */
/* This really needs to be made 64 bit! */
void
push_address(int addr)
{
    int *inttab;

    cmd_check_array(&command.address_table);
    inttab = (int *)command.address_table.table;
    inttab[command.address_table.cur_node] = addr;
    command.address_table.cur_node++;
    
}

/**
 * Function: xxx
 * 
 * @param   first    	beginning of range.
 * @param   second    	end of range.
 * @param   table_type	where we are going to store it.
 *
 * @return  void.
 *
 */
void
push_32bit_range(int first, int second, oss_table_enum table_type)
{
    arg_desc_t *arg_tab;
    cmd_array_desc_t *p_table;
    int index;

    switch(table_type) {
    	case TABLE_PID:
	    p_table = &command.pid_table;
	    break;
    	case TABLE_RANK:
	    p_table = &command.rank_table;
	    break;
    	case TABLE_THREAD:
	    p_table = &command.thread_table;
	    break;
    	case TABLE_BREAK:
	    p_table = &command.break_table;
	    break;
    	case TABLE_ADDRESS: /* this needs to be in put_64bit_range */
	    p_table = &command.address_table;
	    break;
    	case TABLE_LINENO: /* Line numbers */
	    p_table = &command.lineno_table;
	    break;
    	default :
	    return;
	    break;
    }
    
    cmd_check_array(p_table);
    arg_tab = (arg_desc_t *)p_table->table;
    index = p_table->cur_node;
    arg_tab[index].tag = ARG_INT_RANGE;
    arg_tab[index].u.int_range.first = first;
    arg_tab[index].u.int_range.second = second;
    p_table->cur_node++;
}

/**
 * Function: push_32bit_value
 * 
 * @param   value    	integer to store.
 * @param   table_type	where we are going to store it.
 *
 * @return  void.
 *
 */
void
push_32bit_value(int value, oss_table_enum table_type)
{
    arg_desc_t *arg_tab;
    cmd_array_desc_t *p_table;
    int index;

    switch(table_type) {
    	case TABLE_PID:
	    p_table = &command.pid_table;
	    break;
    	case TABLE_RANK:
	    p_table = &command.rank_table;
	    break;
    	case TABLE_THREAD:
	    p_table = &command.thread_table;
	    break;
    	case TABLE_BREAK:
	    p_table = &command.break_table;
	    break;
    	case TABLE_ADDRESS: /* this needs to be in put_64bit_range */
	    p_table = &command.address_table;
	    break;
    	case TABLE_LINENO: /* Line numbers */
	    p_table = &command.lineno_table;
	    break;
    	default :
	    return;
	    break;
    }
    
    cmd_check_array(p_table);
    arg_tab = (arg_desc_t *)p_table->table;
    index = p_table->cur_node;
    arg_tab[index].tag = ARG_INT_VAL;
    arg_tab[index].u.int_value = value;
    p_table->cur_node++;
}

/**
 * Function: push_help_string
 * 
 * 
 *     
 * @param   name    help request string.
 *
 * @return  void.
 *
 */
void
push_help_string(char *name)
{
    char *tname = (char *)malloc(strlen(name)+1);
    help_desc_t *help_tab;
    
    strcpy(tname,name);
    
    cmd_check_array(&command.help_table);
    help_tab = (help_desc_t *)command.help_table.table;
    help_tab[command.help_table.cur_node].name = tname;
    help_tab[command.help_table.cur_node].tag = HELP_DUNNO;
    command.help_table.cur_node++;
}

/**
 * Function: push_host_name
 * 
 * 
 *     
 * @param   host    string representing hostname.
 *
 * @return  void.
 *
 */
void
push_host_name(char *host)
{
    char *tname = (char *)malloc(strlen(host)+1);
    host_id_t *host_tab;
    
    strcpy(tname,host);
    
    cmd_check_array(&command.host_table);
    host_tab = (host_id_t *)command.host_table.table;
    host_tab[command.host_table.cur_node].u.name = tname;
    host_tab[command.host_table.cur_node].tag = HOST_NAME;
    command.host_table.cur_node++;
}

/**
 * Function: push_host_ip
 * 
 * 
 *     
 * @param   ip_num  number representing host IP address.
 *
 * @return  void.
 *
 */
void
push_host_ip(unsigned ip_num)
{
    host_id_t *host_tab;
    
    cmd_check_array(&command.host_table);
    host_tab = (host_id_t *)command.host_table.table;
    host_tab[command.host_table.cur_node].u.ip_num = ip_num;
    host_tab[command.host_table.cur_node].tag = HOST_NUM;
    command.host_table.cur_node++;
}

/**
 * Function: push_file
 * 
 * 
 *     
 * @param   file    string representing filename to store.
 *
 * @return  void.
 *
 */
void
push_file(char *file)
{
    char *tname = (char *)malloc(strlen(file)+1);
    char **strtab;
    
    strcpy(tname,file);
    
    cmd_check_array(&command.file_table);
    strtab = (char **)command.file_table.table;
    strtab[command.file_table.cur_node] = tname;
    command.file_table.cur_node++;
}

/**
 * Function: push_help
 * 
 * 
 *     
 * @param   name    string representing name to store.
 *
 * @return  void.
 *
 */
void
push_help(char *name)
{
    char *tname = (char *)malloc(strlen(name)+1);
    help_desc_t *helptab;
    
    strcpy(tname,name);
    
    cmd_check_array(&command.help_table);
    helptab = (help_desc_t *)command.help_table.table;
    helptab[command.help_table.cur_node].name = tname;
    command.help_table.cur_node++;
}

/**
 * Function: reset_command
 * 
 * 
 *     
 * @param   void 
 *
 * @return  void.
 *
 */
static void 
reset_command()
{
    command.type = CMD_MAX;
    command.error_found = FALSE;
    command.exp_id = 0;
    /* ADDRESS TABLE */
    if (command.address_table.cur_node){
    	free(command.address_table.table);
    }
    /* PID TABLE */
    if (command.pid_table.cur_node){
    	free(command.pid_table.table);
    }
    /* BREAK TABLE */
    if (command.break_table.cur_node){
    	free(command.break_table.table);
    }
    /* RANK TABLE */
    if (command.rank_table.cur_node){
    	free(command.rank_table.table);
    }
    /* THREAD TABLE */
    if (command.thread_table.cur_node){
    	free((void *)command.thread_table.table);
    }
    /* NAME TABLE */
    if (command.name_table.cur_node){
    	int i;
	name_tab_t *name_tab = (name_tab_t *)command.name_table.table;

	for (i=0;i<command.name_table.cur_node;++i) {
	    	free((void *)name_tab[i].name);
		name_tab[i].name = NULL;
	}
	
    	free(command.name_table.table);
    }
    /* HOST TABLE */
    if (command.host_table.cur_node){
    	int i;
	host_id_t *host_tab = (host_id_t *)command.host_table.table;

	for (i=0;i<command.host_table.cur_node;++i) {
	    if (host_tab[i].tag == HOST_NAME) {
	    	free((void *)host_tab[i].u.name);
		host_tab[i].u.name = NULL;
	    }
	}
    	free(command.host_table.table);
    }
    /* FILE TABLE */
    if (command.file_table.cur_node){
    	int i;
    	char **strtab = (char **)command.file_table.table;
	
	for (i=0;i<command.file_table.cur_node;++i) {
	    free((void *)strtab[i]);
	    strtab[i] = NULL;
	}
    	free(command.file_table.table);
    }
    /* HELP TABLE */
    if (command.help_table.cur_node){
    	int i;
    	help_desc_t *helptab = (help_desc_t *)command.help_table.table;
	
	for (i=0;i<command.help_table.cur_node;++i) {
	    free((void *)helptab[i].name);
	}
    	free((void *)command.help_table.table);
    }

    memset(&command,0,sizeof(command_t));
    cmd_init();
}

/**
 * Function: dump_arg
 * 
 * This only works for tables of type arg_desc_t.
 *     
 * @param   void
 *
 * @return  void.
 *
 */
static void
dump_arg(char *label, arg_desc_t *table, int count)
{
    int i;

    if (label)
	printf("\t%s",label);

    for (i=0;i<count;++i) {
    	switch (table[i].tag) {
	    case ARG_INT_VAL:
	    	printf(" %d",table[i].u.int_value);
	    	break;
	    case ARG_ADDR_VAL:
	    	break;
	    case ARG_INT_RANGE:
	    	printf(" %d:%d",
		    	table[i].u.int_range.first,
			table[i].u.int_range.second);
	    	break;
	    case ARG_ADDR_RANGE:
	    	break;
	    case ARG_STRING:
	    	break;
	    case ARG_HOST_FILE:
	    	break;
	    default :
	    	break;
	}
    }
    printf("\n");
}

/**
 * Function: dump_command
 * 
 * 
 *     
 * @param   void
 *
 * @return  void.
 *
 */
void
dump_command()
{

    /* Set if an error occured earlier */
    if (command.type == CMD_MAX || command.error_found){
	    return;
    }
	
    /* COMMAND TYPE */
    if (command.type != CMD_HELP) {
    	printf("command: %s\n",
    	    	cmd_strtab[command.type]);
    }
    /* EXPERIMENT ID */
    if (command.exp_id) {
    	printf("\texp_id: %d\n",command.exp_id);
    }
    /* ADDRESS LIST */
    if (command.address_table.cur_node) {
    	dump_arg("ADDRESS:",
	    	 (arg_desc_t *)command.address_table.table,
	    	 command.address_table.cur_node);
    }
    /* PID LIST */
    if (command.pid_table.cur_node) {
    	dump_arg("PID:",
	    	 (arg_desc_t *)command.pid_table.table,
	    	 command.pid_table.cur_node);
    }
    /* BREAK LIST */
    if (command.break_table.cur_node) {
    	dump_arg("BREAK ID:",
	    	 (arg_desc_t *)command.break_table.table,
	    	 command.break_table.cur_node);
    }
    /* RANK LIST */
    if (command.rank_table.cur_node) {
    	dump_arg("RANK:",
	    	 (arg_desc_t *)command.rank_table.table,
	    	 command.rank_table.cur_node);
    }
    /* THREAD LIST */
    if (command.thread_table.cur_node) {
    	dump_arg("THREAD:",
	    	 (arg_desc_t *)command.thread_table.table,
	    	 command.thread_table.cur_node);
    }
    /* HOST LIST */
    if (command.host_table.cur_node) {
    	int i;
	host_id_t *host_tab = (host_id_t *)command.host_table.table;

	printf("\tHOSTS:");
	for (i=0;i<command.host_table.cur_node;++i) {
	    if (host_tab[i].tag == HOST_NAME)
	    	printf(" %s",host_tab[i].u.name);
	    else {
	    	printf(" %d.%d.%d.%d",
		    	(host_tab[i].u.ip_num >> 24) & 0xff,
		    	(host_tab[i].u.ip_num >> 16) & 0xff,
		    	(host_tab[i].u.ip_num >> 8) & 0xff,
		    	(host_tab[i].u.ip_num) & 0xff);
	    }
	}
	printf("\n");
    }
    /* FILE LIST */
    if (command.file_table.cur_node) {
    	int i;
	char **p_file = (char **)command.file_table.table;

	printf("\tFILES:");
	for (i=0;i<command.file_table.cur_node;++i) {
	    printf(" %s",p_file[i]);
	}
	printf("\n");
    }
    
    /* NAME LIST */
    if (command.name_table.cur_node) {
    	int i;
	name_tab_t *name_tab = (name_tab_t *)command.name_table.table;

	printf("\tSTRINGS:");
	for (i=0;i<command.name_table.cur_node;++i) {
	    switch (name_tab[i].tag) {
	    	case NAME_PARAM:
		    printf("(P)");
		    break;
	    	case NAME_EXPERIMENT:
		    printf("(E)");
		    break;
	    	case NAME_VIEW_TYPE:
		    printf("(V)");
		    break;
		default:
		    break;
	    }

	    printf(" %s",name_tab[i].name);
	}
	printf("\n");
    }

    /* HELP LIST */
    if (command.help_table.cur_node) {
    	dump_help(&command);
    }

    fflush(stdout);

    reset_command();
}

/**
 * Function: cmd_init
 * 
 * 
 *     
 * @param   void
 *
 * @return  void.
 *
 */
void
cmd_init()
{
    command.lineno_table.entry_size	= sizeof(arg_desc_t);	/*  */
    command.address_table.entry_size	= sizeof(arg_desc_t);	/*  */
    command.pid_table.entry_size    	= sizeof(arg_desc_t);
    command.break_table.entry_size  	= sizeof(arg_desc_t);
    command.rank_table.entry_size   	= sizeof(arg_desc_t);
    command.thread_table.entry_size 	= sizeof(arg_desc_t);
    command.file_table.entry_size   	= sizeof(arg_desc_t);
    command.name_table.entry_size	= sizeof(name_tab_t);	/*  */
    command.host_table.entry_size   	= sizeof(host_id_t);	/*  */
    command.help_table.entry_size   	= sizeof(help_desc_t);

}


