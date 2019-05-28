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

#ifdef __KERNEL__
	#ifdef __UBOOT__
		#include <common.h>
	#else
		#include <linux/kernel.h>
		#include <linux/string.h>
	#endif
#else
	#include <stdio.h>
	#include <string.h>
#endif
#include "fmh.h"
#include "crc32.h"


#include <linux/mtd/mtd.h>
//#include <linux/mtd/map.h>
//#include <linux/mtd/partitions.h>


unsigned char  CalculateModule100(unsigned char *Buffer, unsigned long Size);
FMH * CheckForNormalFMH(FMH *fmh);
unsigned long  CheckForAlternateFMH(ALT_FMH *altfmh);

unsigned char CalculateModule100(unsigned char *Buffer, unsigned long Size)
{
	unsigned char Sum=0;

	while (Size--)
	{
		Sum+=(*Buffer);
		Buffer++;
	}	

	return (unsigned char)((~Sum)+1);
}

static
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

FMH *
CheckForNormalFMH(FMH *fmh)
{
	if (strncmp(fmh->FMH_Signature,FMH_SIGNATURE,sizeof(FMH_SIGNATURE)-1) != 0)
	{
	//	printf("No signature, %s\n",fmh->FMH_Signature);
		return NULL;
	}

	if (le16_to_host(fmh->FMH_End_Signature) != FMH_END_SIGNATURE)
	{
	//	printf("No end signature, %s\n",fmh->FMH_End_Signature);
		return NULL;
	}

	if (ValidateModule100((unsigned char *)fmh,sizeof(FMH)) != 0)
	{
	//	printf("No validate100\n");
		return NULL;
	}
	
	return fmh;
			
}

unsigned long 
CheckForAlternateFMH(ALT_FMH *altfmh)
{

	if (strncmp(altfmh->FMH_Signature,FMH_SIGNATURE,sizeof(FMH_SIGNATURE)-1) != 0)
			return INVALID_FMH_OFFSET;

	if (le16_to_host(altfmh->FMH_End_Signature) != FMH_END_SIGNATURE)
			return INVALID_FMH_OFFSET;

	if (ValidateModule100((unsigned char *)altfmh,sizeof(ALT_FMH)) != 0)
			return INVALID_FMH_OFFSET;
	
	return le32_to_host(altfmh->FMH_Link_Address);

}



FMH *
ScanforFMH(unsigned char *SectorAddr, unsigned long SectorSize)
{
	FMH *fmh;
	ALT_FMH *altfmh;
	unsigned long FMH_Offset;

	//printf("SecAddr = %x,SecSize = %ldK\n",SectorAddr,SectorSize/1024);
	/* Check if Normal FMH is found */
	fmh = (FMH *)SectorAddr;
	fmh = CheckForNormalFMH(fmh);
	if (fmh != NULL)
	{
		//printf("No normal FMH\n");
		return fmh;
	}

	/* If Normal FMH is not found, check for alternate FMH */
	altfmh = (ALT_FMH *)(SectorAddr+SectorSize - sizeof(ALT_FMH));
	FMH_Offset = CheckForAlternateFMH(altfmh);
	if (FMH_Offset == INVALID_FMH_OFFSET)
	{
		return NULL;
	}
	if( SectorSize < FMH_Offset )
		fmh = (FMH *)(SectorAddr + (FMH_Offset % SectorSize));
	else
		fmh = (FMH *)(SectorAddr + FMH_Offset);
	
	/* If alternate FMH is found, validate it */
	fmh = CheckForNormalFMH(fmh);
	return fmh;
}


//returns fmh offset
unsigned int
ScanforFMH_UseReadFn(unsigned long FlashOffset, unsigned long SectorSize,FLASH_INFO* flinfo,FMH* buf)
{
	FMH *fmh;
	ALT_FMH *altfmh;
	unsigned long FMH_Offset;
	size_t retlen;
	//int retval;



	/* Check if Normal FMH is found */
	//fmh = (FMH *)SectorAddr;
	retlen = sizeof(FMH);
	flinfo->read_flash(FlashOffset,sizeof(FMH),&retlen,(unsigned char*)buf);
	fmh = (FMH *)buf;
	fmh = CheckForNormalFMH(fmh);
	if (fmh != NULL)
	{
		return 1;
	}

	/* If Normal FMH is not found, check for alternate FMH */
	retlen = sizeof(ALT_FMH);
	flinfo->read_flash(FlashOffset+SectorSize-sizeof(ALT_FMH),sizeof(ALT_FMH),&retlen,(unsigned char*)buf);
	altfmh = (ALT_FMH *)buf;
	FMH_Offset = CheckForAlternateFMH(altfmh);
	if (FMH_Offset == INVALID_FMH_OFFSET)
	{
		return 0;
	}

	retlen = sizeof(FMH);
	if(FlashOffset < SectorSize)
		flinfo->read_flash(FMH_Offset,sizeof(FMH),&retlen,(unsigned char*)buf);
	else
		flinfo->read_flash(FlashOffset+FMH_Offset,sizeof(FMH),&retlen,(unsigned char*)buf);
	fmh = (FMH*)buf;

	/* If alternate FMH is found, validate it */
	fmh = CheckForNormalFMH(fmh);
	if(fmh != NULL)
	{
		return 1;
	}
	else
	{
		return 0;
	}

}

void
CreateFMH(FMH *fmh,unsigned long AllocatedSize, MODULE_INFO *mod)
{
	/* Clear the Structure */	
	memset((void *)fmh,0,sizeof(FMH));

	/* Copy the module information */
	memcpy((void *)&(fmh->Module_Info),(void *)mod,sizeof(MODULE_INFO));
					
	/* Fill the FMH Fields */		
	strncpy(fmh->FMH_Signature,FMH_SIGNATURE,FMH_SIGNATURE_SIZE-1);
	fmh->FMH_Ver_Major 		= FMH_MAJOR;
	fmh->FMH_Ver_Minor 		= FMH_MINOR;
	fmh->FMH_Size	   		= FMH_SIZE;
	fmh->FMH_End_Signature	= host_to_le16(FMH_END_SIGNATURE);
	
	fmh->FMH_AllocatedSize	= host_to_le32(AllocatedSize);

	/*Calculate Header Checksum*/
	fmh->FMH_Header_Checksum = CalculateModule100((unsigned char *)fmh,sizeof(FMH));
		
	return;
}

void
CreateAlternateFMH(ALT_FMH *altfmh,unsigned long FMH_Offset) 
{
	/* Clear the Structure */	
	memset((void *)altfmh,0,sizeof(ALT_FMH));
					
	/* Fill the FMH Fields */		
	strncpy(altfmh->FMH_Signature,FMH_SIGNATURE,FMH_SIGNATURE_SIZE-1);
	altfmh->FMH_End_Signature	= host_to_le16(FMH_END_SIGNATURE);
	
	altfmh->FMH_Link_Address	= host_to_le32(FMH_Offset);

	/*Calculate Header Checksum*/
	altfmh->FMH_Header_Checksum = CalculateModule100((unsigned char *)altfmh,
										sizeof(ALT_FMH));
	return;
}



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

