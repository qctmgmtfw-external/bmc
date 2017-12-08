/*
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2006-2009, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 */

/**
 * @file   dbgout.h
 * @author Subash Kalbarga
 * @date   Wed May 21 10:57:03 2003
 *
 * @brief  Contains macro definitions for cross platform usage
 *
 */

#ifndef __TDBGOUT_H__
#define __TDBGOUT_H__



#include "icc_what.h"
ICC_CPP_EXTERN_C_BEGIN


/***********************Common defines for dbgout**************************/
/**
 * Facility used for logging AMI application messages in syslog
 *
 */
#define TDBG_AMI_APP_FACILITY ((LOG_LOCAL0))

/***********************Common defines for dbgout**************************/

/******************AUDIT TRAIL*************************/
#define TAUDIT_LOG_FACILITY     ((LOG_LOCAL1))


/***********************Common controls***********************************/
#ifndef TDBG_NOFILEINFO
#define TDBG_FILE_LINE_FMT "[%s:%d]"
#define TDBG_FILENAME  __FILE__
#define TDBG_LINENO     __LINE__
#else
#define TDBG_FILE_LINE_FMT "%s%s"
#define TDBG_FILENAME  ""
#define TDBG_LINENO ""
#endif

/***********************Common controls***********************************/



/****************************Linux Application*****************************/
#if ( defined (ICC_OS_LINUX) || defined (ICC_OS_MACH)) && defined (ICC_IS_APPLICATION)
#include <stdarg.h>
#include <stdio.h>
#include <syslog.h>
#include <sys/types.h>
#include <unistd.h>


/**Prototypes**/

/*@unused@*//*@external@*/
extern void IDBG_LINUXAPP_DbgInit( void )
/*@globals fileSystem, internalState@*/
/*@modifies fileSystem, internalState@*/;

/*@unused@*//*@external@*/
extern void IDBG_LINUXAPP_DbgOut( int nLevel, char *lpszFormat, ... )
/*@globals fileSystem, internalState@*/
/*@modifies fileSystem, internalState@*/;

/*@unused@*//*@external@*/
extern void IDBG_LINUXAPP_DbgEnd( void )
/*@globals internalState@*/
/*@modifies internalState@*/;

/*Function to log the debug messages into syslog without printing it to the console*/
extern void IDBG_LINUXAPP_DbgLog( int nLevel, char *lpszFormat, ... );
/*Function to log the buffer of particular length without printing it to the console*/
extern void IDBG_LINUXAPP_DbgLogBuf(int nLevel, char *FileName, int line, char* pbuf, int len);
extern void IDBG_LINUXAPP_AuditInit( void );
extern void IDBG_LINUXAPP_AuditOut( int nLevel, char *lpszFormat, ... );
extern void IDBG_LINUXAPP_AuditEnd( void );



#define PLATFORM_DBGINITFN		          IDBG_LINUXAPP_DbgInit
#define PLATFORM_DBGOUTFN(n,fmt,args...)  IDBG_LINUXAPP_DbgOut(TDBG_AMI_APP_FACILITY | n,fmt,##args)
#define PLATFORM_DBGLOGFN(n,fmt,args...)  IDBG_LINUXAPP_DbgLog(TDBG_AMI_APP_FACILITY | n,fmt,##args)
#define PLATFORM_DBGLOGBUFFN(n,TDBG_FILENAME,TDBG_LINENO,BUF,LEN)   IDBG_LINUXAPP_DbgLogBuf(TDBG_AMI_APP_FACILITY | n, TDBG_FILENAME,TDBG_LINENO, BUF,LEN)
#define PLATFORM_DBGENDFN		          IDBG_LINUXAPP_DbgEnd

#define PLATFORM_AUDITINITFN		        IDBG_LINUXAPP_AuditInit
#define PLATFORM_AUDITOUTFN(n,fmt,args...)  IDBG_LINUXAPP_AuditOut(TAUDIT_LOG_FACILITY | n,fmt,##args)
#define PLATFORM_AUDITENDFN		            IDBG_LINUXAPP_AuditEnd



#define TDBG_SHOULD_LOG    1
#define TDBG_MINLOG_LEVEL  LOG_WARNING

#endif /*****Linux Application****/





/*****************************Linux Kernel*********************************/
#if ( defined (ICC_OS_LINUX) || defined (ICC_OS_MACH)) && defined (ICC_IS_DRIVER)


