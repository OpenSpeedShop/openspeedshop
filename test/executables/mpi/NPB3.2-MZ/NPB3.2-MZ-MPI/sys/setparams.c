/* 
 * This utility configures a NPB to be built for a specific number
 * of nodes and a specific class. It creates a file "npbparams.h" 
 * in the source directory. This file keeps state information about 
 * which size of benchmark is currently being built (so that nothing
 * if unnecessarily rebuilt) and defines (through PARAMETER statements)
 * the number of nodes and class for which a benchmark is being built. 

 * The utility takes 3 arguments: 
 *       setparams benchmark-name nprocs class
 *    benchmark-name is "sp-mz", "bt-mz", or "lu-mz"
 *    nprocs is the number of processors to run on
 *    class is the size of the benchmark
 * These parameters are checked for the current benchmark. If they
 * are invalid, this program prints a message and aborts. 
 * If the parameters are ok, the current npbsize.h (actually just
 * the first line) is read in. If the new parameters are the same as 
 * the old, nothing is done, but an exit code is returned to force the
 * user to specify (otherwise the make procedure succeeds but builds a
 * binary of the wrong name).  Otherwise the file is rewritten. 
 * Errors write a message (to stdout) and abort. 
 * 
 * This program makes use of two extra benchmark "classes"
 * class "X" means an invalid specification. It is returned if
 * there is an error parsing the config file. 
 * class "U" is an external specification meaning "unknown class"
 * 
 * Unfortunately everything has to be case sensitive. This is
 * because we can always convert lower to upper or v.v. but
 * can't feed this information back to the makefile, so typing
 * make CLASS=a and make CLASS=A will produce different binaries.
 *
 * 
 */

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <math.h>

/*
 * This is the master version number for this set of 
 * NPB benchmarks. It is in an obscure place so people
 * won't accidentally change it. 
 */

#define VERSION "3.2"

/* controls verbose output from setparams */
/* #define VERBOSE */

#define MAX_X_ZONES   128
#define MAX_Y_ZONES   128
#define MAX_NUM_ZONES ((MAX_X_ZONES)*(MAX_Y_ZONES))
#define MAX_NUM_PROCS (MAX_NUM_ZONES)
#define FILENAME      "npbparams.h"
#define DESC_LINE     "c NPROCS = %d CLASS = %c\n"
#define FINDENT       "        "
#define CONTINUE      "     > "
#define max(a,b)      (((a) > (b)) ? (a) : (b))

void get_info(char *argv[], int *typep, int *nprocsp, char *classp);
void check_info(char *benchmark, int type, int nprocs, char class);
void read_info(int type, int *nprocsp, char *classp);
void write_info(int type, int nprocs, char class);
void write_sp_info(FILE *fp, int nprocs, char class);
void write_bt_info(FILE *fp, int nprocs, char class);
void write_lu_info(FILE *fp, int nprocs, char class);
void write_compiler_info(int type, FILE *fp);
void write_convertdouble_info(int type, FILE *fp);
void check_line(char *line, char *label, char *val);
int  check_include_line(char *line, char *filename);
void put_string(FILE *fp, char *name, char *val);
void put_def_string(FILE *fp, char *name, char *val);
void put_def_variable(FILE *fp, char *name, char *val);
void zone_max_xysize(double ratio, int gx_size, int gy_size,
      	           int x_zones, int y_zones, int num_procs,
		   long *max_xysize, long *max_xybcsize,
		   int *max_numzones, int *max_lsize, int *num_procs2);

enum benchmark_types {SP, BT, LU};

int main(int argc, char *argv[])
{
  int nprocs, nprocs_old, type;
  char class, class_old;
  
  if (argc != 4) {
    printf("Usage: %s benchmark-name nprocs class\n", argv[0]);
    exit(1);
  }

  /* Get command line arguments. Make sure they're ok. */
  get_info(argv, &type, &nprocs, &class);
  if (class != 'U') {
#ifdef VERBOSE
    printf("setparams: For benchmark %s: number of processors = %d class = %c\n", 
	   argv[1], nprocs, class); 
#endif
    check_info(argv[1], type, nprocs, class);
  }

  /* Get old information. */
  read_info(type, &nprocs_old, &class_old);
  if (class != 'U') {
    if (class_old != 'X') {
#ifdef VERBOSE
      printf("setparams:     old settings: number of processors = %d class = %c\n", 
	     nprocs_old, class_old); 
#endif
    }
  } else {
    printf("setparams:\n\
  *********************************************************************\n\
  * You must specify NPROCS and CLASS to build this benchmark         *\n\
  * For example, to build a class A benchmark for 4 processors, type  *\n\
  *       make {benchmark-name} NPROCS=4 CLASS=A                      *\n\
  *********************************************************************\n\n"); 

    if (class_old != 'X') {
#ifdef VERBOSE
      printf("setparams: Previous settings were CLASS=%c NPROCS=%d\n", 
	     class_old, nprocs_old); 
#endif
    }
    exit(1); /* exit on class==U */
  }

  /* Write out new information if it's different. */
  if (nprocs != nprocs_old || class != class_old) {
#ifdef VERBOSE
    printf("setparams: Writing %s\n", FILENAME); 
#endif
    write_info(type, nprocs, class);
  } else {
#ifdef VERBOSE
    printf("setparams: Settings unchanged. %s unmodified\n", FILENAME); 
#endif
  }

  return 0;
}


