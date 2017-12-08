/*
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2006-2009, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 */

#ifndef MODULE			/* If not defined in Makefile */
#define MODULE
#endif

#ifndef __KERNEL__		/* If not defined in Makefile */
#define __KERNEL__
#endif

#include <linux/module.h>
#include <linux/kernel.h>	/* printk() */
#include <linux/err.h>
#include <asm/io.h>
#include "fmh.h"

#include <linux/mtd/mtd.h>
//#include <linux/mtd/map.h>
//#include <linux/mtd/partitions.h>


extern unsigned long FlashStart;
extern unsigned long FlashSize;
extern unsigned long EnvStart;
extern unsigned long EnvSize;
extern unsigned long FlashSectorSize;
extern unsigned long UsedFlashStart;
extern unsigned long UsedFlashSize;
unsigned int g_MTDSupport = 1;

/*this returns offset not address*/
static long
ScanFirmwareModule(unsigned long FlashStartOffset, unsigned long FlashSize,
				unsigned long SectorSize, unsigned char* buf,FLASH_INFO* flinfo)
{
	unsigned long SectorCount = FlashSize/SectorSize;
	unsigned long i;
	FMH fmh;
	MODULE_INFO *mod = NULL;
	unsigned long ModuleOffset;
	unsigned long Size;
	int fmh_found = 0;
	size_t NumBytesRead;

	Size = 0;

	for (i=0;i<SectorCount;)
	{
		/* Scan the flash and get FMH*/
		fmh_found = ScanforFMH_UseReadFn((FlashStartOffset+(i*SectorSize)),SectorSize,flinfo,&fmh);
		if (fmh_found == 0)
		{
			i++;
			continue;
		}

		mod = &(fmh.Module_Info);

		/* Check if module type is FIRMWARE */
		if (le16_to_host(mod->Module_Type) == MODULE_FMH_FIRMWARE)
			break;

		i+=(le32_to_host(fmh.FMH_AllocatedSize)/SectorSize);


	}

	if (i >= SectorCount)
	{
		printk(KERN_ERR "Unable to Locate MODULE_FMH_FIRMWARE\n");
		return -1;
	}


	Size = le32_to_host(mod->Module_Size);
	ModuleOffset = FlashStartOffset +(i*SectorSize) + le32_to_host(mod->Module_Location);

	//copy here
	if( flinfo->read_flash(ModuleOffset,Size,&NumBytesRead,buf) != 0)
	{
		printk(KERN_ERR "Error reading flash in order to copy MODULE_FMH_FIRMWARE info\n");
		return -1;
	}

	return Size;
	
}





/************************Translation layer for reading from flash**********************************/
int mtd_read_thunk(unsigned long Offset,unsigned long NumBytes,size_t* NumBytesRead,unsigned char* Buffer)
{
   
	struct mtd_info *ractrends_mtd = NULL;


	//find the mtd device

	ractrends_mtd = get_mtd_device(NULL,0);
	if(ractrends_mtd == NULL || IS_ERR(ractrends_mtd))
	{
		g_MTDSupport = 0;
		return -1;
	}
	else
	{
		g_MTDSupport = 1;
	}
	
	//then read from the mtd device for the required params
	return ractrends_mtd->read(ractrends_mtd,(loff_t)Offset,NumBytes,NumBytesRead,Buffer);

}

char* g_cached_fwinfo_buf;

