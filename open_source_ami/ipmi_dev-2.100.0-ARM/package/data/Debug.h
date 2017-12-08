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
 * debug.h
 * Debug Mode Macros
 *
 *  Author: Govind Kothandapani <govindk@ami.com>
 *
 ******************************************************************/
#ifndef DEBUG_H
#define DEBUG_h

#include "Types.h"
#include "dbgout.h"

#define PLATFORM_BREAK  while (1)

#ifdef STHU
#define IPMI_INFO(STR, Args...)
#define IPMI_WARNING(STR, Args...)
#define IPMI_ERROR(STR, Args... )
#else
#define IPMI_INFO(STR, Args...)                         TINFO (STR, ##Args);
#define IPMI_WARNING(STR, Args...)                      TWARN (STR, ##Args);
#define IPMI_ERROR(STR, Args... )                       TCRIT (STR, ##Args);
#endif

#ifdef _DEBUG_

extern void    print_buf (INT8U* Buf, INT16U Len);

#if ENABLE_DEBUG_MACROS==0

/*** Turn off all debug macros if they are disabled in the source ***/
#define IPMI_CHECK_POINT(ARG)
#define IPMI_DBG_BREAK
#define IPMI_ASSERT(ARG)
#define IPMI_ASSERT_CONTINUE(ARG)
#define IPMI_DBG_PRINT_STRUCT_3(STRUCT, M1, M2, M3)
#define IPMI_DBG_PRINT_STRUCT_4(STRUCT, M1, M2, M3, M4)
#define IPMI_DBG_PRINT_STRUCT_5(STRUCT, M1, M2, M3, M4, M5)
#define IPMI_DBG_PRINT_STRUCT_6(STRUCT, M1, M2, M3, M4, M5, M6)

#define PRINT_BUF(BUF, LEN)

#if ENABLE_DEBUG_LOG==0
#define IPMI_DBG_PRINT(STR)
#define IPMI_DBG_PRINT_1(FMT, ARG2)
#define IPMI_DBG_PRINT_2(FMT, ARG1, ARG2)
#define IPMI_DBG_PRINT_3(FMT, ARG1, ARG2, ARG3)
#define IPMI_DBG_PRINT_4(FMT, ARG1, ARG2, ARG3, ARG4)
#define IPMI_DBG_PRINT_BUF(BUF, LEN)

#else		//ENABLE_DEBUG_LOG
#define IPMI_DBG_PRINT(FMT)            	PLATFORM_DBGLOGFN(LOG_DEBUG,TDBG_FILE_LINE_FMT FMT,TDBG_FILENAME,TDBG_LINENO)
#define IPMI_DBG_PRINT_1(fmt, args...) 	PLATFORM_DBGLOGFN(LOG_DEBUG,TDBG_FILE_LINE_FMT fmt,TDBG_FILENAME,TDBG_LINENO,##args)
#define IPMI_DBG_PRINT_2(fmt, args...) 	PLATFORM_DBGLOGFN(LOG_DEBUG,TDBG_FILE_LINE_FMT fmt,TDBG_FILENAME,TDBG_LINENO,##args)
#define IPMI_DBG_PRINT_3(fmt, args...) 	PLATFORM_DBGLOGFN(LOG_DEBUG,TDBG_FILE_LINE_FMT fmt,TDBG_FILENAME,TDBG_LINENO,##args)
#define IPMI_DBG_PRINT_4(fmt, args...) 	PLATFORM_DBGLOGFN(LOG_DEBUG,TDBG_FILE_LINE_FMT fmt,TDBG_FILENAME,TDBG_LINENO,##args)
#define IPMI_DBG_PRINT_BUF(BUF, LEN) 	PLATFORM_DBGLOGBUFFN(LOG_DEBUG, TDBG_FILENAME,TDBG_LINENO, BUF, LEN)
#endif

#else	/* ENABLE_DEBUG_MACROS */

/**
 * @def IPMI_DBG_BREAK
 * @brief Stops the debugger.
**/
#define IPMI_DBG_BREAK	PLATFORM_BREAK

/**
 * @def IPMI_ASSERT
 * @brief Asserts for a condition to be true. If the condition fails program
 * stops executing and the debugger is invoked.
**/
#define IPMI_ASSERT(ARG)									\
 do                                                         \
 {                                                          \
    if (!(ARG))                                             \
    {                                                       \
        IPMI_WARNING ("Assertion failed at %s line %d\n",  \
                        (_FAR_ char *)__FILE__, __LINE__);  \
        IPMI_DBG_BREAK;                                     \
    }                                                       \
                                                            \
 } while (0)
 
 /**
  * @def IPMI_ASSERT_CONTINUE
  * @brief Asserts for a condition to be true. If the condition fails program
  * continues executing.
 **/