/*
 *  get_info(): Get parameters from command line 
 */

void get_info(char *argv[], int *typep, int *nprocsp, char *classp) 
{

  *nprocsp = atoi(argv[2]);

  *classp = *argv[3];

  if      (!strcmp(argv[1], "sp-mz") || !strcmp(argv[1], "SP-MZ")) *typep = SP;
  else if (!strcmp(argv[1], "bt-mz") || !strcmp(argv[1], "BT-MZ")) *typep = BT;
  else if (!strcmp(argv[1], "lu-mz") || !strcmp(argv[1], "LU-MZ")) *typep = LU;
  else {
    printf("setparams: Error: unknown benchmark type %s\n", argv[1]);
    exit(1);
  }
}

/*
 *  check_info(): Make sure command line data is ok for this benchmark 
 */

void check_info(char *benchmark, int type, int nprocs, char class) 
{
  int num_zones; 

  /* check min. number of processors */
  if (nprocs <= 0) {
    printf("setparams: Number of processors must be greater than zero\n");
    exit(1);
  }

  /* check class */
  if (class != 'S' && 
      class != 'W' && 
      class != 'A' && 
      class != 'B' && 
      class != 'C' && 
      class != 'D' && 
      class != 'E' && 
      class != 'F') {
    printf("setparams: Unknown benchmark class %c\n", class); 
    printf("setparams: Allowed classes are \"S\", \"W\", \"A\" through \"F\"\n");
    exit(1);
  }

  switch(type) {
      case SP:
      case BT:
      	 if      (class == 'S') 
      	    num_zones = 2*2;
      	 else if (class == 'W') 
      	    num_zones = 4*4;
      	 else if (class == 'A') 
      	    num_zones = 4*4;
      	 else if (class == 'B') 
      	    num_zones = 8*8;
      	 else if (class == 'C') 
      	    num_zones = 16*16;
      	 else if (class == 'D') 
      	    num_zones = 32*32;
      	 else if (class == 'E') 
      	    num_zones = 64*64;
      	 else if (class == 'F') 
      	    num_zones = 128*128;
         break;
      case LU:
      	 num_zones = 4*4; 
         break;
      default:
        /* never should have gotten this far with a bad name */
        printf("setparams: (Internal Error) Benchmark type %d unknown to this program\n", type); 
        exit(1);
  }

  /* check max. number of processors */
  if (nprocs > num_zones) {
    printf("setparams: Maximum number of processors for benchmark %s, class %c is %d\n",
      	   benchmark, class, num_zones);
    exit(1);
  }
}


/* 
 * read_info(): Read previous information from file. 
 *              Not an error if file doesn't exist, because this
 *              may be the first time we're running. 
 *              Assumes the first line of the file is in a special
 *              format that we understand (since we wrote it). 
 */

void read_info(int type, int *nprocsp, char *classp)
{
  int nread;
  FILE *fp;
  fp = fopen(FILENAME, "r");
  if (fp == NULL) {
#ifdef VERBOSE
    printf("setparams: INFO: configuration file %s does not exist (yet)\n", FILENAME); 
#endif
    goto abort;
  }
  
  /* first line of file contains info (fortran), first two lines (C) */

  switch(type) {
      case SP:
      case BT:
      case LU:
          nread = fscanf(fp, DESC_LINE, nprocsp, classp);
          if (nread != 2) {
            printf("setparams: Error parsing config file %s. Ignoring previous settings\n", FILENAME);
            goto abort;
          }
          break;
      default:
        /* never should have gotten this far with a bad name */
        printf("setparams: (Internal Error) Benchmark type %d unknown to this program\n", type); 
        exit(1);
  }

  fclose(fp);

  return;

 abort:
  *nprocsp = -1;
  *classp = 'X';
  return;
}


/* 
 * write_info(): Write new information to config file. 
 *               First line is in a special format so we can read
 *               it in again. Then comes a warning. The rest is all
 *               specific to a particular benchmark. 
 */

void write_info(int type, int nprocs, char class) 
{
  FILE *fp;
  fp = fopen(FILENAME, "w");
  if (fp == NULL) {
    printf("setparams: Can't open file %s for writing\n", FILENAME);
    exit(1);
  }

  switch(type) {
      case SP:
      case BT:
      case LU:
          /* Write out the header */
          fprintf(fp, DESC_LINE, nprocs, class);
          /* Print out a warning so bozos don't mess with the file */
          fprintf(fp, "\
c  \n\
c  \n\
c  This file is generated automatically by the setparams utility.\n\
c  It sets the number of processors and the class of the NPB\n\
c  in this directory. Do not modify it by hand.\n\
c  \n");

          break;
      default:
          printf("setparams: (Internal error): Unknown benchmark type %d\n", 
                                                                         type);
          exit(1);
  }

  /* Now do benchmark-specific stuff */
  switch(type) {
  case SP:
    write_sp_info(fp, nprocs, class);
    break;
  case BT:
    write_bt_info(fp, nprocs, class);
    break;
  case LU:
    write_lu_info(fp, nprocs, class);
    break;
  default:
    printf("setparams: (Internal error): Unknown benchmark type %d\n", type);
    exit(1);
  }
  write_convertdouble_info(type, fp);
  write_compiler_info(type, fp);
  fclose(fp);
  return;
}


