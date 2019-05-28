/*
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2009-2015, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 */
#ifndef REX_TYPES_H
#define REX_TYPES_H

/*
  Removed these as the names don't reflect their type 
  (signed/unsigned) explicilty. These are replaced with
  equivalent type defines  prefixed with U (unsigned) 
  and S (signed)
*/
	
#if 1
typedef unsigned char	BYTE;
typedef unsigned short	WORD;
typedef unsigned long	DWORD;
typedef signed  char	INT8;
typedef signed  short	INT16;
typedef signed 	long	INT32;
#endif

typedef unsigned char	UBYTE;
typedef signed   char	SBYTE;

typedef unsigned short	UWORD;
typedef signed   short	SWORD;

typedef unsigned long	UDWORD;
typedef signed   long	SDWORD;

typedef unsigned char	UINT8;
typedef signed   char	SINT8;

typedef unsigned char	INT8U;
typedef signed   char	INT8S;

typedef unsigned short	UINT16;
typedef signed   short	SINT16;

typedef unsigned short	INT16U;
typedef signed   short	INT16S;


#ifdef WIN32
	#include <windows.h>
	/* The following are already defined by windows.h 
       but in different form  as belows :
			typedef unsigned  int	UINT32;
			typedef signed	  int	INT32;
	*/
#else
	typedef unsigned long	UINT32;
	typedef signed   long	SINT32;

	typedef unsigned long	INT32U;
	typedef signed   long	INT32S;

	typedef void *			HANDLE;
	typedef void *			LPVOID;
#endif

typedef int				BOOL;
#ifdef FALSE
#undef FALSE
#endif
#define FALSE           0

#ifdef TRUE
#undef TRUE
#endif
#define TRUE            1

/* Define Windows Socket Specific types and defines */
#ifdef WIN32
	#include <winsock.h>
#else
	typedef int				SOCKET;
	typedef UBYTE *			LPBYTE;
	#define INVALID_SOCKET	-1
	#define SOCKET_ERROR	-1
	#define closesocket		close
	#define WINAPI
	typedef char			TCHAR;
	typedef UBYTE		INT4U;
#endif



#endif		/* REX_TYPES_H*/
