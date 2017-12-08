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
 * ipmi_chassis.h
 * IPMI chassis Request and Response structures
 *
 *  Author: Rama Bisa <ramab@ami.com>
 *
 ******************************************************************/
#ifndef IPMI_CHASSIS_DEVICE_H
#define IPMI_CHASSIS_DEVICE_H

#include "Types.h"

#pragma pack( 1 )

/*** Definitions and Macros ***/
#define MAX_BOOT_INIT_MAILBOX_BLOCKS      5
#define MAX_BOOT_INIT_MAILBOX_BLOCK_SIZE  16

#define CHASSIS_IDENTITY_STATE_INFO_SUPPORTED	0x40
#define CHASSIS_IDENTITY_INDEFINITE_ON 		0x20
#define CHASSIS_IDENTITY_TIMED_ON  		0x10
#define CHASSIS_IDENTITY_OFF 			0x00


/**
 * @struct ChassisCapabilities_T
 * @brief Chassis capabilities
 **/
typedef struct
{
    INT8U    CapabilitiesFlags;
    INT8U    FRUInfoAddr;
    INT8U    SDRDeviceAddr;
    INT8U    SELDeviceAddr;
    INT8U    SMDeviceAddr;
    INT8U    ChassBridgeFn;

} PACKED  ChassisCapabilities_T;

/**
 * @struct ChassisPowerState_T
 * @brief Chassis Power state
 **/
typedef struct
{
    INT8U                   PowerState;
    INT8U                   LastPowerEvent;
    INT8U                   MiscChassisState;
    INT8U                   FPBtnEnables;
} PACKED  ChassisPowerState_T;

/**
 * @struct AMI_BootOpt_T
 * @brief AMI specific Boot options
 **/
typedef struct
{
    INT8U       Data1;
    INT8U       Data2;
    
} PACKED  AMI_BootOpt_T;

/* GetChassisCapabilitiesRes_T */
typedef struct
{
    INT8U                   CompletionCode;
    ChassisCapabilities_T   ChassisCapabilities;
    
} PACKED  GetChassisCapabilitiesRes_T;

/* GetChassisStatusRes_T */
typedef struct
{
    INT8U                   CompletionCode;
    ChassisPowerState_T     ChassisPowerState;
    
} PACKED  GetChassisStatusRes_T;

/* ChassisControlReq_T */
typedef struct
{
    INT8U  ChassisControl;
    
} PACKED  ChassisControlReq_T;

/* ChassisControlRes_T */
typedef struct
{
    INT8U   CompletionCode;
    
} PACKED  ChassisControlRes_T;

/* ChassisIdentifyReq_T */
typedef struct
{
    INT8U IdentifyInterval;
    INT8U ForceIdentify;

} PACKED  ChassisIdentifyReq_T;

/* ChassisIdentifyRes_T */
typedef struct
{
    INT8U   CompletionCode;
    
} PACKED  ChassisIdentifyRes_T;

/* SetChassisCapabilitiesReq_T */
typedef struct
{
    ChassisCapabilities_T   ChassisCaps;
    
} PACKED  SetChassisCapabilitiesReq_T;

/* SetChassisCapabilitiesRes_T */
typedef struct
{
    INT8U   CompletionCode;
    
} PACKED  SetChassisCapabilitiesRes_T;

/* SetPowerRestorePolicyReq_T */
typedef struct
{
    INT8U   PowerRestorePolicy;
    
} PACKED  SetPowerRestorePolicyReq_T;

/* SetPowerRestorePolicyRes_T */
typedef struct
{
    INT8U  CompletionCode;
    INT8U  PowerRestorePolicy;
    
} PACKED  SetPowerRestorePolicyRes_T;

/* GetSystemRestartCauseRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   SysRestartCause;
    INT8U   ChannelID;
    
} PACKED  GetSystemRestartCauseRes_T;

/* GetPOHCounterRes_T */
typedef struct
{   
    INT8U           CompletionCode;
    INT8U           MinutesPerCount;
    INT32U          POHCounterReading;
    
} PACKED  GetPOHCounterRes_T;

/* BootInfoAck_T */
typedef struct
{
    INT8U WriteMask;
    INT8U BootInitiatorAckData;
    
} PACKED  BootInfoAck_T;

