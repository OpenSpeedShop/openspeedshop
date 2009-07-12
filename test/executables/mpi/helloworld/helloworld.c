#include <mpi.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
  int rank, size;
  char hostname[256];

  gethostname(hostname,255);

  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  MPI_Barrier(MPI_COMM_WORLD);

  printf("Hello world!  I'm node %d of %d on host %s pid %d\n",
	rank,size,hostname,getpid());
  MPI_Finalize();
  exit(0);
}


