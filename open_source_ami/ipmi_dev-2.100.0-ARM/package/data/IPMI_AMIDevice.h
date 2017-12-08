/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2005-2006, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        6145-F, Northbelt Parkway, Norcross,                **
 **                                                            **
 **        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 ****************************************************************
 ****************************************************************
 *
 * IPMI_AMI.h
 * AMI specific IPMI Commands
 *
 * Author: Basavaraj Astekar <basavaraja@ami.com>
 *
 *****************************************************************/
#ifndef IPMI_AMI_DEVICE_H_
#define IPMI_AMI_DEVICE_H_

#include "Types.h"
#include "coreTypes.h"
#include "fmhinfo.h"
#include "flashlib.h"
#include "IPMI_SDRRecord.h"


#define MAX_LINUX_ROOT_USER_PASSWORD_LEN        64
#define MAX_SOL_PORT_STR_LEN        			16
/*Number of PAM module are going to re-ordered*/
#define MODULES_TO_MODIFIED  3

#define CRITICAL                                0x1
#define NON_CRITICAL                            0x2
#define NON_RECOVERABLE                         0x3
#define FAN_TROUBLED                            0x4
#define BMC_WDT_EXPIRE                          0x5
#define SYS_DC_ON                               0x6
#define SYS_DC_OFF                              0x7
#define SYS_RESET                               0x8
#define SPEC_DATE_TIME                          0x9
#define LPC_RESET_FLAG                          0xa

/*
 * User roles( for command auth. purposes). Currently used by AMISetLEDmode.
 */
typedef enum {
	REGULAR,
	ADMIN,
	POLICY
} user_role_t;

/*
 * LEDs
 */
typedef enum {
	OK2RM  = 0,
	SERVICE  = 1,
	ACT  = 2,
	LOCATE = 3
} LEDtype_t;

/*
 * LED modes
 */
typedef enum {
	LED_OFF  = 0,
	LED_ON = 1,
	LED_STANDBY_BLINK  = 2,
	LED_SLOW_BLINK  = 3,
	LED_FAST_BLINK  = 4
} LEDmode_t;

/* Define your IPMI request and response structures here */

#pragma pack(1)
/**
 * @struct AMICommandReq_T
 * @brief AMI command request structure
 **/
typedef struct
{
    /* define your structures here */
	INT8U	dummy;

} PACKED  AMITestCmdReq_T;

/**
 * @struct AMICommandRes_T
 * @brief AMI command response structure
 **/
typedef struct
{
    /* define your structures here */
	INT8U	dummy;

} PACKED  AMITestCmdRes_T;


//#define MAX_IPMI_UPLOAD_BLOCK   ( 64 * 1024 )
#define MAX_IPMI_UPLOAD_BLOCK   ( 100 )

/**
 * @struct AMISetSSHKeyReq_T
 * @brief AMI Set SSH Key command request structure
 **/
typedef struct
{
    /* UserID of the user whose ssh key we're uploading */
    INT8U   UserID;

    /* Zero-indexed block number for upload.  Set to 0xff for the last */
    /* block.                                                          */
    INT8U   KeyDataBlockNumber;

    /* Amount of data in this block.  Should be max block size for all */
    /* but the last block when the total key size is not a multiple of */
    /* the maximum block size.                                         */
    INT8U   DataLen;

    INT8U   Data[ MAX_IPMI_UPLOAD_BLOCK ];
} PACKED AMISetSSHKeyReq_T;


/**
 * @struct AMISetSSHKeyRes_T
 * @brief AMI Set SSH Key command response structure
 **/
typedef struct
{
    INT8U CompletionCode;
} PACKED AMISetSSHKeyRes_T;


/**
 * @struct AMIDelSSHKeyReq_T
 * @brief AMI Delete SSH Key command request structure
 **/
typedef struct
{
    /* UserID of the user whose ssh key we're deleting */
    INT8U   UserID;
} PACKED AMIDelSSHKeyReq_T;

/**
 * @struct AMIDelSSHKeyRes_T
 * @brief AMI Delete SSH Key command response structure
 **/
typedef struct
{
    INT8U CompletionCode;
} PACKED AMIDelSSHKeyRes_T;


/**
 * @struct AMIUpgradeBlockReq_T
 * @brief AMI Upgrade Block command request structure
 **/
