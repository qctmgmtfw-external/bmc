/*****************************************************************
******************************************************************
***                                                            ***
***        (C)Copyright 2010, American Megatrends Inc.         ***
***                                                            ***
***                    All Rights Reserved                     ***
***                                                            ***
***       5555 Oakbrook Parkway, Norcross, GA 30093, USA       ***
***                                                            ***
***                     Phone 770.246.8600                     ***
***                                                            ***
******************************************************************
******************************************************************/
#include "crc32.h"
#include "checksum.h"

unsigned long
CalculateCRC32(unsigned char *Buffer, unsigned long Size)
{
    unsigned long i,crc32 = 0xFFFFFFFF;

	/* Read the data and calculate crc32 */	
    for(i = 0; i < Size; i++)
		crc32 = ((crc32) >> 8) ^ CrcLookUpTable[(Buffer[i]) 
								^ ((crc32) & 0x000000FF)];
	return ~crc32;
}

void
BeginCRC32(unsigned long *crc32)
{
	*crc32 = 0xFFFFFFFF;
	return;
}

void
DoCRC32(unsigned long *crc32, unsigned char Data)
{
	*crc32=((*crc32) >> 8) ^ CrcLookUpTable[Data ^ ((*crc32) & 0x000000FF)];
	return;
}

void
EndCRC32(unsigned long *crc32)
{
	*crc32 = ~(*crc32);
	return;
}

