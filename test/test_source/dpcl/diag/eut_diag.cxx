/******************************************************************************/
/*                                                                            */
/* This software is subject to the terms of the IBM Public License            */
/* You must accept the terms of this license to use this software.            */
/*                                                                            */
/* Copyright (C) 2001, International Business Machines Corporation            */
/*                                                                            */
/* Further information about the IBM Public License Version 1.0 is obtained   */
/* from URL http://oss.software.ibm.com/developer/opensource/license10.html   */
/*                                                                            */
/******************************************************************************/
#pragma comment(copyright,"5765-543 (C) Copyright IBM Corp. 1998 $Source$ $Id$ $Date$ %Z% %I% %W% %D% %T%")
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*  

    Program: eut_diag
    Program function:
       Trace the function calls and global variables within a source file 
    Usage:
       eut_diag <host name> <pid> <module name>
          OR
       eut_diag <host name> path </full path/executable>

       - specify \"d\" for <host name> to use default host

*/
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
char LOGFILE[128];
#define INTERVAL 4    //interval to check variable values
//#define DEBUG2 
// --------------------------------------------------------------------------
#include <stdio.h>                                                
#include <stdlib.h>                                                
#include <signal.h>

#include <AisGlobal.h>                                                
#include <AisInit.h>
#include <AisMainLoop.h>
#include <Process.h>
#include <InstPoint.h>
#include <ProbeExp.h>
#include <Application.h>
#include <ProbeType.h>
#include <ProbeHandle.h>
#include <InstPoint.h>
#include <SourceObj.h>
#include <AisStatus.h>
#include <LogSystem.h>
#include <AisHandler.h>

#include <iostream.h>
#include <time.h>
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
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

char filename[bufSize+1], buf[bufSize+1];
FILE * logFile;
int num_var=0; 
int lflag=0;   //Ais_main_loop() started flag
int cflag=0;   //create process flag

void stdout_cb(GCBSysType sys, GCBTagType tag, GCBObjType obj, GCBMsgType msg);
void stderr_cb(GCBSysType sys, GCBTagType tag, GCBObjType obj, GCBMsgType msg);
int sighandler(int signal);