typedef struct
{
    /* Block information */
    BI_t   blkInfo;
} PACKED AMIUpgradeBlockReq_T;

/**
 * @struct AMIUpgradeBlockRes_T
 * @brief AMI Upgrade Block command response structure
 **/
typedef struct
{
    INT8U CompletionCode;
} PACKED AMIUpgradeBlockRes_T;


/* Init Flash */
typedef struct
{
    FLASH_PARAMS params;
} PACKED AMIInitFlashReq_T;

typedef struct
{
    INT8U CompletionCode;
    FLASH_PARAMS params;
} PACKED AMIInitFlashRes_T;

/* Exit Flash */
typedef struct
{
    FLASH_PARAMS params;
} PACKED AMIExitFlashReq_T;

/* Exit Flash */
typedef struct
{
    FLASH_PARAMS params;
} PACKED AMIStartFirmwareUpdateReq_T;


typedef struct
{
    INT8U CompletionCode;
} PACKED AMIExitFlashRes_T;
/* Get Flash Layout */
typedef struct
{
    FLASH_LAYOUT FlashLayout;
} PACKED AMIGetFlashLayoutReq_T;

typedef struct
{
    INT8U CompletionCode;
    FLASH_LAYOUT FlashLayout;
} PACKED AMIGetFlashLayoutRes_T;

typedef struct
{
    INT8U CompletionCode;
} PACKED  AMIResetCardRes_T;

typedef struct
{
    INT8U CompletionCode;
} PACKED  AMIUpdateUbootRes_T;

typedef struct
{
    INT8U CompletionCode;
} PACKED  AMISetFirmwareUpdateModeRes_T;

typedef struct
{
    INT8U CompletionCode;
} PACKED  AMIStartFirmwareUpdateRes_T;

typedef struct
{
    FLASH_PARAMS params;
} PACKED AMIGetFirmwareUpdateStatusReq_T;


typedef struct
{
    INT8U EraseInProgress;
    INT8U CurrentSectorErased;
    INT8U TotalSectorstobeErased;
    INT8U WriteInProgress;
    INT8U CurrentSectorWritten;
    INT8U TotalSectorstobeWritten;
    INT8U EraseWriteCompleted;
    
} PACKED AMIFlashStatus_T;

typedef struct
{
    INT8U CompletionCode;
    STRUCTURED_FLASH_PROGRESS flprog;

} PACKED AMIGetFirmwareUpdateStatusRes_T;

typedef struct
{
	int TimeOut;
	int FlashProcessStarted;

} PACKED FlashTimerInfo_T;

typedef struct
{
    INT8U FanSpeed;      /* Fan speed in percentage */
} PACKED  AMISetFanSpeedReq_T;

typedef struct
{
    INT8U CompletionCode;
} PACKED  AMISetFanSpeedRes_T;

/**
 * @struct AMIGetLEDmodeReq_T
 * @brief AMI Get LED Mode command request structure
 **/
typedef struct
{
    INT8U   devAddr; /* value from the "Device Slave Address" field in, */
                     /* LED's Generic Device Locator record, in the SDR     */
    INT8U   led;
    INT8U   ctrlrAddr; /* Its controller's address; value from the          */
                       /* "Device Access Address" field. Zero if the LED is */
                       /* local.                               */
    INT8U   hwInfo;  /* the OEMField from the SDR record */
    INT8U   force;     /* TRUE - directly access the device, FALSE - go     */
                       /* thru its controller. Ignored if LED is local.     */
} PACKED  AMIGetLEDmodeReq_T;

/**
 * @struct AMIGetLEDmodeRes_T
 * @brief AMI Get LED Mode command response structure
 **/
typedef struct
{
    INT8U CompletionCode;
    INT8U mode;
} PACKED  AMIGetLEDmodeRes_T;

/**
 * @struct AMISetLEDmodeReq_T
 * @brief AMI Set LED Mode command request structure
 **/
typedef struct
{
    INT8U   devAddr; /* value from the "Device Slave Address" field in, */
                     /* LED's Generic Device Locator record, in the SDR     */
    INT8U   led;
    INT8U   ctrlrAddr; /* Its controller's address; value from the          */
                       /* "Device Access Address" field. Zero if the LED is */
                       /* local.                               */
    INT8U   hwInfo;  /* the OEMField from the SDR record */
    INT8U   mode;
    INT8U   force;     /* TRUE: directly access the device. FALSE: go thru  */
                       /* its controller. Ignored if LED is local.          */
    INT8U   role; /* This is used by BMC, for authorization purposes   */
} PACKED  AMISetLEDmodeReq_T;

