/*****************************************************************
******************************************************************
***                                                            ***
***        (C)Copyright 2008, American Megatrends Inc.         ***
***                                                            ***
***                    All Rights Reserved                     ***
***                                                            ***
***       5555 Oakbrook Parkway, Norcross, GA 30093, USA       ***
***                                                            ***
***                     Phone 770.246.8600                     ***
***                                                            ***
******************************************************************
******************************************************************
******************************************************************
* 
* Filename: dbgout.c
*
* Description: Implements dbgout platform specific functions
*
******************************************************************/
#include "dbgout.h"


static int gAlreadyInited = 0;
static int gLogAlso       = TDBG_SHOULD_LOG;


#if defined (ICC_IS_APPLICATION)
static char* gPristrings[] =
{
	"EMERGENCY", /* SYSTEM IS UNSUABLE */
	"ALERT",  	 /* ACTION MUST BE TAKEN IMMEDIATELY */
	"CRITICAL",	 /* CRITICAL CONDITIONS */
	"ERROR",     /* ERROR CONDITIONS */
	"WARNING",	 /* WARNING CONDITIONS */
	"NOTICE",	 /* NORMAL BUT SIGNIFICANT CONDITION */
	"INFO",		 /* INFORMATIONAL */
	"DEBUG"		 /* DEBUG-LEVEL MESSAGES */
};
#endif



/**************************************Linux App Debug Functions***************************/
#if defined (ICC_OS_LINUX) && defined(ICC_IS_APPLICATION)

void IDBG_LINUXAPP_DbgInit()
/*@globals internalState, fileSystem, gAlreadyInited@*/
/*@modifies internalState, gAlreadyInited@*/
{
  int facpri,upto;

   if(gAlreadyInited)
    return;
 
   /*@-nullpass@*/
   openlog(NULL,LOG_PID,TDBG_AMI_APP_FACILITY);
   /*@=nullpass@*/

   /*@-unrecog@*/
   facpri = LOG_MAKEPRI(TDBG_AMI_APP_FACILITY,TDBG_MINLOG_LEVEL);
   /*@=unrecog@*/
   upto = LOG_UPTO(facpri);

   (void)setlogmask(upto);

   gAlreadyInited = 1;
}


void IDBG_LINUXAPP_DbgOut(int nLevel,const char* lpszFormat,...)
/*@globals internalState, fileSystem, gAlreadyInited, gPristrings, gLogAlso@*/
/*@modifies internalState, gAlreadyInited@*/
{
    int nBuf;
    int nBufHeader;
    char szBuffer[2048];
    va_list args;

    if(!gAlreadyInited)
        IDBG_LINUXAPP_DbgInit();

    va_start(args, lpszFormat);


    /**Put some leads in the string**/
    nBufHeader = snprintf(szBuffer,sizeof(szBuffer)-1,
                          "[%d %s]",(int)getpid(),gPristrings[nLevel]);
			
    nBuf = vsnprintf(szBuffer+nBufHeader, sizeof(szBuffer)-nBufHeader, lpszFormat, args);
    if(nBuf<0||nBuf>= sizeof(szBuffer)-nBufHeader)
    {
        TCRIT("BUffer overflow");
    }  
  
    fprintf(stderr,"%s\n",szBuffer);

    if(gLogAlso)
    {
        syslog(nLevel,"%s",szBuffer);
    }
    

    va_end(args);
}

void IDBG_LINUXAPP_Runtime_DbgOut(int nLevel,const char* lpszFormat,...)
/*@globals internalState, fileSystem, gAlreadyInited, gPristrings, gLogAlso@*/
/*@modifies internalState, gAlreadyInited@*/
{
    return;
}

void IDBG_LINUXAPP_DbgEnd()
/*@globals gAlreadyInited@*/
/*@modifies gAlreadyInited@*/
{
    gAlreadyInited = 0;
    return;
}


#endif
/**************************************Linux App Debug Functions***************************/




/*************************************Linux Kernel Debug Functions************************/
#if defined (ICC_OS_LINUX) && defined(ICC_IS_DRIVER)

