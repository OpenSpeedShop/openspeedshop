#ifndef _SUPPORT_H
#define	_SUPPORT_H 1

#define TABLE_START 8

typedef enum {
    FALSE,
    TRUE
} boolean;

typedef enum {
    HELP_DUNNO,
    HELP_CMD,
    HELP_PARM,
    HELP_EXPERIMENT,
    HELP_VIEW,
    HELP_GENERAL,
    HELP_MAX
} oss_help_enum_t;

typedef enum {
    H_PARAM_DMODE,
    H_PARAM_DMEDIA,
    H_PARAM_DREFRESH,
    H_PARAM_XSAVEFILE,
    H_PARAM_SAMPRATE,
    H_PARAM_MAX
} help_param_enum;

typedef enum {
    H_EXP_PCSAMP,
    H_EXP_USERTIME,
    H_EXP_MPI,
    H_EXP_FPE,
    H_EXP_HWC,
    H_EXP_IO,
    H_EXP_MAX
} help_exp_enum;

typedef enum {
    H_VIEW_TOPN,
    H_VIEW_EXCLTIME,
    H_VIEW_IO,
    H_VIEW_FPE,
    H_VIEW_HWC,
    H_VIEW_MAX
} help_view_enum;

typedef enum {
    H_GEN_FOCUS,
    H_GEN_ALL,
    H_GEN_COPY,
    H_GEN_KILL,
    H_GEN_GUI,
    H_GEN_MAX,
} help_gen_enum;

typedef enum {
    CMD_HEAD_ERROR,	    /* used for error processing */
    CMD_EXP_ATTACH,
    CMD_EXP_CLOSE,
    CMD_EXP_CREATE,
    CMD_EXP_DETACH,
    CMD_EXP_DISABLE,
    CMD_EXP_ENABLE,
    CMD_EXP_FOCUS,
    CMD_EXP_PAUSE,
    CMD_EXP_RESTORE,
    CMD_EXP_GO,
    CMD_EXP_SAVE,
    CMD_EXP_SETPARAM,
    CMD_EXP_STOP,
    CMD_EXP_VIEW,
    CMD_LIST_EXP,
    CMD_LIST_HOSTS,
    CMD_LIST_OBJ,
    CMD_LIST_PIDS,
    CMD_LIST_SRC,
    CMD_LIST_METRICS,
    CMD_LIST_PARAMS,
    CMD_LIST_REPORTS,
    CMD_LIST_BREAKS,
    CMD_LIST_TYPES,
    CMD_CLEAR_BREAK,
    CMD_EXIT,
    CMD_OPEN_GUI,
    CMD_HELP,
    CMD_HISTORY,
    CMD_LOG,
    CMD_PLAYBACK,
    CMD_RECORD,
    CMD_SETBREAK,
    CMD_MAX
} oss_cmd_enum;

typedef enum {
    HOST_NAME,
    HOST_NUM,
    HOST_MAX
} oss_host_enum;

typedef struct help_desc {
    char *name;
    oss_help_enum_t tag;
    union {
    	oss_cmd_enum cmd_ndx;
	help_param_enum param_ndx;
	help_exp_enum exp_ndx;
	help_view_enum view_ndx;
	help_gen_enum gen_ndx;
    } u;
} help_desc_t;

/** A host can be either a number or a string */
typedef struct host_id {
    union {
    	char *name;
	unsigned ip_num;
    }u;
    oss_host_enum tag;
} host_id_t;

/** Should match all the tables in command struct */
typedef enum {
    TABLE_DUNNO,
    TABLE_NAME,
    TABLE_PID,
    TABLE_BREAK,
    TABLE_ADDRESS,
    TABLE_RANK,
    TABLE_THREAD,
    TABLE_HOST,
    TABLE_FILE,
    TABLE_HELP,
    TABLE_LINENO,
    TABLE_MAX
} oss_table_enum;

typedef enum {
    NAME_DUNNO,
    NAME_HELP,
    NAME_PARAM,
    NAME_EXPERIMENT,
    NAME_VIEW_TYPE,
    NAME_ADDR,
    OSS_NAME_MAX
} oss_name_enum;

/** Which suboption is this string associated with? */
typedef struct name_tab {
    char *name;
    oss_name_enum tag;
} name_tab_t;

/** range of int values */
typedef struct {
    int first;
    int second;
} int_range_t;

/** range of addresses which can be 64 bit */
typedef struct {
    int first;
    int second;
} addr_range_t;

/** host/name pairs */
typedef struct {
    char *host;
    char *file;
} name_tuple_t;

/** tag values for arg_desc_t */
typedef enum {
    ARG_INT_VAL,
    ARG_ADDR_VAL,
    ARG_INT_RANGE,
    ARG_ADDR_RANGE,
    ARG_STRING,
    ARG_HOST_FILE,
    OSS_ARG_MAX
} oss_arg_enum;

/** Storage of argument values */
typedef struct {
    oss_arg_enum tag;
    union {
    	int int_value;
	int addr_value; /* needs to be 64 bit! */
	int_range_t int_range;
	addr_range_t addr_range;
	char *string;
	name_tuple_t hostfile;
    } u;
} arg_desc_t;

/** Generic table database. */
typedef struct cmd_array_desc cmd_array_desc_t;
struct cmd_array_desc {
    void *  table;  	/**< Table of entry_size sized structures */
    
    int cur_node;	/**< Current available node in table. */
    int max_node;	/**< Max entry possible in table. */
    int entry_size; 	/**< Size in bytes for each entry in table. */
};

typedef struct a_command_struct {
    cmd_array_desc_t name_table;    /**< General strings */
    cmd_array_desc_t pid_table;     /**< Process IDs */
    cmd_array_desc_t break_table;   /**< Break ids */
    cmd_array_desc_t address_table; /**< Addresses */
    cmd_array_desc_t rank_table;    /**< Rank values */
    cmd_array_desc_t thread_table;  /**< Thread IDs */
    cmd_array_desc_t host_table;    /**< Host names */
    cmd_array_desc_t file_table;    /**< File names */
    cmd_array_desc_t help_table;    /**< Help requests */
    cmd_array_desc_t lineno_table;  /**< Line numbers */
    
    int 	    exp_id; 	    /**< experiment ID */
    oss_cmd_enum    type;   	    /**< type of command */
    boolean 	    error_found;    /**< This command has been invalidated */
} command_t;

extern command_t command;
extern char *command_name[];
extern char *experiment_name[];
extern char *viewtype_name[];
extern char *paramtype_name[];
extern char *general_name[];

extern void
dump_command(void);

extern void 
set_command_type(oss_cmd_enum type);

extern void 
set_exp_id(int exp_id);

extern void
push_string(char *name, oss_name_enum tag);

extern void
push_rank(int rank);

extern void
push_thread(int thread);

extern void
push_pid(int pid);

extern void
push_host_name(char *host);

extern void
push_host_ip(unsigned ip_num);

extern void
push_file(char *file);

extern void
push_help(char *name);

extern void
push_param(char *);

extern void
push_exp_type(char *);

extern void
push_view_type(char *);

extern void
cmd_init(void);

extern void
set_error(char *,char *);

extern void
push_32bit_value(int, oss_table_enum);

void
push_32bit_range(int, int, oss_table_enum);

extern void 
dump_help(command_t *);



#endif	/* _SUPPORT_H */

