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

#ifndef MODULE			/* If not defined in Makefile */
#define MODULE
#endif

#ifndef __KERNEL__		/* If not defined in Makefile */
#define __KERNEL__
#endif

#include <linux/module.h>
#include <linux/kernel.h>	/* printk() */
#include <linux/version.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <asm/io.h>
#include "fmh.h"

#include <linux/mtd/mtd.h>
//#include <linux/mtd/map.h>
//#include <linux/mtd/partitions.h>

#ifdef CONFIG_SPX_FEATURE_GLOBAL_DUAL_IMAGE_SUPPORT
#include <linux/fs.h>      // Needed by filp
#endif
#include <asm/uaccess.h>   // Needed by segment descriptors

extern unsigned long FlashStart;
extern unsigned long FlashSize;
extern unsigned long EnvStart;
extern unsigned long EnvSize;
extern unsigned long FlashSectorSize;
extern unsigned long UsedFlashStart;
extern unsigned long UsedFlashSize;

#ifdef CONFIG_SPX_FEATURE_DISABLE_MTD_SUPPORT
unsigned int g_MTDSupport = 0;
#else
unsigned int g_MTDSupport = 1;
#endif

#ifdef CONFIG_SPX_FEATURE_GLOBAL_DUAL_IMAGE_SUPPORT

    #define MAX_LINE_LEN 250
    #define IMAGE_1 1
    #define IMAGE_2 2
    static char  running_image;

#endif


/*this returns offset not address*/
static long
ScanFirmwareModule(unsigned long FlashStartOffset, unsigned long FlashSize,
				unsigned long SectorSize, unsigned char* buf,FLASH_INFO* flinfo)
{
	unsigned long SectorCount = FlashSize/SectorSize;
	unsigned long i=0;
	FMH fmh;
	MODULE_INFO *mod = NULL;
	unsigned long ModuleOffset;
	unsigned long Size, Location;
	int fmh_found = 0;
	size_t NumBytesRead;

	Size = 0;
	Location = 0;

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
		printk(KERN_DEBUG "Unable to Locate MODULE_FMH_FIRMWARE\n");
		return -1;
	}

	if(mod != NULL)
	{
		Size = le32_to_host(mod->Module_Size);
		Location = le32_to_host(mod->Module_Location);
	}
	else
	{
		printk(KERN_ERR "Error: dereferencing null pointer\n");
		return -1;
	}

	if(fmh.FMH_Ver_Major == 1 && fmh.FMH_Ver_Minor >= 8)
		ModuleOffset = FlashStartOffset + Location;
	else
		ModuleOffset = FlashStartOffset +(i*SectorSize) + Location;

	//copy here
	if( flinfo->read_flash(ModuleOffset,Size,&NumBytesRead,buf) != 0)
	{
		printk(KERN_ERR "Error reading flash in order to copy MODULE_FMH_FIRMWARE info\n");
		return -1;
	}

	return Size;
	
}


#ifdef CONFIG_SPX_FEATURE_GLOBAL_DUAL_IMAGE_SUPPORT

/**
 * @fn GetCurrentRunningImage
 * @brief To Get the cuurent Running Image
 * @param[in]  - No parameters.
 * @retval       - Running image
                        0 -IMAGE_1 X_LINE_LEN
                        1 -IMAGE_2.                         
 */
static int GetCurrentRunningImage(void)
{
    struct file *f;
    mm_segment_t fs;	
    char buf[MAX_LINE_LEN];
    char *ptr = NULL;	
    int  img;
    
    f = filp_open("/proc/cmdline", O_RDONLY, 0);
    if(f == NULL)
    {
        printk(KERN_ALERT "file_open error!!.\n");
        return -1;
    }		
    fs = get_fs();
    set_fs(get_ds());
    memset(buf,0,MAX_LINE_LEN);
    f->f_op->read(f, buf,MAX_LINE_LEN, &f->f_pos);
    set_fs(fs);
    filp_close(f,NULL);

    ptr = strstr(buf,"imagebooted");
    if(ptr == NULL)
        return -1;
    ptr = strstr(ptr,"=");
    if(ptr == NULL)
        return -1;
    ptr++;
	
    /* In /proc/cmdline image argument specifies 1 -Image1 and 2 -Image2	*/
    img = *ptr - 48;

    if( (img != IMAGE_1) && (img != IMAGE_2) )
        return -1;

return img;
}


#endif


/************************Translation layer for reading from flash**********************************/
int mtd_read_thunk(unsigned long Offset,unsigned long NumBytes,size_t* NumBytesRead,unsigned char* Buffer)
{
   
     struct mtd_info *ractrends_mtd = NULL;

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
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,36))
	return ractrends_mtd->_read(ractrends_mtd,(loff_t)Offset,NumBytes,NumBytesRead,Buffer);