/**
 * @struct AMISetLEDmodeRes_T
 * @brief AMI Set LED Mode command response structure
 **/
typedef struct
{
    INT8U CompletionCode;
} PACKED  AMISetLEDmodeRes_T;

#define FRUDATA_MAX_SIZE	0x80 /* 128 bytes */
#define FRUDATA_TYPE_DIMM	0x00
#define FRUDATA_TYPE_CPU	0x01
#define FRUDATA_TYPE_BIOS	0x02

/**
 * @struct FRUData_DIMM_T
 * @brief FRU Data for DIMM SPD information sent from BIOS in two blocks
 **/
typedef struct
{
    INT8U SPDData[128];		/* 128 bytes */
} PACKED  FRUData_DIMM_T;		/* 128 bytes total */

/**
 * @struct FRUData_CPU_T
 * @brief FRU Data for Processor information sent from BIOS
 **/
typedef struct
{
    uint32 ThermTrip;
    uint32 cpuid1_eax;
    uint32 cpuid_product_name[12];
} PACKED  FRUData_CPU_T;		/* 56 bytes total */

/**
 * @struct FRUData_BIOS_T
 * @brief FRU Data for BIOS information sent from BIOS
 **/
typedef struct
{
    INT8U PartNumber[16];	/* 16 bytes */
    INT8U PartVersion[16];	/* 16 bytes */
} PACKED  FRUData_BIOS_T;		/* 32 bytes total */

/**
 * @struct AMITransferFRUDataReq_T
 * @brief OEM request for transferring FRU data from BIOS
 */
typedef struct
{
    INT8U Type;                    /* identifier for this FRU data */
    INT8U Number;                  /* identifier for different datas of same type */
    INT8U DataLen;                 /* length of data block */
    INT8U Data[FRUDATA_MAX_SIZE];  /* data of data */
} PACKED  AMITransferFRUDataReq_T;

/**
 * @struct AMITransferFRUDataRes_T
 * @brief OEM response for transferring FRU data from BIOS
 */
typedef struct
{
    INT8U CompletionCode;
} PACKED  AMITransferFRUDataRes_T;


/**
 * @struct YafuHeader
 * @brief Flash info structure
 */
 typedef struct
{
    uint32  Seqnum;
    INT16U  YafuCmd;
    INT16U  Datalen;
    uint32  CRC32chksum;
} PACKED YafuHeader;
	
/**
 * @struct AMIYAFUNotAck
 * @brief Flash info structure
 */
typedef struct
{
   INT8U CompletionCode;
   YafuHeader NotAck;
   INT16U       ErrorCode;
}PACKED AMIYAFUNotAck;

/**
 * @struct ALT_FMH
 * @brief Flash info structure
 */

typedef struct
{
    INT16U EndSignature;
    INT8U   HeaderChkSum;
    INT8U   Reserved;
    uint32 LinkAddress;
    INT8U   Signature[8]; 	
} PACKED ALT_FMHead;



/**
 * @struct FMH
 * @brief Flash info structure
 */

	typedef struct
	{
		INT8U		FmhSignature[8];
		union
		{
			INT16U	FwVersion;
			INT8U	FwMinorVer;
			INT8U	FwMajorVer;
		} PACKED Fmh_Version;
		INT16U		FmhSize;
		uint32		AllocatedSize;
		uint32		FmhLocation;
		INT8U		FmhReserved[3];
		INT8U		HeaderChecksum;
		INT8U		ModuleName[8];
		union
		{
			INT16U	ModVersion;
			INT8U	ModMinorVer;
			INT8U	ModMajorVer;
		}PACKED Module_Version;
		INT16U		ModuleType;
		uint32		ModuleLocation;
		uint32		ModuleSize;
		INT16U		ModuleFlags;
		uint32		ModuleLoadAddress;
		uint32		ModuleChecksum;
		INT8U		ModuleAuxVer[2];
		INT8U		ModuleReserved[6];
		INT16U		EndSignature;
	}PACKED  FlashMH;

/**
 * @struct AMIYAFUGetFlashInfoReq_T
 * @brief Flash info structure
 */
 
