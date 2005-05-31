#include <stdio.h>
#include <string.h>
#include <dpclExt.h>

void hello(void * msg_handle)
{
   char msg[100];
   sprintf(msg,"Hello world from myprobe module\n");	 
   Ais_send((const char *)msg_handle, (void *)msg, strlen(msg)+1);
   return;
}