/* 
 * write_sp_info(): Write SP specific info to config file
 */

void write_sp_info(FILE *fp, int nprocs, char class) 
{
  int gx_size, gy_size, gz_size, niter, x_zones, y_zones;
  long max_xysize, max_xybcsize;
  int max_numzones, max_lsize, nprocs2;
  char *dt, *ratio, *int_type;

  int_type="integer";
  if      (class == 'S') 
  {gx_size = 24; gy_size=24; gz_size=6; 
   x_zones = y_zones = 2;
   dt = "0.015d0";   niter = 100;}
  else if (class == 'W') 
  {gx_size = 64; gy_size=64; gz_size=8;
   x_zones = y_zones = 4;
   dt = "0.0015d0";  niter = 400;}
  else if (class == 'A') 
  {gx_size = 128; gy_size=128; gz_size=16; 
   x_zones = y_zones = 4;
   dt = "0.0015d0";  niter = 400;}
  else if (class == 'B') 
  {gx_size = 304; gy_size=208; gz_size=17; 
   x_zones = y_zones = 8;
   dt = "0.001d0";   niter = 400;}
  else if (class == 'C') 
  {gx_size = 480; gy_size=320; gz_size=28; 
   x_zones = y_zones = 16;
   dt = "0.00067d0"; niter = 400;}
  else if (class == 'D') 
  {gx_size = 1632; gy_size=1216; gz_size=34; 
   x_zones = y_zones = 32;
   dt = "0.00030d0"; niter = 500;}
  else if (class == 'E') 
  {gx_size = 4224; gy_size=3456; gz_size=92; 
   x_zones = y_zones = 64; int_type="integer*8";
   dt = "0.0002d0"; niter = 500;}
  else if (class == 'F') 
  {gx_size = 12032; gy_size=8960; gz_size=250; 
   x_zones = y_zones = 128; int_type="integer*8";
   dt = "0.0001d0"; niter = 500;}
  else {
    printf("setparams: Internal error: invalid class %c\n", class);
    exit(1);
  }
  ratio = "1.d0";

  zone_max_xysize(1.0, gx_size, gy_size, x_zones, y_zones, nprocs,
      	          &max_xysize, &max_xybcsize, &max_numzones, &max_lsize,
                  &nprocs2);

  fprintf(fp, "%scharacter class\n", FINDENT);
  fprintf(fp, "%sparameter (class='%c')\n", FINDENT,class);
  fprintf(fp, "%sinteger num_procs, num_procs2\n", FINDENT);
  fprintf(fp, "%sparameter (num_procs=%d, num_procs2=%d)\n", 
          FINDENT, nprocs, nprocs2);
  fprintf(fp, "%sinteger x_zones, y_zones\n", FINDENT);
  fprintf(fp, "%sparameter (x_zones=%d, y_zones=%d)\n", FINDENT, x_zones, y_zones);
  fprintf(fp, "%sinteger gx_size, gy_size, gz_size, niter_default\n", 
          FINDENT);
  fprintf(fp, "%sparameter (gx_size=%d, gy_size=%d, gz_size=%d)\n", 
	       FINDENT, gx_size, gy_size, gz_size);
  fprintf(fp, "%sparameter (niter_default=%d)\n", FINDENT, niter);
  fprintf(fp, "%sinteger problem_size\n", FINDENT);
  fprintf(fp, "%sparameter (problem_size = %d)\n", FINDENT, 
          max(max_lsize,gz_size));
  fprintf(fp, "%s%s max_xysize, max_xybcsize\n", FINDENT, int_type);
  fprintf(fp, "%s%s proc_max_size, proc_max_size5, proc_max_bcsize\n", FINDENT, int_type);
  fprintf(fp, "%sparameter (max_xysize=%ld)\n",  FINDENT, max_xysize);
  fprintf(fp, "%sparameter (max_xybcsize=%ld)\n",  FINDENT, max_xybcsize);
  fprintf(fp, "%sparameter (proc_max_size=max_xysize*gz_size)\n",  FINDENT);
  fprintf(fp, "%sparameter (proc_max_size5=proc_max_size*5)\n", FINDENT);
  fprintf(fp, "%sparameter (proc_max_bcsize=max_xybcsize*(gz_size-2))\n", FINDENT);

  fprintf(fp, "%sinteger max_numzones\n", FINDENT);
  fprintf(fp, "%sparameter (max_numzones=%d)\n", FINDENT, max_numzones);
  fprintf(fp, "%sdouble precision dt_default, ratio\n", FINDENT);
  fprintf(fp, "%sparameter (dt_default = %s, ratio = %s)\n", FINDENT, dt, ratio);
  fprintf(fp, "%s%s start1, start5, qstart_west, qstart_east\n", FINDENT, int_type);
  fprintf(fp, "%s%s qstart_south, qstart_north, qoffset\n", FINDENT, int_type);
  fprintf(fp, "%s%s qcomm_size, qstart2_west, qstart2_east\n", FINDENT, int_type);
  fprintf(fp, "%s%s qstart2_south, qstart2_north\n", FINDENT, int_type);
}
  
/* 
 * write_bt_info(): Write BT specific info to config file
 */

