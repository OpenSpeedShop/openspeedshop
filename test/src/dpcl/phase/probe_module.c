
#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <sys/param.h>
#include "dpclExt.h"

static int      visit = 0;
static char msg[MAXPATHLEN];
static char msg_loc[MAXPATHLEN];
static char msg_time[MAXPATHLEN];
static int      msg_size;

void
begin_func(void *handle)
{
    int         rc;
    printf("begin_func() entered\n");
    sprintf(msg, "begin_func() invoked\n");
    msg_size = strlen(msg) + 1;
    printf("begin_func() calls AisSend msg size %d\n", msg_size);
    if ((rc = Ais_send((const char *)handle, msg, msg_size)) != 0) {
        printf("begin_func(): ERROR, Ais_send()=%d\n", rc);
    }
    printf("begin_func() called\n");
}

void
data_func(void *handle, void *_data)
{
    int rc;
    sprintf(msg, "data_func() invoked\n");
    msg_size = strlen(msg) + 1;
    if ((rc = Ais_send((const char *)handle, msg, msg_size)) != 0) {
         printf("data_func(): ERROR, Ais_send()=%d\n", rc);
    }
    printf("data_func() called\n");
}

void
end_func(void *handle)
{
    int         rc;
    sprintf(msg, "end_func() invoked\n");
    msg_size = strlen(msg) + 1;
    if ((rc = Ais_send((const char *)handle, msg, msg_size)) != 0) {
         printf("end_func(): ERROR, Ais_send()=%d\n", rc);
    }
    printf("end_func() called\n");
 }

void
init_func(void *handle)
{
   int         rc;
   sprintf(msg, "init_func() started\n");
   msg_size = strlen(msg) + 1;
   if ((rc = Ais_send((const char *)handle, msg, msg_size)) != 0) {
         printf("init_func(): ERROR, Ais_send()=%d\n", rc);
   }
   printf("init_func() called\n");
}
