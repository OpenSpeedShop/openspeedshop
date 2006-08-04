#include <sys/types.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <fenv.h>
#include <float.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdint.h>

void fpe_enable()
{
 feenableexcept(FE_ALL_EXCEPT);
}


/* print a message and exit */
void fpe_handler(int sig_number, siginfo_t *info, void *data)
{

  printf("Entered mutatee.cxx: fpe_handler\n" );
  exit(1);
}

int main(void)
{
    /* setup a signal handler for SIGFPE */
    struct sigaction action, oldaction;

    memset(&action, 0, sizeof(action));

    action.sa_sigaction = fpe_handler;      /* which callback function */
    sigemptyset(&action.sa_mask);           /* other signals to block */

    action.sa_flags = SA_STACK | SA_ONSTACK | SA_RESTART | SA_SIGINFO;

    if (sigaction(SIGFPE, &action, &oldaction)) {
      fprintf(stderr, "error: failed to register signal handler %d (%s)\n",
              errno, strerror(errno));
      return 1;
    }

// to test whether we can get into the fpe handler uncomment the next line
//    fpe_enable();

    sleep(60);

    double aa;
    double bb;
    double cc;

    aa = 1.0;
    bb = time(0) - time(0);
    cc = aa / bb;
    printf("######### DIVIDE BY ZERO cc = aa / bb, cc=%f\n", cc);

}
