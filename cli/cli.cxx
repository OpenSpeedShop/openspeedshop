#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <errno.h>
#include <time.h>



#include <sys/types.h>
#include <sys/stat.h>
#include <glob.h>

#include <sys/wait.h>

#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

#include "OutputObject.hxx"

unsigned int __internal_debug_setting = 0;
#include "debug.hxx"

#define D_LENGTH 1


int (*dl_event_routine)();
static bool is_GUI_up_FLAG = 0; // False
void set_gui_up(bool val)
{
  is_GUI_up_FLAG = val;
}

bool is_gui_up()
{
  return( is_GUI_up_FLAG );
}

void *mainPL = NULL;
void
main_loop(int argc, char **argv)
{
  extern void do_event_work();
  extern void do_gui(int, char **);
  extern OutputObject *process_command(const char *);

  fd_set readset,  tmpset;
  struct timeval tvset;
  int result;

  FD_ZERO(&readset);
  FD_SET(0, &readset); /* add "0" file desc to "readset" */


  char command_buffer[2048];
  printf("openss> ");
  fflush(NULL);
  while(1)
  {
//    tvset.tv_sec = 1;
    tvset.tv_sec = 0;
    tvset.tv_usec = 0;
    tmpset = readset;
    
    /* Block and wait */
    result = select(FD_SETSIZE, &tmpset, NULL, NULL, &tvset);
    if (result == 0)
    {
// printf("Timed out... Is there other work to do?\n");
      if( is_gui_up() )
      {
        // Reload the time, look for some cli stuff until timer pops.
        do_event_work();
      }
    } else if (result == -1)
    {
      fprintf(stderr,  "select() error detected. \n");
      break;
    } else
    {
      scanf("%s", command_buffer);
      if( strcmp(command_buffer,"gui") == 0 )
      {
        do_gui(argc, argv);
      } else
      {
        OutputObject *oo = process_command(command_buffer);
        if( oo )
        {
          printf("%s", oo->outputBuffer );
        }
      }
      printf("openss> ");
      fflush(NULL);
    }
  }
}

void do_event_work()
{
//  printf("Gui is up... Is there event work to take care of?\n");

//  int event_ret = dl_event_routine();
  dl_event_routine();

  return;
}


void do_gui(int argc, char **argv)
{
  printf("dlopen the ossgui.so here.\n");

  char gui_plugin_file[2048];

  char *gui_dl_name="/ossgui.so";

  char *plugin_directory = getenv("OPENSPEEDSHOP_PLUGIN_PATH");

  if( !plugin_directory )
  {
    fprintf(stderr, "Can't find the GUI plugin.  $OPENSPEEDSHOP_PLUGIN_PATH not set.\n");
    return;
  }

#ifdef REPLACE_CHECK
  if( !exists(plugin_directory) )
  {
    fprintf(stderr, "Plugin directory does not exist.  Check $OPENSPEEDSHOP_PLUGIN_PATH variable.\n");
    return;
  }
#endif // REPLACE_CHECK

  sprintf(gui_plugin_file, "%s%s", plugin_directory, gui_dl_name);

  printf("see if you can load the file %s\n", gui_plugin_file );

  void *dl_gui_object = dlopen((const char *)gui_plugin_file, (int)RTLD_LAZY );

  if( !dl_gui_object )
  {
    fprintf(stderr, "dlerror()=%s\n", dlerror() );
    return;
  }

  printf("We seemed to open the dynamic gui library (%s) just fine...\n", gui_plugin_file);

  // We've managed to open the dso.  Can we the PluginInfo entry point?
  int (*dl_gui_init_routine)(int, char **, int);
  
//  printf("Try to lookup the entry_point (%s)\n", gui_plugin_file);


  dl_gui_init_routine = (int (*)(int, char **, int))dlsym(dl_gui_object, "gui_init");
  if( dl_gui_init_routine == NULL )
  {
    fprintf(stderr, "libdso: dlsym %s not found in %s dlerror()=%s\n", "gui_init", gui_plugin_file, dlerror() );
  }
  
//  int i = (*dl_gui_init_routine)(argc, argv);
  (*dl_gui_init_routine)(argc, argv, 1);

  dl_event_routine = (int (*)())dlsym(dl_gui_object, "event_routine");

//  int event_ret = dl_event_routine();

  set_gui_up(1); // True
//  printf("%s returned %d\n", "gui_init", i);
}


OutputObject *
process_command(const char *command_buffer)
{
// printf("process_command: command_buffer=(%s)\n", command_buffer);
    char return_buffer[1024];
    if( strncmp(command_buffer, "quit", 4) == 0  ||
        strncmp(command_buffer, "exit", 4) == 0 )
    {
      exit(1);
    } else if( strncmp(command_buffer, "help", 4) == 0 ) 
    {
#ifdef PULL_FOR_USABILITY_STUDY
      strcat(return_buffer, "alias name [name|string]\n");
      strcat(return_buffer, "attach <HOST_FILE_RPT_LIST> \n");
      strcat(return_buffer, "break <filename>|<line number> \n");
      strcat(return_buffer, "continue <HOST_FILE_RPT> \n");
      strcat(return_buffer, "defineExp \n");
      strcat(return_buffer, "detach <HOST_FILE_RPT_LIST> \n");
      strcat(return_buffer, "disableExp \n");
      strcat(return_buffer, "enableExp \n");
      strcat(return_buffer, "halt <HOST_FILE_RPT_LIST> \n");
      strcat(return_buffer, "history \n");
      strcat(return_buffer, "input \n");
      strcat(return_buffer, "list -exp [<expId> || all]\n");
      strcat(return_buffer, "list -hosts <HOST_FILE_LIST> \n");
      strcat(return_buffer, "list -obj <HOST_FILE_LIST> \n");
      strcat(return_buffer, "list -pids <HOST_FILE_LIST>[-mpi] \n");
      strcat(return_buffer, "list -src <HOST_FILE_LIST> \n");
      strcat(return_buffer, "load - load an executable or list of executables \n");
      strcat(return_buffer, "log \n");
      strcat(return_buffer, "run [<expId>] [<HOST_FILE_LIST>] arguments [-hang] \n");
      strcat(return_buffer, "set \n");
      strcat(return_buffer, "show -params \n");
      strcat(return_buffer, "show -reports \n");
      strcat(return_buffer, "unalias \n");
      strcat(return_buffer, "unset \n");
      strcat(return_buffer, "gui:   Loads and brings up the gui.\n");
#endif // PULL_FOR_USABILITY_STUDY
      strcat(return_buffer, "help \n");
      strcat(return_buffer, "For this round of usability studies, no command \n");
      strcat(return_buffer, "line is supported.\n\n");
      strcat(return_buffer, "quit:  quit Open/SpeedShop...\n");
      strcat(return_buffer, "exit:  exit Open/SpeedShop...\n");
      return( new OutputObject(return_buffer) );
    } else
    {
//      printf("Unknown command: Type help for command syntax.\n");
      strcpy(return_buffer, "Unknown command: Type help for command syntax.\n");
      return( new OutputObject(return_buffer) );
    }

  return(NULL);
}