/* BootFlags_T */
typedef struct
{
    INT8U    BootFlagsValid;
    INT8U    Data2;
    INT8U    Data3;
    INT8U    Data4;
    INT8U    Data5;
    
} PACKED  BootFlags_T;

/* BootInitiatorInfo_T */
typedef struct
{
    INT8U   BootSource;
    INT8U   SessionID [4];
    INT8U   BootInfoTimestamp [4];
    
} PACKED  BootInitiatorInfo_T;

/* BootInitiatorMailbox_T */
typedef struct 
{
    INT8U BlockData [MAX_BOOT_INIT_MAILBOX_BLOCK_SIZE];
    
} PACKED  BootInitiatorMailbox_T;

/* BootInitiatorMboxReq_T */
typedef struct
{
    INT8U   BlockSel;
    BootInitiatorMailbox_T  BootMBox;
    
} PACKED  BootInitiatorMboxReq_T;

#ifdef CONFIG_SPX_FEATURE_SSICOMPUTEBLADE_SUPPORT
#define BOT_DEV_NUM         CONFIG_SPX_FEATURE_SSICB_DEFAULT_BOT_DEV_NUM
#define BOT_DEV_INFO_SIZE   CONFIG_SPX_FEATURE_SSICB_DEFAULT_BOT_DEV_INFO_SIZE
#define ANCH_STR_LEN        4
#define IP_ADDR_LEN         4
#define CFG_SCR_NAME_LEN    48
#define CHAS_GUID_LEN       16
#define SCT_RSV_LEN         28

/* OemParamBlkSizeTbl_T */
typedef struct
{
    INT8U   AnchorStr[ANCH_STR_LEN]; /* Header */
    INT8U   Checksum;
    INT8U   MajorRev;
    INT8U   MinorRev;
    INT16U  Len;
    INT8U   Reserved1;
    INT8U   SysIfcBlkSize;           /* Data */
    INT8U   Reserved2;
    INT8U   IPMBIfcBlkSize;
    INT8U   Reserved3;
    INT8U   LANIfcBlkSize;
    INT8U   Reserved4;
} PACKED OemParamBlkSizeTbl_T;

/* BootOrderTbl_T */
typedef struct
{
    INT8U   AnchorStr[ANCH_STR_LEN]; /* Header */
    INT8U   BOTChecksum;
    INT8U   BOTMajorRev;
    INT8U   BOTMinorRev;
    INT16U  Len;
    INT8U   Reserved;
    INT8U   UpdFlag;                 /* Data */
    INT8U   BootOrderInfo[BOT_DEV_INFO_SIZE];
    INT8U   OrderType;
    INT8U   OrderLen;
    INT8U   DevOrderList[BOT_DEV_INFO_SIZE];
    INT8U   DevNamePath[BOT_DEV_INFO_SIZE];
} PACKED BootOrderTbl_T;

/* BootOrderTblReq_T */
typedef struct
{
    INT8U          BlockSel;
    BootOrderTbl_T BootOrderTbl;
} PACKED BootOrderTblReq_T;

/* SlotConfigTbl_T */
typedef struct
{
    INT8U   AnchorStr[ANCH_STR_LEN]; /* Header */
    INT8U   SCTChecksum;
    INT8U   SCTMajorRev;
    INT8U   SCTMinorRev;
    INT16U  Len;
    INT8U   Reserved1;
    INT8U   NICIP[IP_ADDR_LEN];      /* Data */
    INT8U   NICNetmask[IP_ADDR_LEN];
    INT8U   NICGateway[IP_ADDR_LEN];
    INT16U  MgmtVLAN;
    INT8U   MgmtIP[IP_ADDR_LEN];
    INT8U   ConfigScriptName[CFG_SCR_NAME_LEN];
    INT8U   ChassisGUID[CHAS_GUID_LEN];
    INT16U  ConfigBootType;
    INT16U  TFTPPort;
    INT16U  SFTPPort;
    INT16U  SCPort;
    INT8U   Reserved2[SCT_RSV_LEN];
} PACKED SlotConfigTbl_T;
#endif