void write_bt_info(FILE *fp, int nprocs, char class) 
{
  int    gx_size, gy_size, gz_size, niter, x_zones, y_zones;
  long   max_xysize, max_xybcsize;
  int    max_numzones, max_lsize, nprocs2;
  char   *dt, *ratio, *int_type;
  double ratio_val;

  int_type="integer";
  if      (class == 'S') 
  {gx_size = 24; gy_size=24; gz_size=6;
   x_zones = y_zones = 2; ratio = "3.0d0";
   dt = "0.010d0";   niter = 60;}
  else if (class == 'W') 
  {gx_size = 64; gy_size=64; gz_size=8;  
   x_zones = y_zones = 4; ratio = "4.5d0";
   dt = "0.0008d0";  niter = 200;}
  else if (class == 'A') 
  {gx_size = 128; gy_size=128; gz_size=16;  
   x_zones = y_zones = 4; ratio = "4.5d0";
   dt = "0.0008d0";  niter = 200;}
  else if (class == 'B') 
  {gx_size = 304; gy_size=208; gz_size=17; 
   x_zones = y_zones = 8; ratio = "4.5d0";
   dt = "0.0003d0";  niter = 200;}
  else if (class == 'C') 
  {gx_size = 480; gy_size=320; gz_size=28; 
   x_zones = y_zones = 16; ratio = "4.5d0";
   dt = "0.0001d0";  niter = 200;}
  else if (class == 'D') 
  {gx_size = 1632; gy_size=1216; gz_size=34; 
   x_zones = y_zones = 32; ratio = "4.5d0";
   dt = "0.00002d0";  niter = 250;}
  else if (class == 'E') 
  {gx_size = 4224; gy_size=3456; gz_size=92; 
   x_zones = y_zones = 64; ratio = "4.5d0";
   dt = "0.000004d0"; niter = 250; int_type="integer*8";}
  else if (class == 'F') 
  {gx_size = 12032; gy_size=8960; gz_size=250; 
   x_zones = y_zones = 128; ratio = "4.5d0";
   dt = "0.000001d0"; niter = 250; int_type="integer*8";}
  else {
    printf("setparams: Internal error: invalid class %c\n", class);
    exit(1);
  }
  sscanf(ratio, "%lfd0", &ratio_val);

  zone_max_xysize(ratio_val, gx_size, gy_size, x_zones, y_zones, nprocs,
      	          &max_xysize, &max_xybcsize, &max_numzones, &max_lsize,
                  &nprocs2);

  fprintf(fp, "%scharacter class\n", FINDENT);
  fprintf(fp, "%sparameter (class='%c')\n", FINDENT,class);
  fprintf(fp, "%sinteger num_procs, num_procs2\n", FINDENT);
  fprintf(fp, "%sparameter (num_procs=%d, num_procs2=%d)\n", 
          FINDENT, nprocs, nprocs2);
  fprintf(fp, "%sinteger x_zones, y_zones\n", FINDENT);
  fprintf(fp, "%sparameter (x_zones=%d, y_zones=%d)\n", FINDENT, x_zones, y_zones);
  fprintf(fp, "%sinteger gx_size, gy_size, gz_size, niter_default\n", 
          FINDENT);
  fprintf(fp, "%sparameter (gx_size=%d, gy_size=%d, gz_size=%d)\n", 
	       FINDENT, gx_size, gy_size, gz_size);
  fprintf(fp, "%sparameter (niter_default=%d)\n", FINDENT, niter);
  fprintf(fp, "%sinteger problem_size\n", FINDENT);
  fprintf(fp, "%sparameter (problem_size = %d)\n", FINDENT, 
          max(max_lsize,gz_size));
  fprintf(fp, "%s%s max_xysize, max_xybcsize\n", FINDENT, int_type);
  fprintf(fp, "%s%s proc_max_size, proc_max_size5, proc_max_bcsize\n", FINDENT, int_type);
  fprintf(fp, "%sparameter (max_xysize=%ld)\n",  FINDENT, max_xysize);
  fprintf(fp, "%sparameter (max_xybcsize=%ld)\n",  FINDENT, max_xybcsize);
  fprintf(fp, "%sparameter (proc_max_size=max_xysize*gz_size)\n",  FINDENT);
  fprintf(fp, "%sparameter (proc_max_size5=proc_max_size*5)\n", FINDENT);
  fprintf(fp, "%sparameter (proc_max_bcsize=max_xybcsize*(gz_size-2))\n", FINDENT);

  fprintf(fp, "%sinteger max_numzones\n", FINDENT);
  fprintf(fp, "%sparameter (max_numzones=%d)\n", FINDENT, max_numzones);
  fprintf(fp, "%sdouble precision dt_default, ratio\n", FINDENT);
  fprintf(fp, "%sparameter (dt_default = %s, ratio = %s)\n", FINDENT, dt, ratio);
  fprintf(fp, "%s%s start1, start5, qstart_west, qstart_east\n", FINDENT, int_type);
  fprintf(fp, "%s%s qstart_south, qstart_north, qoffset\n", FINDENT, int_type);
  fprintf(fp, "%s%s qcomm_size, qstart2_west, qstart2_east\n", FINDENT, int_type);
  fprintf(fp, "%s%s qstart2_south, qstart2_north\n", FINDENT, int_type);
}
  


/* 
 * write_lu_info(): Write LU specific info to config file
 */

