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
#include "checksum.h"
unsigned char 
CalculateModule100(unsigned char *Buffer, unsigned long Size)
{
	unsigned char Sum=0;

	while (Size--)
	{
		Sum+=(*Buffer);
		Buffer++;
	}	

	return (~Sum)+1;
}

unsigned char 
ValidateModule100(unsigned char *Buffer, unsigned long Size)
{
	unsigned char Sum=0;

	while (Size--)
	{
		Sum+=(*Buffer);
		Buffer++;
	}

	return Sum;
}