/* BootOptParams_T */
typedef union 
{
    INT8U                   SetInProgress;
    INT8U                   ServicePartitionSelector; 
    INT8U                   ServicePartitionScan;
    INT8U                   BootFlagValidBitClearing;
    BootInfoAck_T           BootInfoAck;
    BootFlags_T             BootFlags;
    BootInitiatorInfo_T     BootInitiatorInfo;
    BootInitiatorMboxReq_T  BootMailBox;
    AMI_BootOpt_T           Oem;
#ifdef CONFIG_SPX_FEATURE_SSICOMPUTEBLADE_SUPPORT    /* SSI Boot Option Parameters */
    OemParamBlkSizeTbl_T    OemParamBlkSizeTbl;
    BootOrderTblReq_T       BootOrderTbl;
    INT8U                   BootDevSelector;
    SlotConfigTbl_T         SlotConfigTbl;
#endif
} BootOptParams_T;

/* SetBootOptionsReq_T */
typedef struct
{
    INT8U               ParamValidCumParam;
    BootOptParams_T     BootParam;
    
} PACKED  SetBootOptionsReq_T;

/* SetBootOptionsRes_T */
typedef struct
{
    INT8U               CompletionCode;
    
} PACKED  SetBootOptionsRes_T;

/* BootOptions_T */
typedef struct
{
    INT8U                   ParameterValid;
	INT8U          			u8SetInProgress;    			/**< Set in progess variable */
    INT8U                   ServicePartitionSelector; 
    INT8U                   ServicePartitionScan;
    INT8U                   BootFlagValidBitClearing;
    BootInfoAck_T           BootInfoAck;
    BootFlags_T             BootFlags;
    BootInitiatorInfo_T     BootInitiatorInfo;
    BootInitiatorMailbox_T  BootMailBox[MAX_BOOT_INIT_MAILBOX_BLOCKS];
    AMI_BootOpt_T           Oem;
#ifdef CONFIG_SPX_FEATURE_SSICOMPUTEBLADE_SUPPORT    /* SSI Boot Option Parameters */
    OemParamBlkSizeTbl_T    OemParamBlkSizeTbl;
    BootOrderTbl_T          BootOrderTbl[BOT_DEV_NUM];
    INT8U                   BootDevSelector;
    SlotConfigTbl_T         SlotConfigTbl;
#endif    
} PACKED  BootOptions_T;

/* GetBootOptionsReq_T */
typedef struct
{
    INT8U       ParamSel;
    INT8U       SetSel;
    INT8U       BlockSel;
    
} PACKED  GetBootOptionsReq_T;

/* GetBootOptParams_T */
typedef union
{
    INT8U                   SetInProgress;  
    INT8U                   ServicePartitionSelector; 
    INT8U                   ServicePartitionScan;
    INT8U                   BootFlagValidBitClearing;
    BootInfoAck_T           BootInfoAck;
    BootFlags_T             BootFlags;
    BootInitiatorInfo_T     BootInitiatorInfo;
    BootInitiatorMboxReq_T  BootMailBox;
    AMI_BootOpt_T           Oem;
#ifdef CONFIG_SPX_FEATURE_SSICOMPUTEBLADE_SUPPORT    /* SSI Boot Option Parameters */
    OemParamBlkSizeTbl_T    OemParamBlkSizeTbl;
    BootOrderTblReq_T       BootOrderTbl;
    INT8U                   BootDevSelector;
    SlotConfigTbl_T         SlotConfigTbl;
#endif
} GetBootOptParams_T;

/* GetBootOptionsRes_T */
typedef struct 
{
    INT8U               CompletionCode;
    INT8U               ParamVersion;
    INT8U               ParameterValid;
    GetBootOptParams_T  BootParams;
    
} PACKED  GetBootOptionsRes_T;

/* SetFPBtnEnablesReq_T */
typedef struct
{
    INT8U   ButtonEnables;

} PACKED SetFPBtnEnablesReq_T;

/* SetFPBtnEnablesRes_T */
typedef struct
{
    INT8U   CompletionCode;

} PACKED SetFPBtnEnablesRes_T;

/* SetPowerCycleInteval Req */
typedef struct
{
    INT8U   PowerCycleInterval;
    
} PACKED  SetPowerCycleIntervalReq_T;

/* SetPowerRestorePolicyRes_T */
typedef struct
{
    INT8U  CompletionCode;
    
} PACKED  SetPowerCycleIntervalRes_T;

#pragma pack( )

#endif /* IPMI_CHASSIS_DEVICE*/
