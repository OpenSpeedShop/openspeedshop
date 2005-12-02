/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
**
** This library is free software; you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by the Free
** Software Foundation; either version 2.1 of the License, or (at your option)
** any later version.
**
** This library is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
** FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
** details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this library; if not, write to the Free Software Foundation, Inc.,
** 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/
/** MPI Wrapper Function for @FUNCTION@ */

#include "RuntimeAPI.h"
#include "blobs.h"
#include "runtime.h"

#include <mpi.h>

/*
 * Special-Purpose MPI Wrapper Functions
 *
 * Most of the MPI wrapper functions are generated automagically via the custom
 * `mkwrapper' tool and its associated template file `mkwrapper.template'. The
 * following functions cannot be generated via that mechanism because they have
 * specialized implementations that don't fit the template.
 *
 * Note that for each MPI call, we actually have TWO wrapper functions. One is
 * for C and one is for FORTRAN. The FORTRAN version is always completely lower
 * case with a trailing `_' character.
 */

int mpi_PMPI_Init(int *argc, char ***argv) 
{    
    int retval, datatype_size;
    mpi_event event;

    event.start_time = OpenSS_GetTime();

    /* Call the real MPI function */
    retval = PMPI_Init(argc, argv);

    event.stop_time = OpenSS_GetTime();

    /* Record event */
    mpi_record_event(&event, &MPI_Init);
    
    /* Return the real MPI function's return value to the caller */
    return retval;
}

int mpi_PMPI_Finalize(void)
{
    int retval, datatype_size;
    mpi_event event;

    event.start_time = OpenSS_GetTime();
    
    /* Call the real MPI function */
    retval = PMPI_Finalize();

    event.stop_time = OpenSS_GetTime();

    /* Record event */
    mpi_record_event(&event, &MPI_Finalize);

    /* Return the real MPI function's return value to the caller */
    return retval;
}