void write_lu_info(FILE *fp, int nprocs, char class) 
{
  int itmax, inorm, gx_size, gy_size, gz_size, x_zones, y_zones;
  long max_xysize, max_xybcsize;
  int max_numzones, max_lsize, nprocs2;
  char *dt_default, *ratio, *int_type;

  x_zones = y_zones = 4; 
  int_type="integer";
  if      (class == 'S') 
     {gx_size = 24; gy_size=24; gz_size=6; 
      dt_default = "0.5d0"; itmax = 50; }
  else if (class == 'W')
     {gx_size = 64; gy_size=64; gz_size=8; 
      dt_default = "1.5d-3"; itmax = 300; }
  else if (class == 'A')  
     {gx_size = 128; gy_size=128; gz_size=16;
      dt_default = "2.0d0"; itmax = 250; }
  else if (class == 'B') 
     {gx_size = 304; gy_size=208; gz_size=17;
      dt_default = "2.0d0"; itmax = 250; }
  else if (class == 'C') 
     {gx_size = 480; gy_size=320; gz_size=28;
      dt_default = "2.0d0"; itmax = 250; }
  else if (class == 'D') 
     {gx_size = 1632; gy_size=1216; gz_size=34;
      dt_default = "1.0d0"; itmax = 300; }
  else if (class == 'E') 
     {gx_size = 4224; gy_size=3456; gz_size=92; 
      dt_default = "0.5d0"; itmax = 300; int_type="integer*8";}
  else if (class == 'F') 
     {gx_size = 12032; gy_size=8960; gz_size=250; 
      dt_default = "0.2d0"; itmax = 300; int_type="integer*8";}
  else {
    printf("setparams: Internal error: invalid class %c\n", class);
    exit(1);
  }
  inorm = itmax;
  ratio = "1.d0";

  zone_max_xysize(1.0, gx_size, gy_size, x_zones, y_zones, nprocs,
      	          &max_xysize, &max_xybcsize, &max_numzones, &max_lsize,
                  &nprocs2);

  fprintf(fp, "%scharacter class\n", FINDENT);
  fprintf(fp, "%sparameter (class='%c')\n", FINDENT,class);
  fprintf(fp, "%sinteger num_procs, num_procs2\n", FINDENT);
  fprintf(fp, "%sparameter (num_procs=%d, num_procs2=%d)\n", 
          FINDENT, nprocs, nprocs2);
  fprintf(fp, "%sinteger x_zones, y_zones\n", FINDENT);
  fprintf(fp, "%sparameter (x_zones=%d, y_zones=%d)\n", FINDENT, x_zones, y_zones);
  fprintf(fp, "%sinteger gx_size, gy_size, gz_size\n", 
          FINDENT);
  fprintf(fp, "%sparameter (gx_size=%d, gy_size=%d, gz_size=%d)\n", 
	       FINDENT, gx_size, gy_size, gz_size);
  fprintf(fp, "%sinteger problem_size\n", FINDENT);
  fprintf(fp, "%sparameter (problem_size = %d)\n", FINDENT, 
          max(max_lsize,gz_size));
  fprintf(fp, "%s%s max_xysize, max_xybcsize\n", FINDENT, int_type);
  fprintf(fp, "%s%s proc_max_size, proc_max_size5, proc_max_bcsize\n", FINDENT, int_type);
  fprintf(fp, "%sparameter (max_xysize=%ld)\n",  FINDENT, max_xysize);
  fprintf(fp, "%sparameter (max_xybcsize=%ld)\n",  FINDENT, max_xybcsize);
  fprintf(fp, "%sparameter (proc_max_size=max_xysize*gz_size)\n",  FINDENT);
  fprintf(fp, "%sparameter (proc_max_size5=proc_max_size*5)\n", FINDENT);
  fprintf(fp, "%sparameter (proc_max_bcsize=max_xybcsize*(gz_size-2))\n", FINDENT);

  fprintf(fp, "%sinteger max_numzones\n", FINDENT);
  fprintf(fp, "%sparameter (max_numzones=%d)\n", FINDENT,  max_numzones);
  fprintf(fp, "\nc number of iterations and how often to print the norm\n");
  fprintf(fp, "%sinteger itmax_default, inorm_default\n", FINDENT);
  fprintf(fp, "%sparameter (itmax_default=%d, inorm_default=%d)\n", 
	  FINDENT, itmax, inorm);
  fprintf(fp, "%sdouble precision dt_default, ratio\n", FINDENT);
  fprintf(fp, "%sparameter (dt_default = %s, ratio = %s)\n", FINDENT, 
                dt_default, ratio);
  fprintf(fp, "%s%s start1, start5, qstart_west, qstart_east\n", FINDENT, int_type);
  fprintf(fp, "%s%s qstart_south, qstart_north, qoffset\n", FINDENT, int_type);
  fprintf(fp, "%s%s qcomm_size, qstart2_west, qstart2_east\n", FINDENT, int_type);
  fprintf(fp, "%s%s qstart2_south, qstart2_north\n", FINDENT, int_type);
}


/* 
 * This is a gross hack to allow the benchmarks to 
 * print out how they were compiled. Various other ways
 * of doing this have been tried and they all fail on
 * some machine - due to a broken "make" program, or
 * F77 limitations, of whatever. Hopefully this will
 * always work because it uses very portable C. Unfortunately
 * it relies on parsing the make.def file - YUK. 
 * If your machine doesn't have <string.h> or <ctype.h>, happy hacking!
 * 
 */