typedef struct
{
    YafuHeader FlashInfoReq;

} PACKED AMIYAFUGetFlashInfoReq_T;


typedef struct
{

    uint32  FlashSize;
    uint32  FlashAddress;
    uint32  FlashEraseBlkSize;
    INT16U  FlashProductID;
    INT8U    FlashWidth;	
    INT8U    FMHCompliance;
    INT16U  Reserved;
    INT16U  NoEraseBlks;	 
	
}PACKED FlashDetails;


/**
 * @struct AMIYAFUGetFlashInfoRes_T
 * @brief Flash info structure
 */
typedef struct
{
    INT8U           CompletionCode;
    YafuHeader   FlashInfoRes;
    FlashDetails  FlashInfo;

} PACKED AMIYAFUGetFlashInfoRes_T;	

/**
 * @struct AMIYAFUGetFirmwareInfoReq_T
 * @brief Flash info structure
 */
typedef struct
{
    YafuHeader FirmwareinfoReq;

} PACKED AMIYAFUGetFirmwareInfoReq_T;	

/**
 * @struct FirmInfo
 * @brief Flash info structure
 */
typedef struct
{
    INT8U     FirmMajVersion;
    INT8U     FirmMinVersion;
    INT16U   FirmAuxVersion;	
    uint32   FirmBuildNum;	
    uint32   Reserved;
    INT8U     FirmwareName[8];	
    uint32   FirmwareSize;
    uint32   ProductID;
	
} PACKED FirmInfo;

/**
 * @struct AMIYAFUGetFirmwareInfoRes_T
 * @brief Flash info structure
 */
typedef struct
{
    INT8U         CompletionCode;
    YafuHeader FirmwareinfoRes;	
    FirmInfo      FirmwareInfo;

} PACKED AMIYAFUGetFirmwareInfoRes_T;	


/**
 * @struct AMIYAFUGetFMHInfoRes_T
 * @brief Flash info structure
 */
typedef struct
{
     YafuHeader FMHReq;

} PACKED AMIYAFUGetFMHInfoReq_T;	

/**
 * @struct AMIYAFUGetFMHInfoRes_T
 * @brief Flash info structure
 */
typedef struct
{
    INT8U         CompletionCode; 
    YafuHeader FMHRes;
    INT16U       Reserved;	
    INT16U       NumFMH; 	
              

} PACKED AMIYAFUGetFMHInfoRes_T;		

/**
 * @struct AMIYAFUGetStatusReq_T
 * @brief Flash info structure
 */
typedef struct
{
     YafuHeader GetStatusReq;

} PACKED AMIYAFUGetStatusReq_T;	


/**
 * @struct AMIYAFUGetStatusRes_T
 * @brief Flash info structure
 */
typedef struct
{
    INT8U         CompletionCode; 
    YafuHeader GetStatusRes;
    INT16U       LastStatusCode;
    INT16U       YAFUState;
    INT16U         Mode;
    INT16U       Reserved;
    INT8U         Message[65];	   
	    
} PACKED AMIYAFUGetStatusRes_T;	

/**
 * @struct AMIYAFUActivateFlashModeReq_T
 * @brief Flash info structure
 */
typedef struct
{
    YafuHeader ActivateflashReq;
    INT16U       Mode;	

} PACKED AMIYAFUActivateFlashModeReq_T;	

/**
 * @struct AMIYAFUGetStatusRes_T
 * @brief Flash info structure
 */
typedef struct
{
    INT8U         CompletionCode; 
    YafuHeader ActivateflashRes;
    INT8U         Delay;	
    	
}PACKED AMIYAFUActivateFlashModeRes_T;		

/**
 * @struct AMIYAFUAllocateMemoryReq_T
 * @brief Flash info structure
 */
typedef struct
{
    YafuHeader AllocmemReq;
    uint32       Sizeofmemtoalloc;	

} PACKED AMIYAFUAllocateMemoryReq_T;	


/**
 * @struct AMIYAFUAllocateMemoryRes_T
 * @brief Flash info structure
 */
typedef struct
{
    INT8U         CompletionCode; 
    YafuHeader AllocmemRes;
    uint32       Addofallocmem; 	

}PACKED AMIYAFUAllocateMemoryRes_T;		