#else
	return ractrends_mtd->read(ractrends_mtd,(loff_t)Offset,NumBytes,NumBytesRead,Buffer);
#endif

}

char* g_cached_fwinfo_buf;

/* ----------------Proc File System Read Hardware Information ---------------------*/
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,36))
int
fwinfo_read(struct file *file,  char __user *buf, size_t count, loff_t *offset)
#else
int
fwinfo_read(char *buf, char **start, off_t offset, int count, int *eof, void *data)
#endif
{
	static long fwInfo_size = 0;	// changed from unsigned to signed due to negative values can be returned
	FLASH_INFO flinfo;
    char fmhinfo[256]={0};
	static int firsttime = 1;
	

/* We don't support seeked reads. read should start from 0 */	
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,36))
	if (*offset != 0)
		return 0;
#else
	*eof=1;
	if (offset != 0)
		return 0;
#endif
	
	if(!firsttime)
	{
		///this is not the first time.so we just return what we have read again
		//instead of scanning again

		if ( fwInfo_size > 0 ) {		// line added to avoid crash if no fwinfo is available
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,36))
	        if (copy_to_user(buf,g_cached_fwinfo_buf, fwInfo_size))
	            return -EFAULT;
#else
	    	memcpy(buf,g_cached_fwinfo_buf,fwInfo_size);
#endif
		}
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,36))
		*offset+=fwInfo_size;
#endif
		return fwInfo_size;
	}
	else
	{
		//this is the first time
		firsttime = 0;
	}

#ifdef CONFIG_SPX_FEATURE_GLOBAL_DUAL_IMAGE_SUPPORT

         running_image = GetCurrentRunningImage();
         if(running_image == -1)
         {
             printk(KERN_ALERT "Unable to get current running image info\n");
             return 0;
         }

	
#endif

#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,36))
	/* This is a requirement. So don't change it */
	*start = buf;
#endif



	/* Find the SectorSize (erase size) of Flash */

	flinfo.read_flash = mtd_read_thunk;

	if (g_MTDSupport == 1)
	{
		/* Scan and Get FwInfo */
#ifdef CONFIG_SPX_FEATURE_GLOBAL_DUAL_IMAGE_SUPPORT
	unsigned long flashstartoffset;

	flashstartoffset = CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_START - CONFIG_SPX_FEATURE_GLOBAL_FLASH_START;
        if( running_image == IMAGE_2)
        {
            flashstartoffset += CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_SIZE;
        }

		fwInfo_size = ScanFirmwareModule(flashstartoffset,
				CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_SIZE,
				CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE,
				(unsigned char*)fmhinfo,
				&flinfo);

#else
		fwInfo_size = ScanFirmwareModule(CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_START - CONFIG_SPX_FEATURE_GLOBAL_FLASH_START,
				CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_SIZE,
				CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE,
				(unsigned char*)fmhinfo,
				&flinfo);
#endif

		if (fwInfo_size <= 0 )
		{
			printk(KERN_ALERT "Unable to get Firmware Information\n");
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
	
		sprintf (fmhinfo, 	"FW_VERSION=%d.%d.%d\n"
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
		sprintf (fmhinfo, 	"FW_VERSION=0.0.0\n"
						"FW_DATE=Jan 01 2010\n"
						"FW_BUILDTIME=12:01:01 IST\n"
						"FW_DESC=WARNING : UNOFFICIAL BUILD!!\n"
						"FW_PRODUCTID=1\n");
		fwInfo_size = strlen (fmhinfo);
#endif
	}

	/* Set End of File if no more data */

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
		memcpy(g_cached_fwinfo_buf,fmhinfo,fwInfo_size);
	}
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,36))
	if (copy_to_user(buf,fmhinfo, fwInfo_size))
	        return -EFAULT;

	*offset+=fwInfo_size;
#else
	memcpy(buf,fmhinfo,fwInfo_size);
#endif
    
	return fwInfo_size;
}


#ifdef CONFIG_SPX_FEATURE_GLOBAL_DUAL_IMAGE_SUPPORT

/*
   FillDummyFWinfo fills the fwinfo with hardcoded values. 
 */ 
int FillDummyFWinfo(char *buf)
{
	sprintf (buf, 	"FW_VERSION=0.0.0\n"
				"FW_DATE=0\n"
				"FW_BUILDTIME=0\n"
				"FW_DESC=0\n"
				"FW_PRODUCTID=0\n");
	
return 1;
}

/*
   fwinfo1_read is a call back funtion to the fwinfo1 file. 
   For every read to fwinfo1 file, reads the image-1 fw info directly from mtd device.
*/
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36))
int
fwinfo1_read(struct file *file,  char __user *buf, size_t count, loff_t *offset)
#else
int
fwinfo1_read(char *buf, char **start, off_t offset, int count, int *eof, void *data)
#endif
{

	unsigned long fwInfo_size = 0;
	FLASH_INFO flinfo;
	char fmhinfo[256]={0};

/* We don't support seeked reads. read should start from 0 */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36))
        if (*offset != 0)
                return 0;
