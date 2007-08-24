#include "vt_trc.h"
#include "vt_mpicom.h"
#include "vt_mpireg.h"
#include "vt_mpireq.h"
#include "vt_pform.h"
#include "vt_error.h"
#include "vt_memhook.h"

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
 /* initialized once from environment variable */
extern int vt_mpitrace;
/* changed with every TRACE_ON/TRACE_OFF */
extern int vt_mpi_trace_is_on;

extern int outputOTF;
extern int onlyOutputOTF;

#define IS_TRACE_ON    vt_mpi_trace_is_on
#define TRACE_OFF() \
  VT_MEMHOOKS_OFF(); \
  vt_mpi_trace_is_on = 0;
#define TRACE_ON() \
  VT_MEMHOOKS_ON(); \
  vt_mpi_trace_is_on = vt_mpitrace;