/**
 * @struct AMIYAFUFreeMemoryReq_T
 * @brief Flash info structure
 */
typedef struct
{
    YafuHeader FreememReq;
    uint32       AddrtobeFreed;	

} PACKED AMIYAFUFreeMemoryReq_T;	

/**
 * @struct AMIYAFUFreeMemoryRes_T
 * @brief Flash info structure
 */
typedef struct
{
    INT8U         CompletionCode; 
    YafuHeader FreememRes;
    INT8U         Status; 	

}PACKED AMIYAFUFreeMemoryRes_T;		

/**
 * @struct AMIYAFUReadFlashReq_T
 * @brief Flash info structure
 */
typedef struct
{
    YafuHeader ReadFlashReq;
    uint32       offsettoread;	
    INT8U         Readwidth;
    uint32       Sizetoread;	 

} PACKED AMIYAFUReadFlashReq_T;	

/**
 * @struct AMIYAFUReadFlashRes_T
 * @brief Flash info structure
 */
typedef struct
{
    INT8U         CompletionCode; 
    YafuHeader ReadFlashRes;
    //INT8U         FlashContent[0x4000];	
	
}PACKED AMIYAFUReadFlashRes_T;		

/**
 * @struct AMIYAFUWriteFlashReq_T
 * @brief Flash info structure
 */
typedef struct
{
    YafuHeader WriteFlashReq;
    uint32 offsettowrite;	
    INT8U   Writewidth;
    //INT8U   Buffer[0x4000];	
  	 

} PACKED AMIYAFUWriteFlashReq_T;	

/**
 * @struct AMIYAFUWriteFlashRes_T
 * @brief Flash info structure
 */
typedef struct
{
    INT8U   CompletionCode; 
    YafuHeader WriteFlashRes;
    INT16U SizeWritten; 	
}PACKED AMIYAFUWriteFlashRes_T;		

/**
 * @struct AMIYAFUErashFlashReq_T
 * @brief Flash info structure
 */
typedef struct
{
    YafuHeader EraseFlashReq;
    uint32 Blknumtoerase;	
     	
} PACKED AMIYAFUErashFlashReq_T;


/**
 * @struct AMIYAFUErashFlashRes_T
 * @brief Flash info structure
 */
typedef struct
{
    INT8U   CompletionCode; 
    YafuHeader EraseFlashRes;
    INT8U   Status; 	

}PACKED AMIYAFUErashFlashRes_T;	

/**
 * @struct AMIYAFUProtectFlashReq_T
 * @brief Flash info structure
 */
typedef struct
{
    YafuHeader ProtectFlashReq;
    uint32 Blknum;
    INT8U   Protect;			
     	
} PACKED AMIYAFUProtectFlashReq_T;

/**
 * @struct AMIYAFUProtectFlashRes_T
 * @brief Flash info structure
 */
typedef struct
{
    INT8U   CompletionCode; 
    YafuHeader ProtectFlashRes;	
    INT8U   Status; 	

}PACKED AMIYAFUProtectFlashRes_T;

/**
 * @struct AMIYAFUEraseCopyFlashReq_T
 * @brief Flash info structure
 */
typedef struct
{
    YafuHeader EraseCpyFlashReq;
    uint32 Memoffset;
    uint32 Flashoffset;
    uint32 Sizetocopy;	
     	
} PACKED AMIYAFUEraseCopyFlashReq_T;

/**
 * @struct AMIYAFUEraseCopyFlashRes_T
 * @brief Flash info structure
 */
typedef struct
{
    INT8U   CompletionCode; 
    YafuHeader EraseCpyFlashRes;	
    uint32 Sizecopied; 	

}PACKED AMIYAFUEraseCopyFlashRes_T;

/**
 * @struct AMIYAFUGetECFStatusReq_T
 * @brief Flash info structure
 */
typedef struct
{
    YafuHeader GetECFStatusReq;
         	
} PACKED AMIYAFUGetECFStatusReq_T;

/**
 * @struct AMIYAFUGetECFStatusRes_T
 * @brief Flash info structure
 */
typedef struct
{
    INT8U   CompletionCode; 
    YafuHeader GetECFStatusRes;
	uint32 Status;
	INT16U Progress;
}PACKED AMIYAFUGetECFStatusRes_T;

/**
 * @struct AMIYAFUVerifyFlashReq_T
 * @brief Flash info structure
 */
