////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation; either version 2 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 59 Temple
// Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////


#include "dpcl.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#include <iostream.h>
#include <time.h>

extern char** environ;

const char USAGE1[]="\nUSAGE: eut_diag <host name> pid <pid>\n";
const char USAGE3[]="\t\tOR\n\teut_diag <host name> path </full path/executable\n";
const char USAGE5[]="\t*specify \"d\" for <host name> to use default host\n";
const int bufSize = 128;

int DONE = 0;

Process     P;
SourceType srctyp;
SourceObj * vobj_arr = NULL;
struct var_info {
   char name[bufSize+1];
   int dtype;
   void *dataptr;
};
var_info *var_arr = NULL;
ProbeExp  *send_pes = NULL;

static void terminate_cb(GCBSysType, GCBTagType, GCBObjType, GCBMsgType)
{
    Ais_end_main_loop();
}

char filename[bufSize+1], buf[bufSize+1];
FILE * logFile;
int num_var=0; 
int lflag=0;   //Ais_main_loop() started flag
int cflag=0;   //create process flag

void stdout_cb(GCBSysType sys, GCBTagType tag, GCBObjType obj, GCBMsgType msg);
void stderr_cb(GCBSysType sys, GCBTagType tag, GCBObjType obj, GCBMsgType msg);

