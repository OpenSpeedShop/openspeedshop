/******************************************************************************
* FILE: mm.c
* DESCRIPTION:  
*   In this template code, the master task distributes a matrix multiply
*   operation to numtasks-1 worker tasks.
*   NOTE1:  C and Fortran versions of this code differ because of the way
*     arrays are stored/passed.  C arrays are row-major order but Fortran
*     arrays are column-major order.
* AUTHOR for MPL version: Ros Leibensperger / Blaise Barney
* LAST MPL REVISED: 09/14/93 for latest API changes.  Blaise Barney
* CONVERTED TO MPI: 11/12/94 by Xianneng Shen 
******************************************************************************/

#include <stdio.h>
#include "mpi.h"
#define NRA 62 			/* number of rows in matrix A */
#define NCA 15			/* number of columns in matrix A */
#define NCB 7   		/* number of columns in matrix B */
#define MASTER 0		/* taskid of first task */
#define FROM_MASTER 1		/* setting a message type */
#define FROM_WORKER 2		/* setting a message type */

MPI_Status status;
main(int argc, char **argv) 
{
int numtasks,			/* number of tasks in partition */
    taskid,			/* a task identifier */
    numworkers,			/* number of worker tasks */
    source,			/* task id of message source */
    dest,			/* task id of message destination */
    nbytes,			/* number of bytes in message */
    mtype,			/* message type */
    intsize,			/* size of an integer in bytes */
    dbsize,			/* size of a double float in bytes */
    rows,                      	/* rows of matrix A sent to each worker */
    averow, extra, offset,      /* used to determine rows sent to each worker */
    i, j, k,			/* misc */
    count;
double a[NRA][NCA], 		/* matrix A to be multiplied */
       b[NCA][NCB],      	/* matrix B to be multiplied */
       c[NRA][NCB];		/* result matrix C */

intsize = sizeof(int);
dbsize = sizeof(double);

MPI_Init(&argc, &argv);
MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
numworkers = numtasks-1;

/**************************** master task ************************************/
if (taskid == MASTER) {
  printf("Number of worker tasks = %d\n",numworkers);
  for (i=0; i<NRA; i++)
    for (j=0; j<NCA; j++)
      a[i][j]= i+j;
  for (i=0; i<NCA; i++)
    for (j=0; j<NCB; j++)
      b[i][j]= i*j;

  /* send matrix data to the worker tasks */
  averow = NRA/numworkers;
  extra = NRA%numworkers;
  offset = 0;
  mtype = FROM_MASTER;
  for (dest=1; dest<=numworkers; dest++) {			
    rows = (dest <= extra) ? averow+1 : averow;   	
    printf("   sending %d rows to task %d\n",rows,dest);
    MPI_Send(&offset, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
    MPI_Send(&rows, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
    count = rows*NCA;
    MPI_Send(&a[offset][0], count, MPI_DOUBLE, dest, mtype, MPI_COMM_WORLD);
    count = NCA*NCB;
    MPI_Send(&b, count, MPI_DOUBLE, dest, mtype, MPI_COMM_WORLD);

    offset = offset + rows;
    }

  /* wait for results from all worker tasks */
  mtype = FROM_WORKER;
  for (i=1; i<=numworkers; i++)	{			
    source = i;
    MPI_Recv(&offset, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
    MPI_Recv(&rows, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
    count = rows*NCB;
    MPI_Recv(&c[offset][0], count, MPI_DOUBLE, source, mtype, MPI_COMM_WORLD, 
               &status);
 
    }

  /* print results */
  printf("Here is the result matrix\n");
  for (i=0; i<NRA; i++) { 
    printf("\n"); 
    for (j=0; j<NCB; j++) 
      printf("%6.2f   ", c[i][j]);
    }
  printf ("\n");

  }  /* end of master section */



/**************************** worker task ************************************/
if (taskid > MASTER) {
  mtype = FROM_MASTER;
  source = MASTER;
  printf ("Master =%d, mtype=%d\n", source, mtype);
  MPI_Recv(&offset, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
  printf ("offset =%d\n", offset);
  MPI_Recv(&rows, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
  printf ("row =%d\n", rows);
  count = rows*NCA;
  MPI_Recv(&a, count, MPI_DOUBLE, source, mtype, MPI_COMM_WORLD, &status);
  printf ("a[0][0] =%e\n", a[0][0]);
  count = NCA*NCB;
  MPI_Recv(&b, count, MPI_DOUBLE, source, mtype, MPI_COMM_WORLD, &status);
  printf ("b=\n");
  for (k=0; k<NCB; k++)
    for (i=0; i<rows; i++) {
      c[i][k] = 0.0;
      for (j=0; j<NCA; j++)
        c[i][k] = c[i][k] + a[i][j] * b[j][k];
      }

  mtype = FROM_WORKER;
  printf ("after computer\n");
  MPI_Send(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
  MPI_Send(&rows, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
  MPI_Send(&c, rows*NCB, MPI_DOUBLE, MASTER, mtype, MPI_COMM_WORLD);
  printf ("after send\n");

  }  /* end of worker */
  MPI_Finalize();
} /* of main */