typedef struct
{
    YafuHeader VerifyFlashReq;
    uint32 Memoffset;
    uint32 Flashoffset;
    uint32 Sizetoverify;	
     	
} PACKED AMIYAFUVerifyFlashReq_T;

/**
 * @struct AMIYAFUVerifyFlashRes_T
 * @brief Flash info structure
 */
typedef struct
{
    INT8U   CompletionCode; 
    YafuHeader VerifyFlashRes;	
    uint32 Offset; 	

}PACKED AMIYAFUVerifyFlashRes_T;
/**
 * @struct AMIYAFUGetVerifyStatusReq_T
 * @brief Flash info structure
 */
typedef struct
{
    YafuHeader GetVerifyStatusReq;
         	
} PACKED AMIYAFUGetVerifyStatusReq_T;

/**
 * @struct AMIYAFUGetVerifyStatusRes_T
 * @brief Flash info structure
 */
typedef struct
{
    INT8U   CompletionCode; 
    YafuHeader GetVerifyStatusRes;
	uint32 Status;
	uint32 Offset; 	
	INT16U Progress;
}PACKED AMIYAFUGetVerifyStatusRes_T;

/**
 * @struct AMIYAFUReadMemoryReq_T
 * @brief Flash info structure
 */
typedef struct
{
    YafuHeader ReadMemReq;
    uint32 Memoffset;
    INT8U   ReadWidth;
    INT16U Sizetoread;	
     	
} PACKED AMIYAFUReadMemoryReq_T;

/**
 * @struct AMIYAFUReadMemoryRes_T
 * @brief Flash info structure
 */
typedef struct
{
    INT8U   CompletionCode; 
    YafuHeader ReadMemRes;	
       
}PACKED AMIYAFUReadMemoryRes_T;

/**
 * @struct AMIYAFUWriteMemoryReq_T
 * @brief Flash info structure
 */
typedef struct
{
    YafuHeader WriteMemReq;
    uint32 Memoffset;
    INT8U   WriteWidth; 
         	
} PACKED AMIYAFUWriteMemoryReq_T;

/**
 * @struct AMIYAFUWriteMemoryRes_T
 * @brief Flash info structure
 */
typedef struct
{
    INT8U   CompletionCode; 
    YafuHeader WriteMemRes;	
    INT16U SizeWritten;
       
}PACKED AMIYAFUWriteMemoryRes_T;

/**
 * @struct AMIYAFUCopyMemoryReq_T
 * @brief Flash info structure
 */
typedef struct
{
    YafuHeader CopyMemReq;
    uint32 MemoffsetSrc;
    uint32 MemoffsetDest;	
    uint32 Sizetocopy;	
         	
} PACKED AMIYAFUCopyMemoryReq_T;

/**
 * @struct AMIYAFUCopyMemoryRes_T
 * @brief Flash info structure
 */
typedef struct
{
    INT8U   CompletionCode; 
    YafuHeader CopyMemRes;
    uint32 Sizecopied;
       
}PACKED AMIYAFUCopyMemoryRes_T;

/**
 * @struct AMIYAFUCompareMemoryReq_T
 * @brief Flash info structure
 */
typedef struct
{
    YafuHeader CmpMemReq;
    uint32 Memoffset1;
    uint32 Memoffset2;	
    uint32 SizetoCmp;	
         	
} PACKED AMIYAFUCompareMemoryReq_T;

/**
 * @struct AMIYAFUCompareMemoryRes_T
 * @brief Flash info structure
 */
typedef struct
{
    INT8U   CompletionCode; 
    YafuHeader CmpMemRes;
    uint32 Offset;
       
}PACKED AMIYAFUCompareMemoryRes_T;

/**
 * @struct AMIYAFUClearMemoryReq_T
 * @brief Flash info structure
 */
typedef struct
{
    YafuHeader ClearMemReq;
    uint32 MemofftoClear;
    uint32 SizetoClear;	
         	
} PACKED AMIYAFUClearMemoryReq_T;

/**
 * @struct AMIYAFUClearMemoryRes_T
 * @brief Flash info structure
 */
typedef struct
{
    INT8U   CompletionCode; 
    YafuHeader ClearMemRes;
    uint32 SizeCleared;
       
}PACKED AMIYAFUClearMemoryRes_T;

