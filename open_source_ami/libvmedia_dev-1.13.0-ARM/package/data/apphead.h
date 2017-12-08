/****************************************************************
 **                                                            **
 **    (C)Copyright 2006-2009, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
****************************************************************/
#ifndef __APPHEAD_H__
#define __APPHEAD_H__



#include "usb_ioctl.h"
#include "iusb.h"

#include "icc_what.h"


/***************************Big to little conversions based on platform*******************/
//PLEASE REMEBER TO INCLUDE ICC_WHAT.H TO HAVE THIS AVAILABLE
#ifdef ICC_PLATFORM_BIG_ENDIAN
#define  mac2short(x)   ((u16) (((u16)(x) >> 8) | ((u16)(x) << 8)))
#define  mac2long(x)    (((x) >> 24) | ((x) << 24) | (((x) & 0x00ff0000) >> 8) | (((x) & 0x0000ff00) << 8))
#else
#define  mac2long(x)    (x)
#define  mac2short(x)   (x)
#endif
/******************************************************************************************/ 



/**** PORTS FOR VARIOUS USB SERVER AND CLIENTS...NEED TO DEPRECATE THIS BY USING COMMON PORT LOGIC ****/
#define IUSB_BASE_PORT		5120
#define IUSB_CD_PORT		(IUSB_BASE_PORT+0)
#define IUSB_HID_PORT		(IUSB_BASE_PORT+1)
#define IUSB_HD_PORT		(IUSB_BASE_PORT+2)
#define IUSB_FD_PORT		(IUSB_BASE_PORT+3)
#define IUSB_VF_PORT		(IUSB_BASE_PORT+8)
/******************************************************************************************************/ 

/**********KEEP ALIVE COMMANDS USED BY FLOPPY AND CD SERVER**********************/
//we have to implement a custom keep alive to detect connection closure properly
#define FLOPPY_KEEPALIVE_SCSI_CMD   0xF3
#define CDROM_KEEPALIVE_SCSI_CMD    0xF3
#define HDISK_KEEPALIVE_SCSI_CMD	0xF3
#define VFLASH_KEEPALIVE_SCSI_CMD	0xF3
#define HDISK_TYPE_SCSI_CMD			0xF4
//instead of demanding a ACK response form remote what we do is
//expect unsupported cmd response for this..so that we know remtoe is alive
//a custom commmand with a predefined response will only mean changing
//all clients.
//also TUR cannot be a valid keepalvie cmd..because that would prevent differenitaion
//between a genuine host TUR response and a keep alive response.
//also 0xF1 is in the vendor specific SCSI command range.

/*********************************** CD Buffer Related Defines ************************************/
#define CD_SECTOR_SIZE		2048
#define CD_MAX_READ_SECTORS 	64
#define CD_MAX_READ_SIZE	(CD_SECTOR_SIZE*CD_MAX_READ_SECTORS)
/******************************************************************************************************/ 



/********************************** HD Buffer Related Defines *************************************/
//this is not used now. Preserved for historical reasons
#define HD_SECTOR_SIZE		512
#define HD_MAX_READ_SECTORS 256
#define HD_MAX_READ_SIZE	(HD_SECTOR_SIZE*HD_MAX_READ_SECTORS)
/******************************************************************************************************/ 


/********************************** VF Buffer Related Defines *************************************/
#define VF_SECTOR_SIZE		512
#define VF_MAX_READ_SECTORS 256
#define VF_MAX_READ_SIZE	(VF_SECTOR_SIZE*VF_MAX_READ_SECTORS)
/******************************************************************************************************/ 


/*********************************** FD Buffer Related Defines ************************************/
#define FD_SECTOR_SIZE		512
//#define FD_MAX_READ_SECTORS 128
#define FD_MAX_READ_SECTORS 256
#define FD_MAX_READ_SIZE	(FD_SECTOR_SIZE*FD_MAX_READ_SECTORS)
/******************************************************************************************************/ 


/****The usb device to open to acess usb driver****/
#define USB_DEVICE			"/dev/usb"

/***************************************Names of PID files*********************************************/ 
#define CD_PID_FILE			"/var/run/cdserver.pid"
#define HD_PID_FILE			"/var/run/hdserver.pid"
#define HID_PID_FILE		"/var/run/hidserver.pid"
#define FD_PID_FILE			"/var/run/fdserver.pid"
#define VF_PID_FILE			"/var/run/vfserver.pid"
/******************************************************************************************************/ 



/******************************* Host configuration Related ******************************************/
//check on this...is this used??????
#define CFG_READ 	0
#define CFG_WRITE 	1
#define CMD_REG 	0
#define DATA_REG 	1
/*****************************************************************************************************/ 





#endif /* __APPHEAD_H__ */