#define LOG_DEBUG     KERN_DEBUG
#define LOG_INFO      KERN_INFO
#define LOG_WARNING   KERN_WARNING
#define LOG_CRIT      KERN_CRIT
#define LOG_EMERG     KERN_EMERG

#define STR_LOG_DEBUG    " [KERNDEBUG] "
#define STR_LOG_INFO     " [KERNINFO] "
#define STR_LOG_WARNING  " [KERNWARNING] "
#define STR_LOG_CRIT     " [KERNCRITICAL] "
#define STR_LOG_EMERG    " [KERNEMERGENCY] "


#define PLATFORM_DBGINITFN do { } /*@-predboolptr@*/while (0)/*@=predboolptr@*/;

#define PLATFORM_DBGOUTFN(n,fmt,args...) printk(n STR_##n fmt,##args)

#define PLATFORM_DBGEDNFN  do { } /*@-predboolptr@*/while (0)/*@=predboolptr@*/;


#define PLATFORM_AUDITINITFN do { } /*@-predboolptr@*/while (0)/*@=predboolptr@*/;

#define PLATFORM_AUDITOUTFN(n,fmt,args...) printk(n STR_##n fmt,##args)

#define PLATFORM_AUDITEDNFN  do { } /*@-predboolptr@*/while (0)/*@=predboolptr@*/;


#endif /*****Linux Kernel****/






/****************************Windows Application*****************************/

#if defined (ICC_OS_WINDOWS) && defined (ICC_IS_APPLICATION)

#include <windows.h>
#include <stdio.h>

#define LOG_DEBUG        7
#define LOG_INFO         6
#define LOG_WARNING      4
#define LOG_CRIT         2
#define LOG_EMERG        0


/**Prototypes**/
void IDBG_WINAPP_DbgInit();
void IDBG_WINAPP_DbgOut(int Level,char* pszFormat,va_list arglist);
void IDBG_WINAPP_DbgInfo(char* pszFormat,...);
void IDBG_WINAPP_DbgDebug(char* pszFormat,...);
void IDBG_WINAPP_DbgWarn(char* pszFormat,...);
void IDBG_WINAPP_DbgCrit(char* pszFormat,...);
void IDBG_WINAPP_DbgEmerg(char* pszFormat,...);

void IDBG_WINAPP_Flagged(unsigned long uniquename,unsigned long flag,char* pszFormat,...);

void IDBG_WINAPP_AssertDbg(int cndn,char* pszFormat,...);
void IDBG_WINAPP_AssertWarn(int cndn,char* pszFormat,...);
void IDBG_WINAPP_AssertCrit(int cndn,char* pszFormat,...);
void IDBG_WINAPP_AssertEmerg(int cndn,char* pszFormat,...);

void IDBG_WINAPP_DbgEnd();

#define PLATFORM_DBGINITFN		IDBG_WINAPP_DbgInit
#define PLATFORM_DBGOUTFN		IDBG_WINAPP_DbgOut
#define PLATFORM_DBGINFOFN		IDBG_WINAPP_DbgInfo
#define PLATFORM_DBGDEBUGFN		IDBG_WINAPP_DbgDebug
#define PLATFORM_DBGWARNFN		IDBG_WINAPP_DbgWarn
#define PLATFORM_DBGCRITFN		IDBG_WINAPP_DbgCrit
#define PLATFORM_DBGEMERGFN		IDBG_WINAPP_DbgEmerg
#define PLATFORM_DBGENDFN		IDBG_WINAPP_DbgEnd



#define TDBG_SHOULD_LOG   0
#define TDBG_MINLOG_LEVEL LOG_WARNING

#endif /********Windows Application******/


/************************Actual Debug Macros******************/

#if defined (ICC_DEBUG) /*is debug mode*/

#if defined (ICC_OS_LINUX)  || defined (ICC_OS_MACH) /*linux or mach takes variable arg macros*/

