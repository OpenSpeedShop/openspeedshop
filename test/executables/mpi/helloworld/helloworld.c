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

  printf("Hello world!  I'm node %d of %d on host %s\n", rank,size,hostname);
  MPI_Finalize();
  exit(0);
}