/* ----------------Proc File System Read Hardware Information ---------------------*/
int
fwinfo_read(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
	static unsigned long fwInfo_size;
	FLASH_INFO flinfo;

	static int firsttime = 1;

	if(!firsttime)
	{
		///this is not the first time.so we just return what we have read again
		//instead of scanning again
		*eof = 1;
		memcpy(buf,g_cached_fwinfo_buf,fwInfo_size);
		return fwInfo_size;
	}
	else
	{
		//this is the first time
		firsttime = 0;
	}



	/* This is a requirement. So don't change it */
	*start = buf;

	/* Hack: Assuming all reads are using cat of proc entry and no   *
	 * lseeking random offsets, if offset is not 0, assume it is eof *
	 * If needed fix it later
	 */
	if (offset != 0)
	{
		*eof = 1;
		return 0;
	}


	/* Find the SectorSize (erase size) of Flash */

	flinfo.read_flash = mtd_read_thunk;

	if (g_MTDSupport == 1)
	{
		/* Scan and Get FwInfo */
		fwInfo_size = ScanFirmwareModule(CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_START - CONFIG_SPX_FEATURE_GLOBAL_FLASH_START,
				CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_SIZE,
				CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE,
				(unsigned char*)buf,
				&flinfo);
		if (fwInfo_size <= 0 )
		{
			printk(KERN_ALERT "Unable to get Firmware Information\n");
			*eof = 1;
			return 0;
		}
	} 
	else
	{
		// These Macro will be extracted from .MAP file or it may come from .PRJ file.
		// These Macro will be used only when get_mtd_device fails
#if defined (CONFIG_SPX_FEATURE_GLOBAL_BUILD_MAJOR) && \
	defined (CONFIG_SPX_FEATURE_GLOBAL_BUILD_MINOR) && \
	defined (CONFIG_SPX_FEATURE_GLOBAL_BUILD_NUMBER) && \
	defined (CONFIG_SPX_FEATURE_GLOBAL_BUILD_DATE) && \
	defined (CONFIG_SPX_FEATURE_GLOBAL_BUILD_TIME) && \
	defined (CONFIG_SPX_FEATURE_GLOBAL_BUILD_DESC) && \
	defined (CONFIG_SPX_FEATURE_GLOBAL_PRODUCT_ID)
	
		sprintf (buf, 	"FW_VERSION=%d.%d.%d\n"
						"FW_DATE=%s\n"
						"FW_BUILDTIME=%s\n"
						"FW_DESC=%s\n"
						"FW_PRODUCTID=%d\n", 
						CONFIG_SPX_FEATURE_GLOBAL_BUILD_MAJOR,
						CONFIG_SPX_FEATURE_GLOBAL_BUILD_MINOR,
						CONFIG_SPX_FEATURE_GLOBAL_BUILD_NUMBER,
						CONFIG_SPX_FEATURE_GLOBAL_BUILD_DATE,
						CONFIG_SPX_FEATURE_GLOBAL_BUILD_TIME,
						CONFIG_SPX_FEATURE_GLOBAL_BUILD_DESC,
						CONFIG_SPX_FEATURE_GLOBAL_PRODUCT_ID);
		fwInfo_size = strlen (buf);
#else
		// TODO: These hardcoded fwinfo will be removed once these information is extracted from .MAP file. 
		sprintf (buf, 	"FW_VERSION=0.0.0\n"
						"FW_DATE=Jan 01 2010\n"
						"FW_BUILDTIME=12:01:01 IST\n"
						"FW_DESC=WARNING : UNOFFICIAL BUILD!!\n"
						"FW_PRODUCTID=1\n");
		fwInfo_size = strlen (buf);
#endif
	}

	/* Set End of File if no more data */
	*eof = 1;
	/* Return length of data */
	//remember the fwinfo
	g_cached_fwinfo_buf = kmalloc(fwInfo_size,GFP_KERNEL);
	if(g_cached_fwinfo_buf == NULL)
	{
		printk(KERN_ERR "Error: Could not allocate memory structure for holding fwinfo\n");
		//always treat like first time therefore
		firsttime = 1;
	}
	else
	{
		memcpy(g_cached_fwinfo_buf,buf,fwInfo_size);
	}

	return fwInfo_size;
}


void free_info_cache(void)
{
	if(g_cached_fwinfo_buf != NULL)
		kfree(g_cached_fwinfo_buf);
}