void func_cb(GCBSysType,GCBTagType,GCBObjType,GCBMsgType);
void Initialize(char **argv);
void print_sts(AisStatus rc, char *fcnname); 
void dcall_back(GCBSysType s,GCBTagType t,GCBObjType o,GCBMsgType m);
void dcall_back2(GCBSysType s,GCBTagType t,GCBObjType o,GCBMsgType m);

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
main(int argc, char *argv[]) 
{
    
    AisStatus retval;
    if (argc < 4){
      cout<<"\n**Incorrect numbers of argument entered**"<<USAGE1<<
 		USAGE3<<USAGE5<<endl;
      exit(0);
    }

    Ais_initialize();
#if (0)
    Ais_blog_on("hope2.americas.sgi.com", LGL_detail, LGD_daemon, 0,0);
#endif


    GCBFuncType old_terminate_cb;
    GCBTagType old_terminate_tag;
    retval = Ais_override_default_callback(AIS_PROC_TERMINATE_MSG,
                                           terminate_cb, NULL,
                                           &old_terminate_cb,
                                           &old_terminate_tag);

    if(retval.status() != ASC_success) {
        printf("Ais_override_default_callback() failed: %s\n",
               retval.status_name());
        return 1;
    }


    Initialize(argv);
 
    SourceObj myprog = P.get_program_object();
    SourceObj mymod;
    char        bufmname[bufSize+1];      // buffer for module_name(..)

    char name[bufSize+1];
    bool found=false;
    
        for (int c = 0; c < myprog.child_count() && (found == false); c++)
        {
          mymod = myprog.child(c);
          const char * modname = mymod.module_name(bufmname,bufSize);
          printf("Module name %s examined\n", modname);

#if (1)
	  if (strcmp(modname, "mutatee.c") ==0) {
#endif
#if (0)
	  if (strcmp(modname, "libc.so.6.1") ==0) 
#endif
            printf("Module %s found... getting statement info\n", modname);
	    AisStatus sts(ASC_failure); 
	    StatementInfoList * stmt_info_list = mymod.bget_all_statements(P, &sts);
            if (sts.status() == ASC_success) {
               printf("bget_all_statements() SUCCESS - status %s\n",sts.status_name());
//               printf("bget_all_statements() BOOSUCCESS - status %s\n",sts.status_name());
               int stmt_info_cnt = stmt_info_list->get_count();
//               printf("stmt info count %d\n", stmt_info_cnt);
               for (int i = 0; i < stmt_info_cnt; i++) {
                  StatementInfo stmt = stmt_info_list->get_entry(i);
//                  printf("Source file %s\n", stmt.get_filename());
                  int stmt_info_linecnt = stmt.get_line_count();
//                  printf("stmt line  count %d\n", stmt_info_linecnt);
                  for (int j = 0; j < stmt_info_linecnt; j++) {
                     StatementInfoLine stmt_line = stmt.get_line_entry(j);
//                     printf("   line %d column %d\n", stmt_line.get_line(),
//                                                      stmt_line.get_column());
                     int stmt_line_address_cnt = stmt_line.get_address_count();
//                     printf("stmt address  count %d\n", stmt_line_address_cnt);
                     for (int k = 0; k < stmt_line_address_cnt; k++) {
                        unsigned long stmt_line_address =stmt_line.get_address_entry(k);
//                        printf("      address 0x%016llx\n", stmt_line_address);
                     }
                  }
               }
               printf("bget_all_statements() SUCCESS\n");
            }
            else {
               printf("bget_all_statements() FAILS !!\n");
            }
          found = true;
	  } 
      }  //end for                      

#if (0)
      printf("Destroying process\n");
      AisStatus sts = P.bdestroy();
      if (sts.status() != ASC_success){
        printf("Destroying program FAILED status %s\n",sts.status_name());
        exit(0);
      } else {
        printf("Leaving program\n");
        exit(0);
      }
#endif
    retval = P.bstart();
    if(retval.status() != ASC_success) {
        printf("Process::bstart() failed: %s\n", retval.status_name());
        return 1;
    }

    Ais_main_loop();
    return 0;
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void Initialize(char **argv){
    char **argvv=argv+3;
    char        hostname[bufSize+1];      // buffer for get_host_name()

    if(strcmp(argv[1],"d")==0){
      gethostname(hostname,bufSize);
//      printf("  *Running on \"%s\"\n",hostname);
    } else
      strcpy(hostname,argv[1]);


    if (strcmp("pid",argv[2]) == 0){     //Not POE

      P = Process(hostname, atoi(argv[3]));

      AisStatus sts = P.bconnect();
      if (sts.status() != ASC_success){
        printf("bconnect() was not successful...  %s\n",sts.status_name());
        exit(0);
      } else {
        printf("bconnect() was successful.\n");
      }
      sts=P.battach();   //stop the application
      if (sts.status() != ASC_success){
        printf("battach() was not successful...  %s\n",sts.status_name());
        exit(0);
      } else {
        printf("battach() was successful.\n");
      }

    } else if (strcmp("path",argv[2]) == 0){      //start a new appl
//      printf(" Creating \"%s\"\n",argv[3]);
      AisStatus sts = P.bcreate(hostname, argv[3], 
                                (const char**)&(argv[3]),
                                (const char**)environ,
                      stdout_cb,NULL, stderr_cb,NULL);

      if (sts.status() != ASC_success){
        printf("bcreate() was not successful...  %s\n",sts.status_name());
        exit(0);
      } else {
        printf("bcreate() was successful.\n");
        cflag=1;
      }
    

     } else {
      cout<<USAGE1<<USAGE3<<USAGE5<<endl;
      exit(0);
    }

//    printf("Using Process %d",P.get_pid());
    P.get_host_name(hostname,bufSize);
//    printf(" on host %s...\n",hostname);
}

void
stdout_cb(GCBSysType sys, GCBTagType tag, GCBObjType obj, GCBMsgType msg) {
    //  callback to receive standard out from application and display

#if (0)
    char *p = (char *) msg;
    printf("stdout_cb: ");
    for (int i=0; i<sys.msg_size; i++) {
        printf("%c",*p);
        p++;
    }
    printf("\n");
#endif
    fflush(stdout);
}

void
stderr_cb(GCBSysType sys, GCBTagType tag, GCBObjType obj, GCBMsgType msg) {
    //  callback to receive standard error from application and display

    char *p = (char *) msg;
    printf("stderr_cb: ");
    for (int i=0; i<sys.msg_size; i++) {
        printf("%c",*p);
        p++;
    }
    printf("\n");
    fflush(stdout);
}


void func_cb(GCBSysType sys,GCBTagType tag,GCBObjType obj,GCBMsgType msg) {
  //   callback to receive messages from probes installed on functions 

  char *name,*location;
 
  Process *p = (Process *) obj;
  name=(char*)msg;
  location=(char*)tag;

  fprintf(logFile,"\"%s\" - %s\n ",name,location);
  fflush(logFile);

}


void print_sts(AisStatus rc, char *fcnname) {
  printf("sts from %s: '%s' (%d/%d)\n", fcnname, rc.status_name(),
        rc.status(), rc.severity());

  for (int i=0; i<rc.data_count(); ++i)
    printf("sts.data_value(%d) = '%s'\n", i, rc.data_value(i, buf, bufSize));
}

void dcall_back(GCBSysType s,GCBTagType t,GCBObjType o,GCBMsgType m)
{
   static int complete = 0;
   char name[bufSize+1],*temp=(char*)t;
   int dtype;
   strcpy(name,temp+1);
   dtype = atoi(temp);

#ifdef DEBUG1
   cout<<"dcall_back(): dtype = "<<dtype<<endl;
   cout<<"dcall_back(): name = "<<name<<endl;
   cout<<"Variable: "<<name;
   if(dtype == 1)
     cout<<" Value: "<<*(int*)m<<endl;
   if(dtype == 2)
     cout<<" Value: "<<*(char*)m<<endl;
   if(dtype == 3)
     cout<<" Value: "<<(char*)m<<endl;
   if(dtype == 4)
     cout<<" Value: "<<*( double*)m<<endl;
#endif

   for (int i=0; i<num_var; i++){
       if (strcmp(var_arr[i].name,name)==0){
           complete++;
#ifdef DEBUG1
       cout<<" matched variable name\n";  
#endif

         //save the initial value for the variable
         if(dtype == 1){
            *(int*)(var_arr[i].dataptr)=*(int*)m;
#ifdef DEBUG1
            cout<<" saved value: "<<*(int*)(var_arr[i].dataptr)<<endl;
#endif
         } else if(dtype == 2){
            *(char *)var_arr[i].dataptr=*(char*)m;
#ifdef DEBUG1
            cout<<" saved value: "<<*(char *)var_arr[i].dataptr<<endl;
#endif
         } else if(dtype == 3){
            strcpy((char*)var_arr[i].dataptr,(char*)m);
#ifdef DEBUG1
            cout<<" saved value: "<<(char *)var_arr[i].dataptr<<endl;
#endif
         } else if(dtype == 4){
            *( double*)var_arr[i].dataptr=*( double*)m;
#ifdef DEBUG1
            cout<<" saved value: "<<*( double*)var_arr[i].dataptr<<endl;
#endif
         }
         break;
       }
   }
   //log the initial value for all variables 
}

void dcall_back2(GCBSysType s,GCBTagType t,GCBObjType o,GCBMsgType m)
{
   char name[bufSize+1],*temp=(char*)t;
   int dtype;
   strcpy(name,temp+1);
   dtype = atoi(temp);

#ifdef DEBUG2
   cout<<"dcall_back2(): dtype = "<<dtype <<" variable = "<<name;
   if(dtype == 1)
     cout<<" Value: "<<*(int*)m<<endl;
   if(dtype == 2)
     cout<<" Value: "<<*(char*)m<<endl;
   if(dtype == 3)
     cout<<" Value: "<<(char*)m<<endl;
   if(dtype == 4)
     cout<<" Value: "<<*( double*)m<<endl;
#endif

   for (int i=0; i<num_var; i++){
       if (strcmp(var_arr[i].name,name)==0){
       //check if variable value had changed. If changed, log and update value
         if(dtype == 1){
            if (*(int*)(var_arr[i].dataptr) != *(int*)m){
              *(int*)(var_arr[i].dataptr)=*(int*)m;
              fprintf(logFile,"  *Variable: %s changed, ",name);
              fprintf(logFile,"new value: %d\n",*(int*)m);
              cout<<" Variable "<<name<<" changed. new value: "<<*(int*)(var_arr[i].dataptr)<<endl;
            }
         } else if(dtype == 2){
            if (*(char *)var_arr[i].dataptr!=*(char*)m){
              *(char *)var_arr[i].dataptr=*(char*)m;
              fprintf(logFile,"  *Variable: %s changed, ",name);
              fprintf(logFile,"new value: %c\n",*(char*)m);
              cout<<" Variable "<<name<<" changed. new value: "<<*(char *)var_arr[i].dataptr<<endl;
           }
         } else if(dtype == 3){
            if ((strcmp((char*)var_arr[i].dataptr,(char*)m)!=0)){
              strcpy((char*)var_arr[i].dataptr,(char*)m);
              fprintf(logFile,"  *Variable: %s changed, ",name);
              fprintf(logFile,"new value: %s\n",(char*)m);
              cout<<" Variable "<<name<<" changed. new value: "<<(char *)var_arr[i].dataptr<<endl;
            }
         } else if(dtype == 4){
            if (*( double*)var_arr[i].dataptr!=*( double*)m){
               *( double*)var_arr[i].dataptr=*( double*)m;
               fprintf(logFile,"  *Variable: %s changed, ",name);
               fprintf(logFile,"new value: %f\n",*( double*)m);
               cout<<" Variable "<<name<<" changed. new value: " <<*( double*)var_arr[i].dataptr<<endl;
            }
         }
         fflush(logFile);
         break;
       }
   } 
}

