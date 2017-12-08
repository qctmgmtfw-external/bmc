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
#ifndef _FMH_INFO_H_
#define _FMH_INFO_H_

//#include "Types.h"

#define MAX_ERASE_BLOCK_SIZE   CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE

/*  Other defines   */
#define UPG_NO      0
#define UPG_YES     1
#define UPG_FULL    2
#define UPG_REMOVE  3
#define UPG_ADD		4
#define UPG_LOC		5
#define UPG_SIZE	6

#define MAX_MODULES 30  /* Maximum # of modules in one image    */

#define ERASE_BLOCK_SIZE	CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE

#define FLASH_IMAGE_SIZE   CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_SIZE

#define DATA_PACKET_SIZE	(32*1024)	

#define	TOTAL_PACKETS_XMIT_IN_A_BLOCK	(ERASE_BLOCK_SIZE/DATA_PACKET_SIZE)

#ifdef __GNUC__
#define PACKED __attribute__ ((packed))
#else
#define PACKED
#pragma pack( 1 )
#endif

/*  Data Structures  */
typedef struct  ModInfo
{
    /*  FMH Attributes  */
    unsigned long   FMH_Location;
    unsigned char   FMH_Ver_Major;
    unsigned char   FMH_Ver_Minor;
    unsigned long   FMH_AllocatedSize;

    /*  Module Attributes  */
    unsigned char   Module_Name[9];
    unsigned char   Module_Ver_Major;
    unsigned char   Module_Ver_Minor;
    unsigned char   Module_Ver_Micro;
    unsigned long   Module_Location;        /* Offset to Module from start */
    unsigned long   Module_Size;
    unsigned short  Module_Flags;           /* Refer Module Flags Values in fmh.h */
    unsigned short  Module_Type;            /* Refer Module Type Values in fmh.h */
    unsigned long   Module_Checksum;        /* CRC 32 */

    /*  Flasher Flags   */
    unsigned char   PresentInBoth;
    unsigned char   InvalidChecksum;

}   PACKED MI_t;

typedef struct  BlockInfo
{
    unsigned long BlockStartAdd;
    unsigned char BlockData[DATA_PACKET_SIZE];
    unsigned long CRC32;
    unsigned char EndOfData;
}  PACKED BI_t;

typedef struct
{
	unsigned long	FlashStartAddr;
	unsigned long	UsedFlashStartAddr;
	unsigned long	ConfigStartAddr;
	unsigned long  	Reserved;
}  PACKED FLASH_LAYOUT;

typedef struct  FlashParamsInfo
{
   // char MacAddress[MAX_MAC_LEN];
   unsigned long FlashStartAddr;
   unsigned long FlashSize;
   unsigned long UsedFlashStartAddr;
   unsigned long UsedFlashSize; // Current image size
   unsigned long EnvStartAddr;
   unsigned long EnvSize;
   unsigned long EraseBlockSize;   
   unsigned char CurrentImageMajor;
   unsigned char CurrentImageMinor;
   unsigned char CurrentImageName[16];
   unsigned char RetainConfig;
   unsigned char UpdateUBoot;
}  PACKED FLASH_PARAMS;

#if 0
typedef struct  BlockInfo2
{
    unsigned long BlockStartAdd;
    unsigned long CRC32;
    unsigned char SysbiosBlock;
    unsigned char EndOfData;
    unsigned short BlockDataLen;
    unsigned char BlockData[MAX_ERASE_BLOCK_SIZE];
}  PACKED BI2_t;
#endif

#ifndef __GNUC__
#pragma pack()
#endif

/*  Prototypes  */
int UpgradeBlock(unsigned char *blkData, unsigned int blkStartAdd);
//int CompareFlashBlockData(BI_t *blk);
//int ReadBlock(unsigned long BlkAddress,char *DataBuffer);
unsigned int ActualEraseBlockSize(void);
int BackupConfig(void);
int RestoreConfig(void);
int InitFlash(FLASH_PARAMS *fp);
int ExitFlash(FLASH_PARAMS *fp);
//int GetFlashLayout(FLASH_LAYOUT *flashLayout);
//unsigned long CalculateChksum (char *data, unsigned long size);
int RestartDeviceWithNewFirmware (void);

#endif  /*  _FMH_INFO_H_    */
