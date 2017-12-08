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
unsigned char  CalculateModule100(unsigned char *Buffer, unsigned long Size);
unsigned char  ValidateModule100(unsigned char *Buffer, unsigned long Size);

unsigned long  CalculateCRC32(unsigned char *Buffer, unsigned long Size);
void BeginCRC32(unsigned long *crc32);
void DoCRC32(unsigned long *crc32, unsigned char Data);
void EndCRC32(unsigned long *crc32);

unsigned char CalculateCRC8( const void *data, int length );

