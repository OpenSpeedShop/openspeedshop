#include "vt_trc.h"
#include "vt_mpicom.h"
#include "vt_mpireg.h"
#include "vt_mpireq.h"
#include "vt_pform.h"
#include "vt_error.h"

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

/*
 *-----------------------------------------------------------------------------
 *
 * Init and finalize
 *
 *-----------------------------------------------------------------------------
 */

extern int vt_enter_user_called;
extern int vt_mpi_notrace;
extern int outputOTF;
extern int onlyOutputOTF;


#define IS_TRACE_ON    !vt_mpi_notrace
#define TRACE_OFF()     vt_mpi_notrace = 1
#define TRACE_ON()      vt_mpi_notrace = 0