#if defined (ICC_IS_APPLICATION)		/* Fix for Farday compiler */
#define TDBG_INIT  PLATFORM_DBGINITFN
#define TDBG(fmt,args...)   PLATFORM_DBGOUTFN(LOG_DEBUG,TDBG_FILE_LINE_FMT fmt,TDBG_FILENAME,TDBG_LINENO,##args)
#define TINFO(fmt,args...)  PLATFORM_DBGOUTFN(LOG_INFO,fmt,##args)
#define TWARN(fmt,args...)  PLATFORM_DBGOUTFN(LOG_WARNING,TDBG_FILE_LINE_FMT fmt,TDBG_FILENAME,TDBG_LINENO,##args)
#define TCRIT(fmt,args...)  PLATFORM_DBGOUTFN(LOG_CRIT,TDBG_FILE_LINE_FMT fmt,TDBG_FILENAME,TDBG_LINENO,##args)
#define TEMERG(fmt,args...) PLATFORM_DBGOUTFN(LOG_EMERG,TDBG_FILE_LINE_FMT fmt,TDBG_FILENAME,TDBG_LINENO,##args)
#define TDBG_END            PLATFORM_DBGENDFN
#else
#define TDBG_INIT  PLATFORM_DBGINITFN
#define TDBG(fmt,args...)   PLATFORM_DBGOUTFN(LOG_DEBUG,fmt,##args)
#define TINFO(fmt,args...)  PLATFORM_DBGOUTFN(LOG_INFO,fmt,##args)
#define TWARN(fmt,args...)  PLATFORM_DBGOUTFN(LOG_WARNING,fmt,##args)
#define TCRIT(fmt,args...)  PLATFORM_DBGOUTFN(LOG_CRIT,fmt,##args)
#define TEMERG(fmt,args...) PLATFORM_DBGOUTFN(LOG_EMERG,fmt,##args)
#define TDBG_END            PLATFORM_DBGENDFN
#endif

#elif defined (ICC_OS_WINDOWS)

#define TDBG_INIT       PLATFORM_DBGINITFN
#define TDBG            PLATFORM_DBGDEBUGFN
#define TINFO           PLATFORM_DBGINFOFN
#define TWARN           PLATFORM_DBGWARNFN
#define TCRIT           PLATFORM_DBGCRITFN
#define TEMERG          PLATFORM_DBGEMERGFN
#define TDBG_END        PLATFORM_DBGENDFN

#endif /**Windows or Linux**/


#else /********Is not debug mode***********/

#if defined (ICC_OS_LINUX) || defined (ICC_OS_MACH)

#define TDBG_INIT
#define TDBG(x...)                   /*@-noeffect@*//*@-predboolptr@*/do {} while(0);/*@=predboolptr@*//*@=noeffect@*/
#ifdef LIBCURI_LIB
	#define TINFO(fmt,args...)           TDBG(x...)
	#define TWARN(fmt,args...)           TDBG(x...)
	#define TCRIT(fmt,args...)           TDBG(x...)
	#define TEMERG(fmt,args...)          TDBG(x...)
#else
	#define TINFO(fmt,args...)           PLATFORM_DBGOUTFN(LOG_INFO,fmt,##args)
	#define TWARN(fmt,args...)           PLATFORM_DBGOUTFN(LOG_WARNING,TDBG_FILE_LINE_FMT fmt,TDBG_FILENAME,TDBG_LINENO,##args)
	#define TCRIT(fmt,args...)           PLATFORM_DBGOUTFN(LOG_CRIT,TDBG_FILE_LINE_FMT fmt,TDBG_FILENAME,TDBG_LINENO,##args)
	#define TEMERG(fmt,args...)          PLATFORM_DBGOUTFN(LOG_EMERG,TDBG_FILE_LINE_FMT fmt,TDBG_FILENAME,TDBG_LINENO,##args)
#endif
#define TDBG_END

#elif defined (ICC_OS_WINDOWS)

#define TDBG_INIT                    PLATFORM_DBGINITFN
#define TDBG                         do {} while(0);
#ifdef LIBCURI_LIB
	#define TINFO                        TDBG
	#define TWARN                        TDBG
	#define TCRIT                        TDBG
	#define TEMERG                       TDBG
#else
	#define TINFO                        PLATFORM_DBGINFOFN
	#define TWARN                        PLATFORM_DBGWARNFN
	#define TCRIT                        PLATFORM_DBGCRITFN
	#define TEMERG                       PLATFORM_DBGEMERGFN
#endif
#define TDBG_END                     PLATFORM_DBGENDFN

#endif

#endif /****is it debug mode*******/

