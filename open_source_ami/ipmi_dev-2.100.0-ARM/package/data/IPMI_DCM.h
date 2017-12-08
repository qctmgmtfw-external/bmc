/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2008-2009, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        Suite 200, 5555 Oakbrook Pkwy, Norcross             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 ****************************************************************
 ****************************************************************
 *
 * IPMI_DCM.h
 * DCMDevice Commands Handler structures & macros
 *
 * Author: Rama Bisa <ramab@ami.com>
 *
 *****************************************************************/
#ifndef IPMI_DCMDEV_H
#define IPMI_DCMDEV_H

#include "Types.h"


#pragma pack( 1 )

/* SupportedDCMICap_T */
typedef struct 
{
    INT8U   MandatoryPlatformCaps;
    INT8U   OptionalPlatformCaps;
    INT8U   ManageAccessCaps;

} PACKED SupportedDCMICap_T;

/* MandatoryPlatformAttr_T */
typedef struct 
{
    INT16U  SELAttribs;
    INT8U   IdentifyAttribs;
    INT8U   TempMonitoring;
    INT8U       TempMonitoringSampleFreq;
    
} PACKED MandatoryPlatformAttr_T;

/* OptionalPlatformAttr_T */
typedef struct 
{
    INT8U   PMDeviceSlaveAddr;
    INT8U   PMControllerChannelNumber;

} PACKED OptionalPlatformAttr_T;

/* ManageAccessAttr_T */ 
typedef struct
{
    INT8U   PrimaryLANChannelNum;
    INT8U   SecondaryLANChannelNum;
    INT8U   SerialTMODEChannelNum;

} PACKED ManageAccessAttr_T;

typedef struct
{           
       INT8U   NumberofRollingTimePeriods; 
       INT8U   RollingTimePeriods;                         
}EnhancedSystemAttr_T;

/* DCMICapUn_T */
typedef union 
{
    SupportedDCMICap_T      SupDCMICap;
    MandatoryPlatformAttr_T     ManPlatformAttr;
    OptionalPlatformAttr_T      OptionalPlatformAttr;
    ManageAccessAttr_T      ManageAccessAttr;
    EnhancedSystemAttr_T        EnhancedSystemAttr;
    
} DCMICapUn_T;

/* GetDCMICapReq_T */
typedef struct
{
    INT8U       GroupExtnID;
    INT8U       ParamSelector;

} PACKED  GetDCMICapReq_T;

/* GetDCMICapRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   GroupExtnID;
    INT8U   DCMIMajorVersion;
    INT8U   DCMIMinorVersion;
    INT8U   ParamRevision;
    DCMICapUn_T     CapData;

} PACKED  GetDCMICapRes_T;

/* GetDCMICCRev_T */
typedef struct
{
	INT8U       CompletionCode;
	INT8U       GroupExtnID;
	INT8U       DCMIMajorVersion;
	INT8U       DCMIMinorVersion;
	INT8U       ParamRevision;

} PACKED  GetDCMICCRev_T;

/* GetAssetTagReq_T */
typedef struct 
{
    INT8U   GroupExtnID;
    INT8U   OffsetToRead;
    INT8U   NumBytesToRead;

} PACKED GetAssetTagReq_T;


#define READ_LIMIT_IN_ONE_TRANSACTION		0x10

/* GetAssetTagRes_T */
typedef struct 
{
    INT8U   CompletionCode;
    INT8U   GroupExtnID;
    INT8U   TotalAssetTagLen;
    INT8U   Data [READ_LIMIT_IN_ONE_TRANSACTION];

} PACKED GetAssetTagRes_T;

/* GetDCMISensorInfoReq_T */
typedef struct
{
    INT8U   GroupExtnID;
    INT8U   SensorType;
    INT8U   EntityID;
    INT8U   EntiryInstance;
    INT8U   StartingEntityInstance;

} PACKED GetDCMISensorInfoReq_T;

#define MAX_RECORD_IDS_IN_ONE_TRANSACTION	8

/* GetDCMISensorInfoRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   GroupExtnID;
    INT8U   TotalInstances;
    INT8U   NumRecordIDs;
    INT16U   Data [MAX_RECORD_IDS_IN_ONE_TRANSACTION];

} PACKED GetDCMISensorInfoRes_T;

/* GetPowerReadingReq_T */
typedef struct
{
    INT8U   GroupExtnID;
    INT8U   Mode;
    INT8U   Reserved1;
    INT8U   Reserved2;

} PACKED GetPowerReadingReq_T;

/* GetPowerReadingRes_T */ 
typedef struct
{
    INT8U   CompletionCode;
    INT8U    GroupExtnID;
    INT16U   CurPwrInWatts;
    INT16U   MinPwrInWatts;
    INT16U   MaxPwrInWatts;
    INT16U   AvgPwrInWatts;
    INT32U   TimeStamp;
    INT32U   TimePeriodForStatsInMsec;
    INT8U   PwrReadingState;
    
} PACKED GetPowerReadingRes_T;

/* GetPowerLimitReq_T */
typedef struct
{
    INT8U   GroupExtnID;
    INT8U   Reserved1;
    INT8U   Reserved2;

} PACKED GetPowerLimitReq_T;

/* GetPowerLimitRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   GroupExtnID;
    INT8U   Reserved1;
    INT8U   Reserved2;
    INT8U   ExceptionAction;
    INT16U   PwrLimitInWatts;
    INT32U   CorrectionTimeLimitInMsec;
    INT8U   Reserved3;
    INT8U   Reserved4;
    INT16U   MangSamplingTimeInSecs;

} PACKED GetPowerLimitRes_T;

/* SetPowerLimitReq_T */
typedef struct
{
    INT8U   GroupExtnID;
    INT16U   Reserved1;
    INT8U   Reserved2;
    INT8U   ExceptionAction;
    INT16U   PwrLimitInWatts;
    INT32U   CorrectionTimeLimitInMsec;
    INT16U   Reserved3;
    INT16U   MangSamplingTimeInSecs;

} PACKED SetPowerLimitReq_T;

/* SetPowerLimitRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   GroupExtnID;

} PACKED SetPowerLimitRes_T;

/* ActivatePwrLimitReq_T */
typedef struct
{
    INT8U   GroupExtnID;
    INT8U   PowerLimitActivation;
    INT16U   Reserved;

} PACKED ActivatePwrLimitReq_T;

/* ActivatePwrLimitRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   GroupExtnID;

} PACKED ActivatePwrLimitRes_T;

/* SetAssetTagReq_T */
typedef struct 
{
    INT8U   GroupExtnID;
    INT8U   OffsetToWrite;
    INT8U   DataLen;
    INT8U   Data [READ_LIMIT_IN_ONE_TRANSACTION];

} PACKED SetAssetTagReq_T;

/* SetAssetTagRes_T */
typedef struct 
{
    INT8U   CompletionCode;
    INT8U   GroupExtnID;

} PACKED SetAssetTagRes_T;

/* GetManagementControllerIdStringReq_T */
typedef struct 
{
    INT8U   GroupExtnID;
    INT8U   OffsetToRead;
    INT8U   NumBytesToRead;

} PACKED GetManagementControllerIdStringReq_T;

/* GetManagementControllerIdStringRes_T */
typedef struct 
{
    INT8U   CompletionCode;
    INT8U   GroupExtnID;
    INT8U   TotalLen;
    INT8U   Data [READ_LIMIT_IN_ONE_TRANSACTION];

} PACKED GetManagementControllerIdStringRes_T;

/* SetManagementControllerIdStringReq_T */
typedef struct 
{
    INT8U   GroupExtnID;
    INT8U   OffsetToWrite;
    INT8U   DataLen;
    INT8U   Data [READ_LIMIT_IN_ONE_TRANSACTION];

} PACKED SetManagementControllerIdStringReq_T;

/* SetManagementControllerIdStringRes_T */
typedef struct 
{
    INT8U   CompletionCode;
    INT8U   GroupExtnID;
} PACKED SetManagementControllerIdStringRes_T;

#pragma pack( )

#endif /* IPMI_DCMDEV_H */