/**
 * @struct AMIYAFUGetBootConfigReq_T
 * @brief Flash info structure
 */
typedef struct
{
    YafuHeader GetBootReq;
    INT8U         VarName[65];  	
             	
} PACKED AMIYAFUGetBootConfigReq_T;

/**
 * @struct AMIYAFUGetBootConfigRes_T
 * @brief Flash info structure
 */
typedef struct
{
    INT8U         CompletionCode; 
    YafuHeader GetBootRes;
    INT8U         Status;	
   	
}PACKED AMIYAFUGetBootConfigRes_T;

/**
 * @struct AMIYAFUSetBootConfigReq_T
 * @brief Flash info structure
 */
typedef struct
{
    YafuHeader SetBootReq;
    INT8U         VarName[65];	
                	  
} PACKED AMIYAFUSetBootConfigReq_T;

/**
 * @struct AMIYAFUSetBootConfigRes_T
 * @brief Flash info structure
 */
typedef struct
{
    INT8U         CompletionCode; 
    YafuHeader SetBootRes;
    INT8U         Status;	
    	
}PACKED AMIYAFUSetBootConfigRes_T;

/**
 * @struct AMIYAFUGetBootVarsReq_T
 * @brief Flash info structure
 */
typedef struct
{
    YafuHeader GetBootReq;
                  	  
} PACKED AMIYAFUGetBootVarsReq_T;

/**
 * @struct AMIYAFUGetBootVarsRes_T
 * @brief Flash info structure
 */
typedef struct
{
    INT8U         CompletionCode; 
    YafuHeader GetBootRes;
    INT8U         VarCount;
    	
}PACKED AMIYAFUGetBootVarsRes_T;



/**
 * @struct AMIYAFUDeactivateFlashReq_T
 * @brief Flash info structure
 */
typedef struct
{
    YafuHeader DeactivateFlashReq;
               	  
} PACKED AMIYAFUDeactivateFlashReq_T;

/**
 * @struct AMIYAFUDeactivateFlashRes_T
 * @brief Flash info structure
 */
typedef struct
{
    INT8U         CompletionCode; 
    YafuHeader DeactivateFlashRes;
    INT8U         Status;	
	    	
}PACKED AMIYAFUDeactivateFlashRes_T;

/**
 * @struct AMIYAFUResetDeviceReq_T
 * @brief Flash info structure
 */
typedef struct
{
    YafuHeader ResetReq;
    INT16U       WaitSec;	
               	  
} PACKED AMIYAFUResetDeviceReq_T;

/**
 * @struct AMIYAFUResetDeviceRes_T
 * @brief Flash info structure
 */
typedef struct
{
    INT8U         CompletionCode; 
    YafuHeader ResetRes;
    INT8U         Status;	
	    	
}PACKED AMIYAFUResetDeviceRes_T;

/**
 * @struct AMIGetFruDetailReq_T
 * @brief FRU request info structure
 */
typedef struct
{
    INT8U FruReq;
}PACKED AMIGetFruDetailReq_T;

/**
 * @struct AMIGetFruDetailRes_T
 * @brief FRU response info structure
 */
typedef struct
{
    INT8U CompletionCode;
    INT8U TotalFru;
    INT8U DeviceNo;
    INT8U FRUName[MAX_ID_STR_LEN];
}PACKED AMIGetFruDetailRes_T;


/**
 * @struct AMIGetRootUserAccessRes_T
 * @brief Get Linux Root user access response info structure
 */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   CurrentUserIDState;
} PACKED AMIGetRootUserAccessRes_T;


/**
 * @struct AMISetRootPasswordReq_T
 * @brief Set Root user password request info structure
 */
typedef struct
{
    INT8U Operation;
    INT8U Password[MAX_LINUX_ROOT_USER_PASSWORD_LEN + 1];
}PACKED AMISetRootPasswordReq_T;

/**
 * @struct AMISetRootPasswordRes_T
 * @brief  set Root user password response info structure
 */
typedef struct
{
    INT8U CompletionCode;
}PACKED AMISetRootPasswordRes_T;

/**
 * @struct AMIGetUserShelltypeReq_T
 * @brief  get user shelltype structure
 */
typedef struct
{
    INT8U UserID;
}PACKED AMIGetUserShelltypeReq_T;

/**
 * @struct AMIGetUserShelltypeRes_T
 * @brief  get user shelltype structure
 */