#endif
/*************************************Linux Kernel Debug Functions************************/ 




/************************************WindowsApp Debug Functions***************************/
#if defined (ICC_OS_WINDOWS) && defined(ICC_IS_APPLICATION)

void IDBG_WINAPP_DbgInit()
{
    if(gAlreadyInited)
        return;

    gAlreadyInited = 1;
}

void IDBG_WINAPP_DbgOut(int Level,char* pszFormat,va_list arglist)
{
	char buf[1024];

	if(!gAlreadyInited)
		IDBG_WINAPP_DbgInit();

	sprintf(buf, "[%lu][%s]", GetCurrentThreadId(),gPristrings[Level]);
	vsprintf(&buf[strlen(buf)], pszFormat, arglist);
	OutputDebugString(buf);

	return;
}


void IDBG_WINAPP_DbgInfo(char* pszFormat,...)
{
	va_list arglist;
	va_start(arglist,pszFormat);
	IDBG_WINAPP_DbgOut(LOG_INFO,pszFormat,arglist);
	va_end(arglist);
}

void IDBG_WINAPP_DbgDebug(char* pszFormat,...)
{
	va_list arglist;
	va_start(arglist,pszFormat);
	IDBG_WINAPP_DbgOut(LOG_DEBUG,pszFormat,arglist);
	va_end(arglist);
}


void IDBG_WINAPP_DbgWarn(char* pszFormat,...)
{
	va_list arglist;
	va_start(arglist,pszFormat);
	IDBG_WINAPP_DbgOut(LOG_WARNING,pszFormat,arglist);
	va_end(arglist);
}

void IDBG_WINAPP_DbgCrit(char* pszFormat,...)
{
	va_list arglist;
	va_start(arglist,pszFormat);
	IDBG_WINAPP_DbgOut(LOG_CRIT,pszFormat,arglist);
	va_end(arglist);
}

void IDBG_WINAPP_DbgEmerg(char* pszFormat,...)
{
	va_list arglist;
	va_start(arglist,pszFormat);
	IDBG_WINAPP_DbgOut(LOG_EMERG,pszFormat,arglist);
	va_end(arglist);

}


void IDBG_WINAPP_Flagged(unsigned long uniquename,unsigned long flag,char* pszFormat,...)
{
   	va_list arglist;
	va_start(arglist,pszFormat);
    if((uniquename & flag))
	    IDBG_WINAPP_DbgOut(LOG_DEBUG,pszFormat,arglist);
	va_end(arglist);
}

void IDBG_WINAPP_AssertDbg(int cndn,char* pszFormat,...)
{
    va_list arglist;
	va_start(arglist,pszFormat);
    if((cndn))
	    IDBG_WINAPP_DbgOut(LOG_DEBUG,pszFormat,arglist);
	va_end(arglist);
}

void IDBG_WINAPP_AssertWarn(int cndn,char* pszFormat,...)
{
    va_list arglist;
	va_start(arglist,pszFormat);
    if((cndn))
	    IDBG_WINAPP_DbgOut(LOG_WARNING,pszFormat,arglist);
	va_end(arglist);
}


void IDBG_WINAPP_AssertCrit(int cndn,char* pszFormat,...)
{
    va_list arglist;
	va_start(arglist,pszFormat);
    if((cndn))
	    IDBG_WINAPP_DbgOut(LOG_CRIT,pszFormat,arglist);
	va_end(arglist);
}

void IDBG_WINAPP_AssertEmerg(int cndn,char* pszFormat,...)
{
    va_list arglist;
	va_start(arglist,pszFormat);
    if((cndn))
	    IDBG_WINAPP_DbgOut(LOG_EMERG,pszFormat,arglist);
	va_end(arglist);
}

void IDBG_WINAPP_DbgEnd()
{
    return;
}

#endif
/************************************WindowsApp Debug Functions***************************/



/*********************************WindowsDriver Debug Functions***************************/
/*********************************WindowsDriver Debug Functions***************************/



