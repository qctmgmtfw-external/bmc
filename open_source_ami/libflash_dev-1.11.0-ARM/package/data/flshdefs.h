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
/* Filename:    flshdefs.h                              */
/* Author:      Anurag Bhatia                           */
/* Created:     12/14/2005                              */
/* Description: Flasher defs                            */
/***************************************************************/

#ifndef _FLASHER_DEFS_H_
#define _FLASHER_DEFS_H_

#define FLASH_REQUIRED		0x01
#define IMAGESIZE_CHANGE	0x02
#define VERSION_CHANGE		0x04

#define PRESERVE_CFG		0x01
#define RESET_BMC           0x02

#define FLASH_END_ADDRESS	0xFFFFFFFF

#define CURRENT_IMAGE_START_OFFSET		(UsedFlashStart - FlashStart)
#define UBOOT_ENV_START_OFFSET			(CONFIG_SPX_FEATURE_GLOBAL_UBOOT_ENV_START - CONFIG_SPX_FEATURE_GLOBAL_FLASH_START)

#define UPDATE_UBOOT		0x02

typedef enum {
	NORMAL,
	PREPARE_FLASH,
	IMAGE_VERIFIED,
	FLASH_START
} FlasherState;

typedef struct StructFlasherCmd {
unsigned char Command;
unsigned char Options;
} FlasherCmd;

typedef struct StructFlasherCmdResponse {
unsigned char Command;
unsigned char Status;
unsigned char Data[256];
unsigned short DataLen;
} FlasherCmdResponse;

typedef struct StructImageVerificationInfo {
unsigned char CurrentImageName[16];
unsigned char NewImageName[16];
unsigned long CurrentImageStartAdd;
unsigned long NewImageStartAdd;
unsigned long CurrentImageSize;
unsigned long NewImageSize;
unsigned char CurrentImageVersion[16];
unsigned char NewImageVersion[16];
unsigned char Reserved[20];
unsigned long Status;
} ImageVerificationInfo;

typedef struct StructImageVerificationResp {
ImageVerificationInfo ImagesCompareInfo;
unsigned char Status;
} ImageVerificationResponse;

/*  Prototypes  */
int GetSysCtlUlong (const char *TagName, unsigned long *SysVal);
int InitEnv(int mtd, unsigned long EnvOffset, unsigned long EnvSize);
int WriteEnv (int fd, unsigned long EnvOffset, unsigned long EnvSize, unsigned long EBlkSize);
int PrepareImageFlashArea(int RunLevel,unsigned char Cmd);
unsigned int VerifyImage (ImageVerificationInfo* ImgVeriInfo);
int DoFlash (unsigned char PreserveConfig);
int DoFlashOptions (unsigned char PreserveCfg, unsigned char UpdateUboot);
void SetFlashProgress(char* SubAction,char* Progress,int State);

#endif  /*  _FLASHER_DEFS_H_    */
