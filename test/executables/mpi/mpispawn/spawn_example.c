#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

/* 
 * From: http://mpi.deino.net/mpi_functions/MPI_Comm_spawn.html 
*/
#define NUM_SPAWNS 2

int main( int argc, char *argv[] )
{
    int rank;
    int np = NUM_SPAWNS;
    int errcodes[NUM_SPAWNS];
    MPI_Comm parentcomm, intercomm;

    MPI_Init( &argc, &argv );
    MPI_Comm_rank(MPI_COMM_WORLD,&rank); 
    MPI_Comm_get_parent( &parentcomm );
    if (parentcomm == MPI_COMM_NULL)
    {
        /* Create 2 more processes - this example must be called spawn_example.exe for this to work. */
        MPI_Comm_spawn( "spawn_example", MPI_ARGV_NULL, np, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &intercomm, errcodes );
        printf("I'm the parent., rank=%ld, parentcomm=%ld, intercomm=%ld\n", rank, parentcomm, intercomm);
    } else {
        printf("I'm the spawned., rank=%ld, parentcomm=%ld, intercomm=%ld\n", rank, parentcomm, intercomm);
    }
    fflush(stdout);
    MPI_Finalize();
    return 0;
}