#define IPMI_ASSERT_CONTINUE(ARG)									\
do														 \
{ 														 \
 if (!(ARG))											 \
 {														 \
	 IPMI_WARNING ("Assertion failed at %s line %d\n",  \
					 (_FAR_ char *)__FILE__, __LINE__);  \
	 IPMI_WARNING ("Assertion will be handled later...\n");  		\
 }														 \
														 \
} while (0)

#if ENABLE_DEBUG_LOG==0
/**
 * @def IPMI_DBG_PRINT
 * @brief A regular printf style debug printing macro. Could take more
 * ROM space due to the strings to be stored.
**/
#define IPMI_DBG_PRINT(STR)	                            printf (STR)
#define IPMI_DBG_PRINT_1(FMT, ARG1)	                    printf (FMT, ARG1)
#define IPMI_DBG_PRINT_2(FMT, ARG1, ARG2)               printf (FMT, ARG1, ARG2)
#define IPMI_DBG_PRINT_3(FMT, ARG1, ARG2, ARG3)         printf (FMT, ARG1, ARG2, ARG3)
#define IPMI_DBG_PRINT_4(FMT, ARG1, ARG2, ARG3, ARG4)   printf (FMT, ARG1, ARG2, ARG3, ARG4)

/**
 * @def IPMI_DBG_PRINT_BUF
 * @brief Prints the buffer.
**/
#define IPMI_DBG_PRINT_BUF(BUF, LEN)                    print_buf (BUF, LEN);
#define PRINT_BUF(BUF, LEN)                             print_buf (BUF, LEN);

#else	//ENABLE_DEBUG_LOG

#define IPMI_DBG_PRINT(FMT)            											\
do																				\
{																				\
	printf (FMT);																\
	PLATFORM_DBGLOGFN(LOG_DEBUG,TDBG_FILE_LINE_FMT FMT,TDBG_FILENAME,TDBG_LINENO);	\
} while (0)

#define IPMI_DBG_PRINT_1(fmt, args...) 											\
do																				\
{																				\
	printf (fmt, args);															\
	PLATFORM_DBGLOGFN(LOG_DEBUG,TDBG_FILE_LINE_FMT fmt,TDBG_FILENAME,TDBG_LINENO,##args);	\
} while (0)

#define IPMI_DBG_PRINT_2(fmt, args...) 											\
do																				\
{																				\
	printf (fmt, args);															\
	PLATFORM_DBGLOGFN(LOG_DEBUG,TDBG_FILE_LINE_FMT fmt,TDBG_FILENAME,TDBG_LINENO,##args);	\
} while (0)

#define IPMI_DBG_PRINT_3(fmt, args...) 											\
do																				\
{																				\
	printf (fmt, args);															\
	PLATFORM_DBGLOGFN(LOG_DEBUG,TDBG_FILE_LINE_FMT fmt,TDBG_FILENAME,TDBG_LINENO,##args);	\
} while (0)

#define IPMI_DBG_PRINT_4(fmt, args...) 											\
do																				\
{																				\
	printf (fmt, args);															\
	PLATFORM_DBGLOGFN(LOG_DEBUG,TDBG_FILE_LINE_FMT fmt,TDBG_FILENAME,TDBG_LINENO,##args);	\
} while (0)
	
#define IPMI_DBG_PRINT_BUF(BUF, LEN) 											\
do																				\
{																				\
	print_buf (BUF, LEN);														\
	PLATFORM_DBGLOGBUFFN(LOG_DEBUG,TDBG_FILENAME,TDBG_LINENO,BUF,LEN)			\
} while (0)

#endif	//ENABLE_DEBUG_LOG

#endif	/* ENABLE_DEBUG_MACROS */

#else   /* If DEBUG */

#define IPMI_DBG_PRINT(STR)
#define IPMI_DBG_PRINT_1(FMT, ARG2)
#define IPMI_DBG_PRINT_2(FMT, ARG1, ARG2)
#define IPMI_DBG_PRINT_3(FMT, ARG1, ARG2, ARG3)
#define IPMI_DBG_PRINT_4(FMT, ARG1, ARG2, ARG3, ARG4)
#define IPMI_DBG_BREAK
#define IPMI_ASSERT(ARG)
#define IPMI_ASSERT_CONTINUE(ARG)
#define IPMI_DBG_PRINT_BUF(BUF, LEN)
#define IPMI_DBG_PRINT_STRUCT_3(STRUCT, M1, M2, M3)
#define IPMI_DBG_PRINT_STRUCT_4(STRUCT, M1, M2, M3, M4)
#define IPMI_DBG_PRINT_STRUCT_5(STRUCT, M1, M2, M3, M4, M5)
#define IPMI_DBG_PRINT_STRUCT_6(STRUCT, M1, M2, M3, M4, M5, M6)

#define PRINT_BUF(BUF, LEN)
#define IPMI_DBG_PRINT_BUF(BUF, LEN)

#endif     /* If DEBUG */


#endif	/* DEBUG_H */