#define VERBOSE
#define LL 400
#include <stdio.h>
#define DEFFILE "../config/make.def"
#define DEFAULT_MESSAGE "(none)"
void write_compiler_info(int type, FILE *fp)
{
  FILE *deffile;
  char line[LL];
  char f77[LL], flink[LL], f_lib[LL], f_inc[LL], fflags[LL], flinkflags[LL];
  char compiletime[LL], randfile[LL];
  struct tm *tmp;
  time_t t;
  deffile = fopen(DEFFILE, "r");
  if (deffile == NULL) {
    printf("\n\
setparams: File %s doesn't exist. To build the NAS benchmarks\n\
           you need to create is according to the instructions\n\
           in the README in the main directory and comments in \n\
           the file config/make.def.template\n", DEFFILE);
    exit(1);
  }
  strcpy(f77, DEFAULT_MESSAGE);
  strcpy(flink, DEFAULT_MESSAGE);
  strcpy(f_lib, DEFAULT_MESSAGE);
  strcpy(f_inc, DEFAULT_MESSAGE);
  strcpy(fflags, DEFAULT_MESSAGE);
  strcpy(flinkflags, DEFAULT_MESSAGE);
  strcpy(randfile, DEFAULT_MESSAGE);

  while (fgets(line, LL, deffile) != NULL) {
    if (*line == '#') continue;
    /* yes, this is inefficient. but it's simple! */
    check_line(line, "F77", f77);
    check_line(line, "FLINK", flink);
    check_line(line, "F_LIB", f_lib);
    check_line(line, "F_INC", f_inc);
    check_line(line, "FFLAGS", fflags);
    check_line(line, "FLINKFLAGS", flinkflags);
    check_line(line, "RAND", randfile);
  }

  
  (void) time(&t);
  tmp = localtime(&t);
  (void) strftime(compiletime, (size_t)LL, "%d %b %Y", tmp);


  switch(type) {
      case SP:
      case BT:
      case LU:
          put_string(fp, "compiletime", compiletime);
          put_string(fp, "npbversion", VERSION);
          put_string(fp, "cs1", f77);
          put_string(fp, "cs2", flink);
          put_string(fp, "cs3", f_lib);
          put_string(fp, "cs4", f_inc);
          put_string(fp, "cs5", fflags);
          put_string(fp, "cs6", flinkflags);
	  put_string(fp, "cs7", randfile);
          break;
      default:
          printf("setparams: (Internal error): Unknown benchmark type %d\n", 
                                                                         type);
          exit(1);
  }

}

void check_line(char *line, char *label, char *val)
{
  char *original_line;
  original_line = line;
  /* compare beginning of line and label */
  while (*label != '\0' && *line == *label) {
    line++; label++; 
  }
  /* if *label is not EOS, we must have had a mismatch */
  if (*label != '\0') return;
  /* if *line is not a space, actual label is longer than test label */
  if (!isspace(*line) && *line != '=') return ; 
  /* skip over white space */
  while (isspace(*line)) line++;
  /* next char should be '=' */
  if (*line != '=') return;
  /* skip over white space */
  while (isspace(*++line));
  /* if EOS, nothing was specified */
  if (*line == '\0') return;
  /* finally we've come to the value */
  strcpy(val, line);
  /* chop off the newline at the end */
  val[strlen(val)-1] = '\0';
  if (val[strlen(val) - 1] == '\\') {
    printf("\n\
setparams: Error in file make.def. Because of the way in which\n\
           command line arguments are incorporated into the\n\
           executable benchmark, you can't have any continued\n\
           lines in the file make.def, that is, lines ending\n\
           with the character \"\\\". Although it may be ugly, \n\
           you should be able to reformat without continuation\n\
           lines. The offending line is\n\
  %s\n", original_line);
    exit(1);
  }
}

int check_include_line(char *line, char *filename)
{
  char *include_string = "include";
  /* compare beginning of line and "include" */
  while (*include_string != '\0' && *line == *include_string) {
    line++; include_string++; 
  }
  /* if *include_string is not EOS, we must have had a mismatch */
  if (*include_string != '\0') return(0);
  /* if *line is not a space, first word is not "include" */
  if (!isspace(*line)) return(0); 
  /* skip over white space */
  while (isspace(*++line));
  /* if EOS, nothing was specified */
  if (*line == '\0') return(0);
  /* next keyword should be name of include file in *filename */
  while (*filename != '\0' && *line == *filename) {
    line++; filename++; 
  }  
  if (*filename != '\0' || 
      (*line != ' ' && *line != '\0' && *line !='\n')) return(0);
  else return(1);
}


#define MAXL 46
void put_string(FILE *fp, char *name, char *val)
{
  int len;
  len = strlen(val);
  if (len > MAXL) {
    val[MAXL] = '\0';
    val[MAXL-1] = '.';
    val[MAXL-2] = '.';
    val[MAXL-3] = '.';
    len = MAXL;
  }
  fprintf(fp, "%scharacter %s*%d\n", FINDENT, name, len);
  fprintf(fp, "%sparameter (%s=\'%s\')\n", FINDENT, name, val);
}

/* NOTE: is the ... stuff necessary in C? */
void put_def_string(FILE *fp, char *name, char *val)
{
  int len;
  len = strlen(val);
  if (len > MAXL) {
    val[MAXL] = '\0';
    val[MAXL-1] = '.';
    val[MAXL-2] = '.';
    val[MAXL-3] = '.';
    len = MAXL;
  }
  fprintf(fp, "#define %s \"%s\"\n", name, val);
}

