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

int
main( int argc, char ** argv )
{
  extern void do_gui(int, char **);
  extern void main_loop(int, char **);
  for(int i=0;i<argc;i++)
  {
    printf("argv[%d]=(%s)\n", i, argv[i] );
  }

  main_loop(argc, argv);
}

