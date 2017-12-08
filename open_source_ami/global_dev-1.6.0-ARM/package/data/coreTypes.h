/****************************************************************
 **                                                            											 **
 **    (C)Copyright 2006-2009, American Megatrends Inc.        			 **
 **                                                            											 **
 **            All Rights Reserved.                            									 **
 **                                                            											 **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             					 **
 **                                                            											 **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         				 **
 **                                                            											 **
 ****************************************************************/


/*****************************************************************************
 *
 *   coreTypes.h
 *
 *****************************************************************************/
#ifndef CORE_TYPES_H
#define CORE_TYPES_H

#ifdef __KERNEL__
#include <linux/types.h>
#include <linux/version.h>
#endif

/*@-incondefs@*/
/*@-likelybool@*/
/*@-constuse@*/
/*@-typeuse@*/
#ifdef __cplusplus
extern "C" {
#endif

#ifndef __cplusplus                         /* thus must be a .c program */
/*@-cppnames@*/
#ifdef __KERNEL__
#if (LINUX_VERSION_CODE <=  KERNEL_VERSION(2,6,15))  //NOTE: bool is already defined in linux/types.h
    typedef unsigned char bool;
#endif
#else
    typedef unsigned char bool;
#endif
    #define false (bool)0
    #define true  (bool)1
    typedef int BOOL ;
/*@=cppnames@*/
#else
    #ifndef _WIN32
        #define BOOL bool
    #endif
#endif


#ifndef FALSE
    #define      FALSE   0
    #define      TRUE    1
#endif

typedef   signed char    int8 ;
//#ifndef _WIN32
typedef   signed char    INT8 ;
//#endif
typedef          char    CHAR ;
typedef unsigned char   uint8 ;
#ifndef __KERNEL__
typedef unsigned char      u8 ;
#endif
typedef unsigned char   UINT8 ;
typedef unsigned char   UCHAR ;
typedef unsigned char    BYTE ;             /* for existing MegaRAC code */

typedef          short   int16;
typedef          short   INT16;
typedef unsigned short  uint16;
#ifndef __KERNEL__
typedef unsigned short     u16;             /* for G2 team */
#endif
typedef unsigned short  UINT16;
typedef unsigned short   TWOBYTES;          /* for existing MegaRAC code */
typedef unsigned short       WORD;          /* for existing MegaRAC code */

#if defined (__x86_64__) || defined (WIN64)
typedef          int    int32;
                      /* INT32 */           /* see below */
typedef          long    LONG ;
typedef unsigned int   uint32;
typedef unsigned long  uint64;
#else
typedef          long    int32;
                      /* INT32 */           /* see below */
typedef          long    LONG ;
typedef unsigned long   uint32;
typedef unsigned long long   uint64;
#endif
#ifndef __KERNEL__
#if defined (__x86_64__) || defined (WIN64)
typedef unsigned int      u32;             /* for G2 team */
#else
typedef unsigned long      u32;             /* for G2 team */
#endif
#endif
                     /* UINT32 */           /* see below */
typedef unsigned long   ULONG ;
typedef unsigned long    FOURBYTES;         /* for existing MegaRAC code */
typedef unsigned long        DWORD;         /* for existing MegaRAC code */

#if defined(_MSC_VER)                       /* automatically specified by VisualStudio compiler */
    /* already defined by basetsd.h, so to avoid warnings, don't define here */
#else
    typedef          bool    BOOLEAN;
    typedef          long    INT32;
    typedef unsigned long   UINT32;
#endif

#ifdef _WIN32
 #define ssize_t size_t
#endif

#ifdef _WIN32
#include <windows.h>
#else
	/*typedef unsigned long	UINT32;
	typedef signed   long	SINT32;

	typedef unsigned long	INT32U;
	typedef signed   long	INT32S;
*/
	typedef void *			HANDLE;
	typedef void *			LPVOID;
#endif

/*-------------------------------------------------------------------------
 *
 *  types internal to the core library
 *
 *-------------------------------------------------------------------------*/
#define CORE_CHAR_TO_Core_Id(a,b,c,d) ( ((Core_Id)a<<24) | ((Core_Id)b<<16) | (c<<8) | d )
typedef uint32 Core_Id;

#ifdef __cplusplus
}
#endif

/*@=incondefs@*/
/*@=likelybool@*/
/*@=constuse@*/
/*@=typeuse@*/

#endif /* __CORE_TYPES_H__ */