void put_def_variable(FILE *fp, char *name, char *val)
{
  int len;
  len = strlen(val);
  if (len > MAXL) {
    val[MAXL] = '\0';
    val[MAXL-1] = '.';
    val[MAXL-2] = '.';
    val[MAXL-3] = '.';
    len = MAXL;
  }
  fprintf(fp, "#define %s %s\n", name, val);
}



void write_convertdouble_info(int type, FILE *fp)
{
  switch(type) {
  case SP:
  case BT:
  case LU:
    fprintf(fp, "%slogical  convertdouble\n", FINDENT);
#ifdef CONVERTDOUBLE
    fprintf(fp, "%sparameter (convertdouble = .true.)\n", FINDENT);
#else
    fprintf(fp, "%sparameter (convertdouble = .false.)\n", FINDENT);
#endif
    break;
  }
}


#ifndef BLOAD_NO_COMM
void get_neighbors(int x_zones, int y_zones, int nids[][6])
{
   int zone, izone, jzone, iz_west, iz_east, jz_south, jz_north;

   zone = 0;
   for (jzone = 0; jzone < y_zones; jzone++) {
      jz_south = ((jzone-1+y_zones) % y_zones);
      jz_north = ((jzone+1)         % y_zones);
      for (izone = 0; izone < x_zones; izone++) {
      	 iz_west = ((izone-1+x_zones) % x_zones);
      	 iz_east = ((izone+1)         % x_zones);
      	 nids[zone][0] = iz_west + jzone*x_zones;
      	 nids[zone][1] = iz_east + jzone*x_zones;
      	 nids[zone][2] = izone + jz_south*x_zones;
      	 nids[zone][3] = izone + jz_north*x_zones;
      	 nids[zone][4] = izone;
      	 nids[zone][5] = jzone;
	 zone += 1;
      }
   }
}

int get_comm_index(int zone, int iproc, int nids[][6],
                   int proc_zone_id[], int x_size[], int y_size[])
{
/*
 *  Calculate the communication index of a zone within a processor group
 */
   int id_west, id_east, id_south, id_north;
   int comm_index, izone, jzone;

   id_west  = nids[zone][0];
   id_east  = nids[zone][1];
   id_south = nids[zone][2];
   id_north = nids[zone][3];
   izone    = nids[zone][4];
   jzone    = nids[zone][5];

   comm_index = 0;
   if (proc_zone_id[id_west] == iproc)
      comm_index = comm_index + y_size[jzone];
   if (proc_zone_id[id_east] == iproc)
      comm_index = comm_index + y_size[jzone];
   if (proc_zone_id[id_south] == iproc)
      comm_index = comm_index + x_size[izone];
   if (proc_zone_id[id_north] == iproc)
      comm_index = comm_index + x_size[izone];

   return comm_index;
}
#endif


