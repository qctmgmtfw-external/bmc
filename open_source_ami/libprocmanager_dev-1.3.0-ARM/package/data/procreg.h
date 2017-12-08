/******************************************************************
 ******************************************************************
 ***                                                                                                           **
 ***    (C)Copyright 2010, American Megatrends Inc.                                     **
 ***                                                                                                           **
 ***    All Rights Reserved.                                                                          **
 ***                                                                                                           **
 ***    5555 , Oakbrook Pkwy, Norcross,                                                       **
 ***                                                                                                           **
 ***    Georgia - 30093, USA. Phone-(770)-246-8600.                                  **
 ***                                                                                                           **
 ******************************************************************
 ******************************************************************
 ******************************************************************
 *
 * procreg.h
 * Process Register and Deregister function for process manager
 *
 *  Author: Winston <winstonv@amiindia.co.in>
 ******************************************************************/
#ifndef _H_PROC_REG_
#define _H_PROC_REG_

#include "time.h"



#ifndef PACKED
#define PACKED __attribute__ ((packed))
#endif


#define PROC_ARG 100
#define SLP_ARG 256
#define PROC_EXECV 50

#define PROC_MON "/var/procmon_pipe"
#define TRUE 1
#define FALSE 0
#define PROC_RETRIES 5
#define MAX_BUF_SIZE 256

typedef void (*procsighndlr)(int);


typedef struct
{
    int pid;
    int reg;
    int proctoslp;
    int noofprocargs;
    int noofslpargs;
    int strdslpargs;
    int servstatus;
    time_t tm;
    char procexecv[PROC_EXECV];
    char procarg[PROC_ARG];
    char slparg[SLP_ARG];
}PACKED PROC_MON_T;

extern int ProcMonitorRegister(char *procexecv,void (*handler)(int),int procargcnt,...);
extern int ProcMonitorDeRegister(char *procexecv);
extern  int getpidname(int pid,char *pbuf);

extern int RegisterService (char *procexecv,int port,char *slpargs);


#endif //_H_PROC_REG_