#else
        *eof=1;
        if (offset != 0)
                return 0;
#endif
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36))
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
#endif

	/* Find the SectorSize (erase size) of Flash */

	flinfo.read_flash = mtd_read_thunk;

	if (g_MTDSupport == 1)
	{
		/* Scan and Get FwInfo */
		fwInfo_size = ScanFirmwareModule(CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_START - CONFIG_SPX_FEATURE_GLOBAL_FLASH_START,
				CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_SIZE,
				CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE,
				(unsigned char*)fmhinfo,
				&flinfo);
		if ((signed long)fwInfo_size <= 0 )
		{
			printk(KERN_DEBUG "Firmware might be corrupted..\n");
			//FW might be corrupted, filling with dummy values.
			FillDummyFWinfo(fmhinfo);
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36))
			*eof = 1;
#endif
			fwInfo_size = strlen (fmhinfo); 
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36))
			if (copy_to_user(buf,fmhinfo, fwInfo_size))
				return -EFAULT;
			*offset+=fwInfo_size;
#else
			memcpy(buf,fmhinfo, fwInfo_size);
#endif
			return fwInfo_size;
		}
	}
	else
	{
		printk(KERN_ALERT "No MTD support\n");
	}
	/* Set End of File if no more data */
	/* Return length of data */
	//remember the fwinfo
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36))
	if (copy_to_user(buf,fmhinfo, fwInfo_size)) 
		return -EFAULT;

        *offset+=fwInfo_size;
#else
        memcpy(buf,fmhinfo, fwInfo_size);
#endif
	return fwInfo_size;
}


/*
   fwinfo2_read is a call back funtion to the fwinfo2 file. 
   For every read to fwinfo2 file, reads the image-2 fw info directly from mtd device.
*/
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36))
int
fwinfo2_read(struct file *file,  char __user *buf, size_t count, loff_t *offset)
#else
int
fwinfo2_read(char *buf, char **start, off_t offset, int count, int *eof, void *data)
#endif
{
	unsigned long fwInfo_size = 0;
	FLASH_INFO flinfo;
	char fmhinfo[256]={0};
/* We don't support seeked reads. read should start from 0 */
#if (LINUX_VERSION_CODE >=  KERNEL_VERSION(2,6,36))
        if (*offset != 0)
                return 0;
#else
        *eof=1;
        if (offset != 0)
                return 0;
#endif
#if (LINUX_VERSION_CODE <  KERNEL_VERSION(2,6,36))
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
#endif

	/* Find the SectorSize (erase size) of Flash */

	flinfo.read_flash = mtd_read_thunk;

	if (g_MTDSupport == 1)
	{
		/* Scan and Get FwInfo */
		unsigned long flashstartoffset;

                flashstartoffset = CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_START - CONFIG_SPX_FEATURE_GLOBAL_FLASH_START + CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_SIZE;		
		
		fwInfo_size = ScanFirmwareModule(flashstartoffset,
				CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_SIZE,
				CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE,
				(unsigned char*)fmhinfo,
				&flinfo);
	
		if ((signed long)fwInfo_size <= 0 )
		{
			printk(KERN_DEBUG "Firmware might be corrupted..\n");
			//FW might be corrupted, filling with dummy values.
			FillDummyFWinfo(fmhinfo);
#if (LINUX_VERSION_CODE <  KERNEL_VERSION(2,6,36))
                        *eof = 1;
#endif
                        fwInfo_size = strlen (fmhinfo);
#if (LINUX_VERSION_CODE >=  KERNEL_VERSION(2,6,36))
						if (copy_to_user(buf,fmhinfo, fwInfo_size))
							return -EFAULT;
                        *offset+=fwInfo_size;
#else
                        memcpy(buf,fmhinfo, fwInfo_size);
#endif
			return fwInfo_size;		
		}
	} 
	else
	{
		printk(KERN_ALERT "No MTD support\n");
	}

	/* Set End of File if no more data */
	/* Return length of data */
	//remember the fwinfo
#if (LINUX_VERSION_CODE >=  KERNEL_VERSION(2,6,36))
	if (copy_to_user(buf,fmhinfo, fwInfo_size)) 
		return -EFAULT;
	*offset+=fwInfo_size;
#else
    memcpy(buf,fmhinfo, fwInfo_size);
#endif
        return fwInfo_size;
}

#endif

void free_info_cache(void)
{
	if(g_cached_fwinfo_buf != NULL)
		kfree(g_cached_fwinfo_buf);
}