void set_func_probe(Process&,InstPoint,char*,char*);
void func_cb(GCBSysType,GCBTagType,GCBObjType,GCBMsgType);
void Initialize(char **argv);
int get_data_type(ProbeExp sor);
void print_sts(AisStatus rc, char *fcnname); 
void dcall_back(GCBSysType s,GCBTagType t,GCBObjType o,GCBMsgType m);
void dcall_back2(GCBSysType s,GCBTagType t,GCBObjType o,GCBMsgType m);
void ck_vars(void);
void log_vars(void);

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
main(int argc, char *argv[]) 
{
    
    if (argc < 4){
      cout<<"\n**Incorrect numbers of argument entered**"<<USAGE1<<
 		USAGE3<<USAGE5<<endl;
      exit(0);
    }

    Ais_initialize();
    Ais_add_signal(SIGINT,sighandler);
    Ais_add_signal(SIGALRM,sighandler);
#if (0)
    Ais_blog_on("hope.americas.sgi.com", LGL_detail, LGD_daemon, 0,0);
#endif

    Initialize(argv);
 
    SourceObj myprog = P.get_program_object();
    SourceObj mymod;
    char        bufmname[bufSize+1];      // buffer for module_name(..)

    char name[bufSize+1];
    int found=0;
    
    while(found==0)
    {
      cout<<"enter the source file name to trace function calls"<<endl;
      cout<<"type !Help to list source files"<<endl;

      cin>>filename;

      if(strcmp(filename,"!Help")==0)
      {
        //  loop through source files to list
        cout<<"Source files:"<<endl;
        int count=myprog.child_count();
        for(int x=0;x<count;x++)
        {
          mymod=myprog.child(x);
          mymod.module_name(name,bufSize);
          cout<<x<<" "<<name<<endl;
        }
      } else {
        for (int c = 0; c < myprog.child_count(); c++)
        {
          mymod = myprog.child(c);
          const char * modname = mymod.module_name(bufmname,bufSize);

	  if (strcmp(modname, filename) ==0) 
          {
            printf("Module %s found... expanding\n",filename);
	    AisStatus sts = mymod.bexpand(P);
            if (sts.status() != ASC_success){
              printf("bexpand() was not successful.. %s\n",sts.status_name());
              exit(0);
            } else {
              printf("bexpand() was successful.\n");
            }
            found=1;
	    break;
	  } 
        }
        if (found == 0) 
           cout<<"\""<<filename<<"\" not found.\n   re";  
      }  //end for                      
    } //end while  
    
    cout<<"Enter the name of file that you wish to log information in."<<endl;
    cin>>LOGFILE;
    logFile=fopen(LOGFILE,"w");
    fprintf(logFile,"Source file \"%s\":\n",filename);   
    fprintf(logFile,"(Function and variable tracing)\n\n");

    SourceObj myobj;
    char        bufdname[bufSize];   // buffer for get_demangled_name(..)
    

    int child_count=mymod.child_count();
    
    printf("\n");

    for ( int c = 0; c < child_count; c++)
    {
     	myobj = mymod.child(c);
        srctyp = myobj.src_type();
        if (srctyp == SOT_data) {
            num_var++;
        }
    }

    vobj_arr = new SourceObj[num_var];
    var_arr = new var_info[num_var];
    send_pes = new ProbeExp[num_var];
    int i1=0;

    for (int c = 0; c < child_count; c++)
    {
     	myobj = mymod.child(c);
        srctyp = myobj.src_type();

        if (srctyp == SOT_function) {

	  char * funname = myobj.get_demangled_name(bufdname,bufSize);

          if (funname == NULL) continue;

          cout<<"function \""<<funname<<"\" found"<<endl;

          InstPoint  mypoint;

          for ( int d = 0; d < myobj.exclusive_point_count(); d++)
          {
             mypoint = myobj.exclusive_point(d);

            if ( mypoint.get_type() == IPT_function_entry)
            {
               set_func_probe(P,mypoint,funname," entry");
            } else if ( mypoint.get_type() == IPT_function_exit)
            {
               set_func_probe(P,mypoint,funname,"exit");
            }
          }  //end for exclusive point count 
      } else if (srctyp == SOT_data) {
#if (1)

            printf("SOTDATA FOUND!\n");
            vobj_arr[i1]=myobj;

            ProbeExp parms[3],send,sor;
            char name[bufSize],passit[bufSize];
            int dtype;
            sor = myobj.reference();

            dtype = get_data_type(sor);
            var_arr[i1].dtype=dtype;
            printf("DataType %d\n", dtype);
      
            myobj.get_variable_name(name,bufSize);

            cout<<"variable \""<<name<<"\" found"<<endl;
            
            printf("variable_name %s\n", name);
            
            strcpy(var_arr[i1].name,name);


            if ((dtype>0) && (dtype<5) && dtype !=3){  
              sprintf(passit,"%d%s",dtype,name);
              parms[0] = Ais_msg_handle;
              if (dtype == 3)
                parms[1] = sor;
              else
                parms[1] = sor.address();

              if (dtype==1) {
                parms[2] = ProbeExp(sizeof(int));
                var_arr[i1].dataptr=new int;
              }else if (dtype==4) {
                parms[2] = ProbeExp(sizeof( double));
                var_arr[i1].dataptr=new  double;
              }else if (dtype==2){
                parms[2] = ProbeExp(sizeof(char));
                var_arr[i1].dataptr=new char;
              }else { 
                parms[2] = ProbeExp(bufSize);
                var_arr[i1].dataptr=new char[bufSize];
             }

              send = Ais_send.call(3,parms);
              GCBFuncType dcb = dcall_back;

              AisStatus sts = P.bexecute(send, dcb, (GCBTagType)passit);
              if (sts.status() != ASC_success)
                print_sts(sts, "bexecute");
              i1++;
            } else {
              printf("..variable type not supported dtype %d\n", dtype);
              i1++;
            }
#endif
         
      }   // end SOT_data
    }    // end for child_count


    if(strcmp(argv[2],"path")==0){
      AisStatus sts=P.bstart();
      if (sts.status()==ASC_success){
        printf("bstart() was successful\n");
      } else {
        printf("bstart() FAILED.. %s\n",sts.status_name());
        exit(0);
      }
    } 
    else if(strcmp(argv[2],"pid")==0){
      AisStatus sts=P.bresume();
      if (sts.status()==ASC_success){
        printf("bresume() was successful\n");
      } else {
        printf("bresume() FAILED.. %s\n",sts.status_name());
        exit(0);
      }
    }
  
    printf("*Global variables are checked every %d seconds\n",INTERVAL);
    printf("*To monitor, enter \"tail -f %s\"\n",LOGFILE);
    printf("*Enter <CTRL>-c to exit*\n");
    sleep(3);

    lflag=1;
    alarm(INTERVAL);
    Ais_main_loop();

    if(strcmp(argv[2],"path")==0){
      AisStatus sts=P.battach();
      if (sts.status()==ASC_success){
        printf("battach() was successful\n");
      } else {
        printf("battach() FAILED.. %s\n",sts.status_name());
        exit(0);
      }
      sts=P.bdestroy();
      if (sts.status()==ASC_success){
        printf("bdestroy() was successful\n");
      } else {
        printf("bdestroy() FAILED.. %s\n",sts.status_name());
        exit(0);
      }
    } 

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void Initialize(char **argv){
    char **argvv=argv+3;
    char        hostname[bufSize+1];      // buffer for get_host_name()

    if(strcmp(argv[1],"d")==0){
      gethostname(hostname,bufSize);
      printf("  *Running on \"%s\"\n",hostname);
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
      AisStatus sts = P.bcreate(hostname,argv[3],argvv,NULL,
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

    printf("Using Process %d",P.get_pid());
    P.get_host_name(hostname,bufSize);
    printf(" on host %s...\n",hostname);
}

int sighandler(int s){

  if (s == SIGINT){
    if (lflag == 1){
      Ais_end_main_loop();
      if (cflag==0) log_vars();
      printf("*Please check \"%s\" for the result.\n",LOGFILE);
DONE = 1;
    } else
      exit(0);
  } else if (s == SIGALRM){
     if (lflag){
if (DONE == 1) {
  //printf("DONE is 1\n");
  //fflush(stdout);
}
else {
  //printf("DONE is 0\n");
  //fflush(stdout);
         ck_vars();
         alarm(INTERVAL);
}
     }
  }

    return(0);
}

void
stdout_cb(GCBSysType sys, GCBTagType tag, GCBObjType obj, GCBMsgType msg) {
    //  callback to receive standard out from application and display

    char *p = (char *) msg;
    printf("stdout_cb: ");
    for (int i=0; i<sys.msg_size; i++) {
        printf("%c",*p);
        p++;
    }
    printf("\n");
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


void set_func_probe(Process &P,InstPoint ip,char *name,char *location)
{
 //  Installs probe at instrumentation point

 AisStatus sts;
 cout<<"  Inserting PROBE: \""<<name<<"\" "<<location<<endl;
#if (1)
 int length=strlen(name)+1;
 ProbeExp parms[3],send;

 // build probe expression
 parms[0]=Ais_msg_handle;
 parms[1]=ProbeExp(name);
 parms[2]=ProbeExp(length);

 // build probe to send message to client

 send=Ais_send.call(3,parms);
// define callback for information returning from probe

 
 ProbeHandle myph;
 GCBFuncType cb[1];
 cb[0]=func_cb;
 GCBTagType  tg=(GCBTagType)location;

 //   install the probe
 sts=P.binstall_probe(1,&send,&ip,cb,&tg,&myph);
 if (sts.status() != ASC_success) {
   cout<<"Install Status: "<<sts.status_name()<<endl;
 }

 // activate the probe
 sts=P.bactivate_probe(1,&myph);
 if (sts.status() != ASC_success) {
   cout<<"Activate Status: "<<sts.status_name()<<endl;
 }

#endif
}

void func_cb(GCBSysType sys,GCBTagType tag,GCBObjType obj,GCBMsgType msg) {
  //   callback to receive messages from probes installed on functions 

  char *name,*location;
 
  Process *p = (Process *) obj;
  name=(char*)msg;
  location=(char*)tag;

  fprintf(logFile,"\"%s\" - %s\n ",name,location);
  fflush(logFile);
  //ck_vars();

}

int get_data_type(ProbeExp sor)
{
        DataExpNodeType pt = (sor.get_data_type()).get_node_type();
        int ret_val=0;
        switch(pt)
        {
          case DEN_int64_type:  //4 byte integer
                                ret_val = 1;
                                break;
          case DEN_int8_type:  //character
                                ret_val = 2;
                                break;
          case DEN_array_type:  //string
                                ret_val = 3;
                                break;
          case DEN_float64_type:  //character
                                ret_val = 4;
                                break;
        }
        return ret_val;
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
   if (complete == num_var) log_vars();
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

void ck_vars(void){

  ProbeExp parms[3],sor,send;
  int dtype;
  parms[0] = Ais_msg_handle;
  char passit[bufSize], name[bufSize+1];
  SourceObj myobj;

  printf("CK_VARS enter\n");
#if (1)
  GCBFuncType dcb = dcall_back2;
  for (int i=0; i < num_var; i++){

  myobj=vobj_arr[i];   
  sor = myobj.reference();
      
  myobj.get_variable_name(name,bufSize);

  dtype = get_data_type(sor);

  printf("CK_VARS variable_name %s dtype %d\n", name, dtype);
  if ((dtype>0) && (dtype<5) && dtype !=3){  
     sprintf(passit,"%d%s",dtype,name);
     if (dtype == 3)
       parms[1] = sor;
     else
       parms[1] = sor.address();

     if (dtype==1) {
       parms[2] = ProbeExp(sizeof(int));
     }else if (dtype==4) {
       parms[2] = ProbeExp(sizeof( double));
     }else if (dtype==2){
       parms[2] = ProbeExp(sizeof(char));
     }else { 
       parms[2] = ProbeExp(bufSize);
     }

     send = Ais_send.call(3,parms);

     printf("CK_VARS bexecute passit %s\n", passit);
     AisStatus sts = P.bexecute(send, dcb, (GCBTagType)passit);
     if (sts.status() != ASC_success){
           print_sts(sts, "bexecute");
           if (sts == ASC_terminated_pid){
              cout<<"Exiting..\n";
              cout<<"*Please check \""<<LOGFILE<<"\" for the result.\n";
              exit(0);
           }
     }
     }
  }
#endif
}

void log_vars(void){
   char name[bufSize+1];
   int dtype;
 
   for (int i=0; i<num_var; i++){
       strcpy(name,var_arr[i].name);
       fprintf(logFile,"  *Variable: %s ",name);
       dtype=var_arr[i].dtype;
       if (dtype == 1){
          fprintf(logFile,"Value: %d\n",*(int*)var_arr[i].dataptr);
       } 
       else if (dtype == 2){
          fprintf(logFile,"Value: %c\n",*(char*)var_arr[i].dataptr);
       }
       else if (dtype == 3){
         fprintf(logFile,"Value: %s\n",(char*)var_arr[i].dataptr);
       }
       else if (dtype == 4){
          fprintf(logFile,"Value: %f\n",*( double*)var_arr[i].dataptr);
       }
       else {
          fprintf(logFile,"\n");
       }
       fflush(logFile);
   } 
}