void zone_max_xysize(double ratio, int gx_size, int gy_size,
      	           int x_zones, int y_zones, int num_procs,
		   long *max_xysize, long *max_xybcsize,
		   int *max_numzones, int *max_lsize, int *num_procs2)
{
   int num_zones = x_zones*y_zones;
   int iz, mz, z2, i, j, ip, np;
   long cur_size;
   double x_r0, y_r0, x_r, y_r, x_smallest, y_smallest, aratio;

   int x_size[MAX_X_ZONES], y_size[MAX_Y_ZONES];
   int zone_size[MAX_NUM_ZONES], z_order[MAX_NUM_ZONES];
   int proc_zone_size[MAX_NUM_PROCS], proc_num_zones[MAX_NUM_PROCS];
   int proc_zone_size2[MAX_NUM_PROCS], proc_xybc_size[MAX_NUM_PROCS];
#ifndef BLOAD_NO_COMM
   int proc_zone_id[MAX_NUM_ZONES], zone, zone_comm, comm_index, tratio;
   int nids[MAX_NUM_ZONES][6];
   double diff_ratio;
#endif

   aratio = (ratio > 1.0)? (ratio-1.0) : (1.0-ratio);
   if (aratio > 1.e-10) {

/*   compute zone stretching only if the prescribed zone size ratio 
     is substantially larger than unity */

      x_r0  = exp(log(ratio)/(x_zones-1));
      y_r0  = exp(log(ratio)/(y_zones-1));
      x_smallest = (double)(gx_size)*(x_r0-1.0)/
      	           (pow(x_r0, (double)x_zones)-1.0);
      y_smallest = (double)(gy_size)*(y_r0-1.0)/
      	           (pow(y_r0, (double)y_zones)-1.0);

/*   compute tops of intervals, using a slightly tricked rounding
     to make sure that the intervals are increasing monotonically
     in size */

      x_r = x_r0;
      for (i = 0; i < x_zones; i++) {
   	 x_size[i] = x_smallest*(x_r-1.0)/(x_r0-1.0)+0.45;
	 x_r *= x_r0;
      }

      y_r = y_r0;
      for (j = 0; j < y_zones; j++) {
   	 y_size[j] = y_smallest*(y_r-1.0)/(y_r0-1.0)+0.45;
	 y_r *= y_r0;
      }
   }
   else {

/*    compute essentially equal sized zone dimensions */

      for (i = 0; i < x_zones; i++)
         x_size[i]   = (i+1)*gx_size/x_zones;

      for (j = 0; j < y_zones; j++)
         y_size[j]   = (j+1)*gy_size/y_zones;

   }

   for (i = x_zones-1; i > 0; i--) {
      x_size[i] = x_size[i] - x_size[i-1];
   }

   for (j = y_zones-1; j > 0; j--) {
      y_size[j] = y_size[j] - y_size[j-1];
   }


/* ... sort the zones in decending order */
   cur_size = 0;
   for (iz = 0; iz < num_zones; iz++) {
      i = iz % x_zones;
      j = iz / x_zones;
      zone_size[iz] = x_size[i] * y_size[j];
      z_order[iz] = iz;
      if (cur_size < x_size[i]) cur_size = x_size[i];
      if (cur_size < y_size[j]) cur_size = y_size[j];
   }
   *max_lsize = cur_size;

   for (iz = 0; iz < num_zones-1; iz++) {
      cur_size = zone_size[z_order[iz]];
      mz = iz;
      for (z2 = iz+1; z2 < num_zones; z2++) {
     	 if (cur_size < zone_size[z_order[z2]]) {
     	    cur_size = zone_size[z_order[z2]];
     	    mz = z2;
     	 }
      }
      if (mz != iz) {
     	 z2 = z_order[iz];
     	 z_order[iz] = z_order[mz];
     	 z_order[mz] = z2;
      }
   }

/* ... use a simple bin-packing scheme to balance the load among processes */
   for (ip = 0; ip < num_procs; ip++) {
      proc_zone_size[ip] = 0;
      proc_zone_size2[ip] = 0;
      proc_xybc_size[ip] = 0;
      proc_num_zones[ip] = 0;
   }

#ifdef BLOAD_NO_COMM
   for (iz = 0; iz < num_zones; iz++) {
      z2 = z_order[iz];
      np = 0;
      cur_size = proc_zone_size[0];
      for (ip = 1; ip < num_procs; ip++) {
     	 if (cur_size > proc_zone_size[ip]) {
     	    np = ip;
     	    cur_size = proc_zone_size[ip];
     	 }
      }
      proc_zone_size[np] += zone_size[z2];
      i = z2 % x_zones;
      j = z2 / x_zones;
      cur_size = x_size[i] + 1 - (x_size[i]%2);
      proc_zone_size2[np] += cur_size * y_size[j];
      cur_size = (cur_size-2 + y_size[j]-2)*10;
      proc_xybc_size[np] += cur_size;
      proc_num_zones[np] += 1;
   }
#else
   for (iz = 0; iz < num_zones; iz++)
      proc_zone_id[iz] = -1;

   /* balance with computation + communication */
   get_neighbors(x_zones, y_zones, nids);
   iz = 0;
   while (iz < num_zones) {

      /* skip the previously assigned zones */
     while (iz < num_zones && proc_zone_id[z_order[iz]] >= 0) {
   	 iz = iz + 1;
     }
      if (iz >= num_zones) break;

      /* the current most empty processor */
      np = 0;
      cur_size = proc_zone_size[0];
      for (ip = 1; ip < num_procs; ip++) {
     	 if (cur_size > proc_zone_size[ip]) {
     	    np = ip;
     	    cur_size = proc_zone_size[ip];
     	 }
      }

      /* get a zone that has the largest communication index with
   	 the current group and does not worsen the computation balance */
      mz = z_order[iz];
      if (iz < num_zones-1) {
     	 zone_comm = get_comm_index(mz, np, nids, 
   			      	    proc_zone_id, x_size, y_size);
     	 tratio = 1;
   	 for (z2 = iz+1; tratio && z2 < num_zones; z2++) {
     	    zone = z_order[z2];

     	    diff_ratio = (double)(zone_size[z_order[iz]] - 
   			  zone_size[zone]) / zone_size[z_order[iz]];
   	    if (diff_ratio > 0.05) tratio = 0;

     	    if (tratio && proc_zone_id[zone] < 0) {
     	       comm_index = get_comm_index(zone, np, nids, 
   				           proc_zone_id, x_size, y_size);
     	       if (comm_index > zone_comm) {
     		  mz = zone;
     		  zone_comm = comm_index;
     	       }
     	    }
     	 }
      }

      /* assign the zone to the current processor group */
      proc_zone_id[mz] = np;
      proc_zone_size[np] += zone_size[mz];
      i = mz % x_zones;
      j = mz / x_zones;
      cur_size = x_size[i] + 1 - (x_size[i]%2);
      proc_zone_size2[np] += cur_size * y_size[j];
      cur_size = (cur_size-2 + y_size[j]-2)*10;
      proc_xybc_size[np] += cur_size;
      proc_num_zones[np] += 1;

   }
#endif

   *max_xysize = *max_xybcsize = *max_numzones = 0;
   for (ip = 0; ip < num_procs; ip++) {
      cur_size = proc_zone_size2[ip];
      if (cur_size > *max_xysize) *max_xysize = cur_size;
      cur_size = proc_xybc_size[ip];
      if (cur_size > *max_xybcsize) *max_xybcsize = cur_size;
      cur_size = proc_num_zones[ip];
      if (cur_size > *max_numzones) *max_numzones = cur_size;
   }

   np = 1;
   while (np < num_procs)
      np *= 2;
   *num_procs2 = np;
}