typedef struct
{
    INT8U CompletionCode;
    INT8U Shelltype;	
}PACKED AMIGetUserShelltypeRes_T;


/**
 * @struct AMISetUserShelltypeReq_T
 * @brief  set user shelltype structure
 */
typedef struct
{
    INT8U UserID;
    INT8U Shelltype;
}PACKED AMISetUserShelltypeReq_T;

/**
 * @struct AMISetUserShelltypeRes_T
 * @brief  set user shelltype structure
 */
typedef struct
{
    INT8U CompletionCode;
}PACKED AMISetUserShelltypeRes_T;


/**
 * @struct AMITriggerEventDataUn_T
 * @brief  Trigger Event Data Structure
 */
typedef union
{
    INT32U  Time;
}PACKED AMITriggerEventDataUn_T;

/**
 * @struct AMISetTriggerEventReq_T
 * @brief  Set Trigger Event Request Structure
 */
typedef struct
{
    INT8U TriggerParam;
    INT8U EnableDisableFlag;
    AMITriggerEventDataUn_T TriggerData;
}PACKED AMISetTriggerEventReq_T;

/**
 * @struct AMISetTriggerEventRes_T
 * @brief  Set Trigger Event Response Structure
 */
typedef struct
{
    INT8U CompletionCode;
}PACKED AMISetTriggerEventRes_T;

/**
 * @struct AMIGetTriggerEventReq_T
 * @brief  Get Trigger Event Request Structure
 */
typedef struct
{
    INT8U TriggerParam;
}PACKED AMIGetTriggerEventReq_T;

/**
 * @struct AMIGetTriggerEvent_T
 * @brief  Get Trigger Event Structure
 */
typedef struct
{
    INT8U CompletionCode;
    INT8U EnableDisableFlag;
}PACKED AMIGetTriggerEvent_T;

/**
 * @struct AMIGetTriggerEventRes_T
 * @brief  Get Trigger Event Response Structure
 */
typedef struct
{
    AMIGetTriggerEvent_T AMIGetTriggerEvent;
    AMITriggerEventDataUn_T TriggerData;
}PACKED AMIGetTriggerEventRes_T;


/**
 * @struct AMIGetLoginAuditCfgRes_T
 * @brief  Get login Audit configuration response structure
 */
typedef struct
{
    INT8U CompletionCode;
    INT8U WebLogAuditCfg;
    INT8U IPMILogAuditCfg;	
    INT8U TelnetLogAuditCfg;
    INT8U SSHLogAuditCfg;
}PACKED AMIGetLoginAuditCfgRes_T;


/**
 * @struct AMISetLoginAuditCfgReq_T
 * @brief  Set login Audit configuration request structure
 */
typedef struct
{
    INT8U WebLogAuditCfg;
    INT8U IPMILogAuditCfg;
    INT8U TelnetLogAuditCfg;
    INT8U SSHLogAuditCfg;
}PACKED AMISetLoginAuditCfgReq_T;

/**
 * @struct AMISetLoginAuditCfgRes_T
 * @brief  Set login Audit configuration response structure
 */
typedef struct
{
    INT8U CompletionCode;
}PACKED AMISetLoginAuditCfgRes_T;

/**
 * @struct AMIGetSOLConfRes_T
 * @brief  Get SOL Conf Response Structure
 */
typedef struct
{
    INT8U CompletionCode;
    INT8U SolSessionTimeout;
    char  SolIfcPort[MAX_SOL_PORT_STR_LEN];
}PACKED AMIGetSOLConfRes_T;

/**
 * @struct AMISetPamReq_T
 * @brief  Set PAM Order request structure
 */
typedef struct 
{
    INT8U OrderLen;
    INT8U  Sqnce[MODULES_TO_MODIFIED];
}AMISetPamReq_T;

/**
 * @struct AMISetPamRes_T
 * @brief  Set PAM Order Response Structure
 */
typedef struct 
{
    INT8U CompletionCode;
}AMISetPamRes_T;

/**
 * @struct AMIGetPamRes_T
 * @brief Get PAM Order Response Structure
 */
typedef struct 
{
    INT8U CompletionCode;
    INT8U Seqnce[MODULES_TO_MODIFIED];
}AMIGetPamRes_T;
#pragma pack()

#endif /* IPMI_AMI_H */

