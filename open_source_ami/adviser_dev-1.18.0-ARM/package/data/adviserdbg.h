/*****************************************************************
 **                                                             **
 **     (C) Copyright 2006-2009, American Megatrends Inc.       **
 **                                                             **
 **             All Rights Reserved.                            **
 **                                                             **
 **         5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                             **
 **         Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                             **
 ****************************************************************/

/**
 * debug.h
**/
#ifndef DEBUG_H
#define DEBUG_H
#include "dbgout.h"

#define DLEVEL		0

#if DLEVEL > 0
#define TINFO_1(fmt, args...)									\
printf (fmt, ##args)
#else
#define TINFO_1(fmt, args...)
#endif


#if DLEVEL > 1
#define TINFO_2(fmt, args...)									\
printf (fmt, ##args)
#else
#define TINFO_2(fmt, args...)
#endif

#if DLEVEL > 2
#define TINFO_3(fmt, args...)									\
printf (fmt, ##args)
#else
#define TINFO_3(fmt, args...)
#endif

#if DLEVEL > 3
#define TINFO_4(fmt, args...)									\
printf (fmt, ##args)
#else
#define TINFO_4(fmt, args...)
#endif

#if DLEVEL > 3
#define DUMP_BUF(BUF, SIZE)										\
dump_buffer (BUF, SIZE)
#else
#define DUMP_BUF(BUF, SIZE)
#endif

#endif	/* DEBUG_H */

