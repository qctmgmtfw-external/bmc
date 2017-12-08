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
//#define DEBUG
#define SECTOR_SIZE 1


#ifndef DEBUG
	#define dprintf(...)
#else
	#define dprintf(format,args...) printf(format,##args)
#endif
typedef unsigned long ulong;
typedef unsigned short ushort;
typedef unsigned char uchar;

typedef struct {
        ulong   size;                   /* total bank size in bytes             */
        ushort  sector_count;           /* number of erase units                */
        ulong   flash_id;               /* combined device & manufacturer code  */
        ulong   start[1];   /* physical sector start addresses */
        uchar   protect[1]; /* sector protection status        */
#ifdef CFG_FLASH_CFI
        uchar   portwidth;              /* the width of the port                */
        uchar   chipwidth;              /* the width of the chip                */
        ushort  buffer_size;            /* # of bytes in write buffer           */
        ulong   erase_blk_tout;         /* maximum block erase timeout          */
        ulong   write_tout;             /* maximum write timeout                */
        ulong   buffer_write_tout;      /* maximum buffer write timeout         */
        ushort  vendor;                 /* the primary vendor id                */
        ushort  cmd_reset;              /* Vendor specific reset command        */
        ushort  interface;              /* used for x8/x16 adjustments          */
        ushort  legacy_unlock;          /* support Intel legacy (un)locking     */
#endif
} flash_info_t;

FMH fmh_buffer;
ALT_FMH altfmh_buffer;
flash_info_t file_info;

unsigned char  CalculateModule100(unsigned char *Buffer, unsigned long Size);
static FMH * CheckForNormalFMH(FMH *fmh);
static unsigned long  CheckForAlternateFMH(ALT_FMH *altfmh);

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

static
FMH *
CheckForNormalFMH(FMH *fmh)
{
	if (strncmp((char *)fmh->FMH_Signature,FMH_SIGNATURE,sizeof(FMH_SIGNATURE)-1) != 0)
			return NULL;

	if (le16_to_host(fmh->FMH_End_Signature) != FMH_END_SIGNATURE)
			return NULL;

	if (ValidateModule100((unsigned char *)fmh,sizeof(FMH)) != 0)
			return NULL;
	
	return fmh;
			
}

static
unsigned long 
CheckForAlternateFMH(ALT_FMH *altfmh)
{

	if (strncmp((char *)altfmh->FMH_Signature,FMH_SIGNATURE,sizeof(FMH_SIGNATURE)-1) != 0)
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

	/* Check if Normal FMH is found */
	//flash_read((ulong)SectorAddr,sizeof(FMH),(char*)&fmh_buffer);
	memcpy((char*)&fmh_buffer, SectorAddr, sizeof(FMH));
	
	fmh = (FMH *)(&fmh_buffer);
	fmh = CheckForNormalFMH(fmh);
	if (fmh != NULL)
		return fmh;

	/* If Normal FMH is not found, check for alternate FMH */
	//flash_read((ulong)SectorAddr+SectorSize - sizeof(ALT_FMH),sizeof(ALT_FMH),(char *)&altfmh_buffer);
	memcpy((char*)&fmh_buffer, SectorAddr+SectorSize, sizeof(FMH));
	
	altfmh = (ALT_FMH*)&altfmh_buffer;
	FMH_Offset = CheckForAlternateFMH(altfmh);
	if (FMH_Offset == INVALID_FMH_OFFSET)
		return NULL;

	//flash_read((ulong)SectorAddr+FMH_Offset,sizeof(FMH),(char *)&fmh_buffer);	
	memcpy((char*)&fmh_buffer, SectorAddr+FMH_Offset, sizeof(FMH));
	fmh = (FMH*)&fmh_buffer;
	
	/* If alternate FMH is found, validate it */
	fmh = CheckForNormalFMH(fmh);
	return fmh;
}

