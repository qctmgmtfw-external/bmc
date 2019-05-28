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
#if 0
#include <stdio.h>
#include <ctype.h>
#include "config.h"
#include "types.h"
#include "console.h"
#include "datadisp.h"
#else
//#include <common.h>
//#include <exports.h>
#include <linux/kernel.h>
#include "types.h"
#include "datadisp.h"
#define BYTES_PER_LINE 16
#define ConsolePrintf printk
#define isprint(x) (((x) >=32) && ((x) < 127))
#endif

void
DumpContents(UINT32 Address, char *Buffer, UINT16 BufLen, UINT8 Format, void (*WaitFn)(void))
{
	UINT32 Index;		/* Index to the Buffer */
	UINT8  x;			/* No of Bytes written in a line */
	char   charBuf[BYTES_PER_LINE+1];
	UINT32 Data;
	
	/* Reduce BufLen to nearest Format boundary */
	BufLen = (BufLen/Format) *Format;	

	/* Null terminate the character buffer */
	charBuf[BYTES_PER_LINE] = 0;

	/* Initialize No of Bytes written in line to zero */
	x = 0;

	
	for (Index=0;Index<BufLen;Index+=Format)
	{
		/* Beginning of New Line. Write Address */
		if (x == 0)
			ConsolePrintf("0x%08lX : ",Address);


		/* Print the Value according to the format */
		
		switch (Format)
		{
			case FORMAT_BYTE:
				Data = (UINT32)(*((UINT8 *)(Buffer+Index)));
				ConsolePrintf("%02X",(UINT8)Data);
				break;
			case FORMAT_WORD:
				Data = (UINT32)(*((UINT16 *)(Buffer+Index)));
				ConsolePrintf("%04X",(UINT16)Data);
				break;
			case FORMAT_DWORD:
				Data = (UINT32)(*((UINT32 *)(Buffer+Index)));
				ConsolePrintf("%08lX",(UINT32)Data);
				break;
			default:
				Data = 0;
				ConsolePrintf("INTERNAL ERROR: Unknown Format type in Data Display\n");
				break;
		}			

		/* Add character to Character Buffer */
		if (Format == FORMAT_BYTE)
		{
			if (isprint((UINT8)Data))
				charBuf[x] = (UINT8)(Data);
			else
				charBuf[x] = '.';
		}

		/* Increment number of bytes written */
		x+=Format;
		
		/* Print Spaces between every data and an '-' in the middle of line */
		if (x == (BYTES_PER_LINE/2))
			ConsolePrintf(" - ");
		else
			ConsolePrintf(" ");

		/* If end of line */
		if (x == BYTES_PER_LINE)
		{
			if (Format == FORMAT_BYTE)
				ConsolePrintf(  "%s",charBuf);		/* Print Character Bytes	*/
			ConsolePrintf("\n");					/* Begin a new Line			*/
			x = 0;							/* Reset No of bytes in line*/
			Address+=BYTES_PER_LINE;		/* Increment Address		*/
		}

		if (WaitFn)
			(*WaitFn)();

	}  /* for */
	if (x!=0)
		ConsolePrintf("\n");	
	return ;
}

