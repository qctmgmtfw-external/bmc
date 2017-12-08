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

/***************************************************************/
/* Filename:    flshfiles.h                             */
/* Author:      Baskar Parthiban                        */
/* Created:     09/25/2003                              */
/* Description: Important file locations for flasher    */
/*              May also contain other shareable info   */
/**************************************************************/

#ifndef _FLASHER_FILES_H_
#define _FLASHER_FILES_H_


/*  SysCtl Entries exported by 'helper' kernel module */
#define SYSCTL_FLASH_START  "/proc/sys/ractrends/Helper/FlashStart"
#define SYSCTL_FLASH_SIZE   "/proc/sys/ractrends/Helper/FlashSize"
#define SYSCTL_FLASH_SECTOR_COUNT   "/proc/sys/ractrends/Helper/FlashSectorCount"
#define SYSCTL_FLASH_SECTOR_SIZE   "/proc/sys/ractrends/Helper/FlashSectorSize"
#define SYSCTL_ENV_START    "/proc/sys/ractrends/Helper/EnvStart"
#define SYSCTL_ENV_SIZE     "/proc/sys/ractrends/Helper/EnvSize"
#define SYSCTL_USED_FLASH_START "/proc/sys/ractrends/Helper/UsedFlashStart"
#define SYSCTL_USED_FLASH_SIZE "/proc/sys/ractrends/Helper/UsedFlashSize"

/*  device name used by flasher to access entire flash partition    */
#define MTDDEVICE   "/dev/mtd0"

/*  All other programs see flasher's /... as /mnt/... so we use
    the prefix /mnt to all file locations, for use by such programs */
#define ADD_MNT_PREFIX(XYZ)   "/mnt"XYZ


/*  These macros (FLASHER_XXX) are for use by the flasher program   */
#define FLASHER_IMAGEFILE   "/mnt/rom.ima"
#define FLASHER_PROGRESS_FILE "/mnt/flasher.progress"

#define IMAGEFILE    FLASHER_IMAGEFILE

//these are flasher completion files that we use for determining if some operation
//has completed..right now we just wait for creation of file..
//later we can gracefully lock on the files etc
#define FLASHER_COMPLETION_FILE_INIT7 "/var/flasher.initcomplete"
#define FLASHER_COMPLETION_FILE_CPCONF "/var/flasher.cpconfcomplete"
#define FLASHER_USB_FLASHING_FILE		"/var/flasher.usb"
#define FLASHER_SAVE_IPMI_CONFIG "/var/ipmiflushcomplete"

/*  Command and status pipes between flasher and driving program    */
#define FLASHER_PIPE        "/var/pipe/flasher_cmd"
#define FLASHER_REPLY_PIPE  "/var/pipe/flasher_reply"
#define FLASHER_IPMICONFIGSREQQ "/var/pipe/ipmiReqQ"

/*  Status string denoting successful completion of flash process   */
#define FLASH_SUCCESS_STRING    "flash: Firmware Upgrade Successful"

/*  Flasher Commands and Responses - all are one byte values */
#define FLSH_CMD_PREP_FLASH_AREA    ((unsigned char)0x01)
#define FLSH_CMD_VERIFY_IMAGE       ((unsigned char)0x02)
#define FLSH_CMD_START_FLASH        ((unsigned char)0x03)
#define FLSH_CMD_ABORT_FLASH        ((unsigned char)0x04)

#define FLSH_CMD_PREP_FLASH_AREA_USB ((unsigned char)0x05)
#define FLSH_CMD_START_FLASH_OPTIONS ((unsigned char)0x06)
#define FLSH_CMD_PREP_YAFU_FLASH_AREA ((unsigned char)0x07)
#define FLSH_CMD_DUMMY_FLASH_AREA ((unsigned char)0x08)

#ifdef CONFIG_SPX_FEATURE_GLOBAL_DUAL_IMAGE_SUPPORT
  #define FLSH_DUAL_IMAGE_OPTIONS ((unsigned char)0x09)
#endif

#define FLSH_STATUS_SUCCESS         ((unsigned char)0x00)
#define FLSH_STATUS_FAILURE         ((unsigned char)0x01)
#define FLSH_STATUS_INVSTATE         ((unsigned char)0x02)

#define WEB_RUNLEVEL            0x08
#define YAFU_RUNLEVEL           0x07
#define USB_RUNLEVEL            0x07
#endif  /*  _FLASHER_FILES_H_    */