//AUDIT is available no matter what the mode - debug or non debug
#ifdef ICC_OS_WINDOWS
// vc++ compiler is not accepting variable list parameter in macros.
#define     TAUDIT(x)
#else
#define     TAUDIT(level,fmt,args...)              PLATFORM_AUDITOUTFN(level, fmt, ##args)
#endif

/************************Flags based controls for Debug Out***********************/

/**Credit : Much of the credit for th flagged control stuuf goes to LarryS
and icc_dbg.h. I have just adapted it and slightly simplified it to make life easier
for those who dont want to use flags**/



/********Internal****************/
#if defined (ICC_OS_LINUX) || defined (ICC_OS_MACH)

#define TDBG_FORM_VAR_NAME(modulename)  g_##modulename##_DebugLevel

#elif defined(ICC_OS_WINDOWS)

#define TDBG_FORM_VAR_NAME(modulename)  modulename

#endif

#define TDBG_DECLARE_DBGVAR_INTERNAL(storage,modulename) storage unsigned long TDBG_FORM_VAR_NAME(modulename) = 0;
#define emptystorage

/********Internal****************/



#define TDBG_DECLARE_DBGVAR(modulename)        TDBG_DECLARE_DBGVAR_INTERNAL(emptystorage,modulename);
#define TDBG_DECLARE_DBGVAR_STATIC(modulename) TDBG_DECLARE_DBGVAR_INTERNAL(static,modulename);
//#define TDBG_DECLARE_DBGVAR_EXTERN(modulename) TDBG_DECLARE_DBGVAR_INTERNAL(extern,modulename);
#define TDBG_DECLARE_DBGVAR_EXTERN(modulename) extern unsigned long TDBG_FORM_VAR_NAME(modulename);



#define TDBG_DEFINE_FLAG_BIT(n) ((0x00000001L) << (n))

#define TDBG_DBGFLAG_ON(modulename,flag) ((TDBG_FORM_VAR_NAME(modulename)) |= (flag));
#define TDBG_DBGFLAG_OFF(modulename,flag) ((TDBG_FORM_VAR_NAME(modulename)) &= ~(flag));

#define TDBG_DBGFLAG_ON_ALL(modulename)  ((TDBG_FORM_VAR_NAME(modulename)) = (0xFFFFFFFF));
#define TDBG_DBGFLAG_OFF_ALL(modulename) ((TDBG_FORM_VAR_NAME(modulename)) = (0x00000000));

#define TDBG_IS_DBGFLAG_SET(modulename,flag) ( (TDBG_FORM_VAR_NAME(modulename)) & (flag) )

#if defined (ICC_OS_LINUX) || defined (ICC_OS_MACH)

#define TDBG_FLAGGED(modulename,flag,fmt,args...) {if( TDBG_IS_DBGFLAG_SET(modulename,flag)) TDBG(fmt,##args);};

#elif defined (ICC_OS_WINDOWS)

#define TDBG_FLAGGED IDBG_WINAPP_Flagged

#endif
/************************Flags based controls for Debug Out***********************/


/***********************************Assert macros*********************************/
#define TDBG_ASSERTION_FAIL_STR "ASSERTION FAIL:"

#if defined (ICC_OS_LINUX) || defined (ICC_OS_MACH)

#define TEMERG_ASSERT(cndn,fmt,args...) {if(!(cndn)) TEMERG(TDBG_ASSERTION_FAIL_STR fmt,##args);}
#define TCRIT_ASSERT(cndn,fmt,args...)  {if(!(cndn)) TCRIT(TDBG_ASSERTION_FAIL_STR fmt,##args);}
#define TWARN_ASSERT(cndn,fmt,args...)  {if(!(cndn)) TWARN(TDBG_ASSERTION_FAIL_STR fmt,##args);}
#define TDBG_ASSERT(cndn,fmt,args...)   {if(!(cndn)) TDBG(TDBG_ASSERTION_FAIL_STR fmt,##args);}

#elif defined (ICC_OS_WINDOWS)

#define TEMERG_ASSERT           IDBG_WINAPP_AssertEmerg
#define TCRIT_ASSERT            IDBG_WINAPP_AssertCrit
#define TWARN_ASSERT            IDBG_WINAPP_AssertWarn
#define TDBG_ASSERT             IDBG_WINAPP_AssertDbg

#endif
/***********************************Assert macros*********************************/



ICC_CPP_EXTERN_C_END
#endif /*********DBGOUT_H**********/