void
CreateFMH(FMH *fmh,unsigned long AllocatedSize, MODULE_INFO *mod, unsigned long Location)
{
        /* Clear the Structure */
        memset((void *)fmh,0,sizeof(FMH));

        /* Copy the module information */
        memcpy((void *)&(fmh->Module_Info),(void *)mod,sizeof(MODULE_INFO));

        /* Fill the FMH Fields */
        strncpy((char *)fmh->FMH_Signature,FMH_SIGNATURE,sizeof(FMH_SIGNATURE)-1);
        fmh->FMH_Ver_Major              = FMH_MAJOR;
        fmh->FMH_Ver_Minor              = FMH_MINOR;
        fmh->FMH_Size                   = FMH_SIZE;
        fmh->FMH_End_Signature  = host_to_le16(FMH_END_SIGNATURE);

        fmh->FMH_AllocatedSize  = host_to_le32(AllocatedSize);
        fmh-> FMH_Location = host_to_le32(Location);

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
	strncpy((char *)altfmh->FMH_Signature,FMH_SIGNATURE,sizeof(FMH_SIGNATURE)-1);
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

int
ListFMH(const char *FileMemPointer, const unsigned long FileSize, const FMH *FileFMH, unsigned long *PartitionLocation, unsigned long *PartitionLocationFMH, unsigned long MaxPartitionNum)
{
	flash_info_t *flinfo;
	unsigned long SectorSize;
	unsigned long SectorCount;
	volatile unsigned long PartitionCount;
	unsigned long i;
	FMH	*pfmh = NULL;
	MODULE_INFO *mod;
	unsigned char Name[9];
	char* pFileMemPointer = (char*)FileMemPointer;
	unsigned int location_offset = 0;


	//Assign file flash informat to porinter
	flinfo = &file_info;
	file_info.size = FileSize;
	SectorCount = file_info.size / SECTOR_SIZE;
	dprintf("Listing FMH Modules\n");

	dprintf("Flash Size : %ld KB \n",flinfo->size >> 10);

	flinfo = &file_info;
		

	PartitionCount = 0;
		for (i=0;i<SectorCount;)
		{
			SectorSize = SECTOR_SIZE;

			pfmh = ScanforFMH((unsigned char *)pFileMemPointer,SectorSize);
			if (pfmh == NULL)
			{
				i++;
				pFileMemPointer += SECTOR_SIZE;
				location_offset += SECTOR_SIZE;
				continue;
			}else{
				dprintf("!!Found FMH at 0x%lx\n", (unsigned long)pFileMemPointer-(unsigned long)FileMemPointer);
				memcpy((void*)((unsigned long)FileFMH+ sizeof(FMH)* PartitionCount), (void*)pfmh, sizeof(FMH));
				//We need the Alternate FMH for recontruction
				
			}
			
			/* Print Information about the FMH and its module */
			dprintf("\nPartition count : %ld\n", PartitionCount);
			dprintf("\nFMH Located at 0x%08lx of Size 0x%08x\n",
					(unsigned long)location_offset, le32_to_host(pfmh->FMH_AllocatedSize));
					//(unsigned long)flinfo->start[i],le32_to_host(fmh->FMH_AllocatedSize));
			PartitionLocationFMH[PartitionCount] = (unsigned long)location_offset;
			mod = &(pfmh->Module_Info);
			strncpy((char *)Name,(char *)mod->Module_Name,8);
			
			Name[8]=0;
			dprintf("Name    : %s\n",Name);
			dprintf("Ver     : %d.%d\n",mod->Module_Ver_Major,mod->Module_Ver_Minor);
			dprintf("Type 	: 0x%04x\n",le16_to_host(mod->Module_Type));
			dprintf("Flags	: 0x%04x\n",le16_to_host(mod->Module_Flags));
			dprintf("Size 	: 0x%08x\n",le32_to_host(mod->Module_Size));
			dprintf("Location: 0x%08lx\n",(unsigned long)location_offset +
								le32_to_host(mod->Module_Location));
			PartitionLocation[PartitionCount] = (unsigned long)location_offset +
                                                                le32_to_host(mod->Module_Location);
			dprintf("LoadAddr: 0x%08x\n",le32_to_host(mod->Module_Load_Address));

			/* Validate CheckSum */
			if (le16_to_host(mod->Module_Flags) & MODULE_FLAG_VALID_CHECKSUM)
			{
				if (CalculateCRC32((unsigned char *)pFileMemPointer +
								le32_to_host(mod->Module_Location),
								le32_to_host(mod->Module_Size))
								== le32_to_host(mod->Module_Checksum))
					dprintf("CheckSum: Valid\n");
				else
					dprintf("CheckSum: Invalid\n");
			}
			else
					dprintf("CheckSum: Not Computed\n");

			dprintf("--------------------------------------------------\n");


			pFileMemPointer += SECTOR_SIZE;
			location_offset += SECTOR_SIZE;
			PartitionCount++;
			if(PartitionCount > MaxPartitionNum)
				return 0;
		} /* sector */
	if(pfmh == FileFMH)//Assme FMH partation larger than 2
		return 0;
	else
		return PartitionCount;
}

