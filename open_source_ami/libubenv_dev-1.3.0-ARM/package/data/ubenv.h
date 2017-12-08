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

#ifndef _UBENV_H
#define _UBENV_H

#include <Types.h>

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

#define MAX_UBOOT_VAR_LEN (32)

typedef struct environment_s {
    u32 crc;    /* CRC32 over data bytes    */
    u32 size;   /* Size of environment */
    u8 *data;
} env_t;


typedef struct UbEnv_tag
{
    char szVarName [ MAX_UBOOT_VAR_LEN ];
    char szValue [ MAX_UBOOT_VAR_LEN ];
} UbEnv_T;


/*---------------- UBoot-utility ---------------*/
int SetUBootEnv (UbEnv_T* pUb);
int PrintAllEnv (char* envBuf);
/* ---------------------------------------------*/

/*Functions to get/set a u-boot parameter */
int GetUBootParam (char*, char* );
int SetUBootParam (char*, char* );

#endif /* _UBENV_H */
