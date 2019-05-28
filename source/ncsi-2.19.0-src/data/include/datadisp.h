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
#ifndef REX_DBG_DATADISP_H
#define REX_DBG_DATADISP_H
//#include "config.h"
#include "types.h"

#define FORMAT_BYTE		sizeof(UBYTE)
#define FORMAT_WORD		sizeof(UWORD)
#define FORMAT_DWORD	sizeof(UDWORD)

void DumpContents(UINT32 Address, char *Buffer, UINT16 BufLen, UINT8 Format, void (*WaitFn)(void));

#endif  
