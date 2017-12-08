/*****************************************************************
 *****************************************************************
 ***                                                            **
 ***    (C)Copyright 2005-2006, American Megatrends Inc.        **
 ***                                                            **
 ***            All Rights Reserved.                            **
 ***                                                            **
 ***        6145-F, Northbelt Parkway, Norcross,                **
 ***                                                            **
 ***        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 ***                                                            **
 *****************************************************************
 *****************************************************************
 ******************************************************************
 *
 * Filename : WS_Util.h
 * 
 * File description : Implementation of WS-Man Specification. 
 *
 *  Author: 	Manish Tomar <manisht@amiindia.co.in>
 *  			Yogeswaran T <yogeswarant@amiindia.co.in>
 ******************************************************************/
#ifndef _WS_UTIL_H_
#define _WS_UTIL_H_
#include <stdarg.h>
#include <string.h>
#include "WS_Include.h"
#include "WS_Structures.h"
#include "WS_Fault.h"
#include "WS_EventingDaemon.h"

static inline void DEBUG_MSG(int level, char *str,...);

static inline void DEBUG_MSG(int level, char *str,...)
{
    int i;
     va_list ap;

    //! Use export DEBUG_WSMAN=CONSOLE to print the messages in the console.
    //! Use export DEBUG_WSMAN=FILE to redirect the messages to the file /tmp/AMISP/wsmanlog.log.
    if(getenv("DEBUG_WSMAN") != NULL)
    {
         va_start(ap, str);
        if(!strcmp(getenv("DEBUG_WSMAN"),"CONSOLE"))
        {
            for(i = 0; i < level; i++)
                printf("\t");
            vfprintf(stderr,str, ap);

        }
        else if(!strcmp(getenv("DEBUG_WSMAN"), "FILE"))
        {
            FILE *logFile;
            logFile = fopen("/tmp/wsmanlog.log","a");
            if(logFile)
                vfprintf(logFile,str, ap);
            fclose(logFile);
        }
        va_end(ap);
    }
}

#define WSMAN_ENTER(funcName) \
do{                             \
    if(funcName)                \
        DEBUG_MSG(0, "%s(%d) - Entering %s\n", __FILE__, __LINE__, funcName); \
}       \
while(0)


#define WSMAN_RETURN(funcName) \
do{                             \
    if(funcName)                \
        DEBUG_MSG(0, "%s(%d) - Leaving  %s\n", __FILE__, __LINE__, funcName); \
}       \
while(0)

#define CHECK_AND_FREE(ptr)	\
do{							\
	if(ptr)					\
	{						\
		free(ptr);			\
		ptr = NULL ;		\
	}						\
}							\
while(0)
	

#define DEFAULT_HTTP_PORT	80
#define UUID_STR_LEN (36 +1)

#define WS_CIM_DATETIME 1
#define WS_INTERVAL 2
#define WS_DATE 3
#define WS_TIME 4
#define WS_DATETIME 5

#define RAND_FILE "/proc/sys/kernel/random/uuid"


extern void bprintf(const char *module, const char* fmt,...);

/* Function to print debug messages */
extern void debug_printf(const char *module, const char *format, ...);

extern WS_INT16 GetHostNameFromURI(const char* URI, char* str, int length);

extern WS_INT16 GetPortNumberFromURI(char *URI);

extern WS_INT16 ToHexChars(const unsigned char* input, int iLen, char* output, int oLen);

extern WS_INT16 CreateStdHttpHeaders(char* URI, int contLen, char* str, int length);

extern int TcpConnect(char* server, int port);

extern WS_INT16 FreeArrOfStrings(char** array, int length);

extern WS_INT16 CopySelectorSet(SelectorSet *dest, const SelectorSet *src);

extern WS_INT16 CopySelectorSetwithEpr(SelectorSet *dest, const SelectorSet *src);

WS_INT16 strStartsWith(char *string, char *startsWith);

WS_INT16 strEndsWith(char *str, char *suff);

WS_INT16 GetAction(char* ActionURI,char *method);

WS_INT16 WSIsInteger(char *value);

WS_INT16 WSIsDuplicate(char **strarr, WS_UINT16 count, char *needle);

WS_INT16 WSSubsSelectorIsDuplicate(SubsSelectorSet select, WS_UINT16 count, char *needle);

WS_INT16 WSGetNS4CIMClient(char *namespaces, char **cimnamespace1, char **cimnamespace2);

char *WSGetSimpleSelectorVal(WSMAN_EPR *epr, char *name);

void FreeSelectorSetContents(SelectorSet selectorSet);

void FreeSelectorSet(SelectorSet *selectorSet);

WS_INT16 CompareWSMANEPRs(WSMAN_EPR *pOrigEPR , WSMAN_EPR *pEPR);

WS_INT16 CompareSelectorSets(SelectorSet *pOrigSelectorSet, SelectorSet *pSelectorSet);

WS_INT16 Free_WSMAN_EPR(WSMAN_EPR *epr);

WS_INT16 Free_void_WSEPR(void *epr);

char *getClassName(char *resourceURI);

WS_INT16 WS_GetDaysAndTime(char *MessageTimeStamp, char *strdays, char *strhours,
			 char *strmins, char *strsecs);

int WS_GetDateAndTime(char *DateTime, char *stryear, char *strmonth, char *strday, 
			char *strhours, char *strmins, char *strsecs);

int WS_GetDaysAndTimeFromInterval(char *pstrInterval, char *strDays, char *strHours, 
			char *strMins, char *strSecs);

int WS_FillMessageID(char *pstrMessageID, int length);

/* Macros to handle memory check */
#define WS_CALLOC_NULL(ptr,n,size) \
		(ptr) = calloc((n) , (size)); \
		if ((ptr) == NULL) { \
			SetFault(WSMAN_INTERNAL_ERROR);\
			SetFaultTextDetail("Could not allocate memory"); \
			fprintf(stderr, "COULD NOT ALLOCATE MEM\n"); return NULL; }

#define WS_CALLOC_FAILURE(ptr,n,size) \
		(ptr) = calloc((n) , (size)); \
		if ((ptr) == NULL) { \
			SetFault(WSMAN_INTERNAL_ERROR);\
            SetFaultTextDetail("Could not allocate memory"); \
			fprintf(stderr, "COULD NOT ALLOCATE MEM\n"); return FAILURE; }
			
#define WS_REALLOC_NULL(ptr,type,newsize) \
		(ptr) = (type)realloc((ptr), (newsize)); \
		if ((ptr) == NULL) { \
			SetFault(WSMAN_INTERNAL_ERROR);\
            SetFaultTextDetail("Could not allocate memory"); \
			fprintf(stderr, "COULD NOT ALLOCATE MEM\n"); return NULL; }

#define WS_REALLOC_FAILURE(ptr,type,newsize) \
		(ptr) = (type) realloc((ptr), (newsize)); \
		if ((ptr) == NULL) { \
			SetFault(WSMAN_INTERNAL_ERROR);\
            SetFaultTextDetail("Could not allocate memory"); \
			fprintf(stderr, "COULD NOT ALLOCATE MEM\n"); return FAILURE; }

#define WS_ASSERT_CALLOC(ptr,n,size) \
		(ptr) = calloc((n) , (size)); \
		if ((ptr) == NULL) { \
			SetFault(WSMAN_INTERNAL_ERROR);\
            SetFaultTextDetail("Could not allocate memory"); \
			fprintf(stderr, "COULD NOT ALLOCATE MEM\n"); goto CLEAN_AND_RETURN; }

#endif
