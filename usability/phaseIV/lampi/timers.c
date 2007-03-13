/* routine to return time in seconds

   Use:     (void) timers ( &cpu0, &et0 );
           ...code to be timed...
            (void) timers ( &cpu1, &et1 );
           print*,'cpu time for code was ', cpu1-cpu0
           print*,'elapsed time for code was ',et1-et0

NOTE:  AS USED IN THE ASCI BENCHMARKS THIS ROUTINE MUST RETURN
BOTH CPU TIME AND ELAPSED (WALLCLOCK) TIME.  VENDOR MUST SUPPLY
SYSTEM_SPECIFIC CODE TO DO THIS.  THE ROUTINE BELOW IS A SAMPLE
ONLY; ALTERNATIVE IMPLEMENTATIONS ARE ALLOWED.

*/
#include	<sys/time.h>
#include        <sys/times.h>
#include	<sys/resource.h>

void
timers(cpu,et)
double *cpu,*et;
{
	struct rusage r;
	struct timeval t;
	
	getrusage( RUSAGE_SELF, &r );
	*cpu = r.ru_utime.tv_sec + r.ru_utime.tv_usec*1.0e-6;
	
	gettimeofday( &t, (struct timezone *)0 );
	*et = t.tv_sec + t.tv_usec*1.0e-6;
}


/* Fortran calling flavors */

void
TIMERS(cpu,et)
double *cpu,*et;
{
	timers(cpu,et);
}

void
timers_(cpu,et)
double *cpu,*et;
{
	timers(cpu,et);
}
