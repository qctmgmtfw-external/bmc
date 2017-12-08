/****************************************************************
 ****************************************************************
 **                                                            **
 ***    (C)Copyright 2005-2006, American Megatrends Inc.       **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        6145-F, Northbelt Parkway, Norcross,                **
 **                                                            **
 **        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 ****************************************************************
 *****************************************************************
 *
 * ipmi_PEF.h
 * AppDevice Commands Handler
 *
 * Author: Govind Kothandapani <govindk@ami.com>
 *       : Rama Bisa <ramab@ami.com>
 *       : Basavaraj Astekar <basavaraja@ami.com>
 *       : Bakka Ravinder Reddy bakkar@ami.com>
 *
 *****************************************************************/
#ifndef IPMI_PEFDEV_H
#define IPMI_PEFDEV_H

#include "Types.h"

#pragma pack( 1 )

#define ALERT_STR_MAX_BLOCKS                0x3
#define ALERT_STR_BLOCK_SIZE                0x10
#define MAX_ALERT_ENTRIES		127
#define MAX_PEF_ENTRIES			127
#define MAX_ALERT_STRING_LENGTH  48
#define MAX_PEF_CONF_PARAM 					0x0F
#define MIN_PEF_OEM_CONF_PARAM				96
#define MAX_PEF_OEM_CONF_PARAM				127

/**
 * @struct EvtFilterTblEntry_T
 * @brief Event Filter table entry format.
**/
typedef struct
{
    INT8U   FilterConfig;           /**< Filter configuration. */
    INT8U   EvtFilterAction;        /**< Filter Action. */
    INT8U   AlertPolicyNum;         /**< Alert Policy Number. */
    INT8U   EventSeverity;          /**< Event Severity. */
    INT8U   GenIDByte1;             /**< Event Gen ID Byte 1. */
    INT8U   GenIDByte2;             /**< Channel Number/LUN. */
    INT8U   SensorType;             /**< Sensor Type. */
    INT8U   SensorNum;              /**< Sensor Number. */
    INT8U   EventTrigger;           /**< Event Trigger Reading, 0xff - match any. */
    INT16U  EventData1OffsetMask;   /**< Event Data1 offset Mask. */
    INT8U   EventData1ANDMask;      /**< Event Data1 offset AND Mask. */
    INT8U   EventData1Cmp1;         /**< Event Data1 Compare 1 Mask. */
    INT8U   EventData1Cmp2;         /**< Event Data1 Compare 2 Mask. */
    INT8U   EventData2ANDMask;      /**< Event Data1 offset AND Mask. */
    INT8U   EventData2Cmp1;         /**< Event Data1 Compare 1 Mask. */
    INT8U   EventData2Cmp2;         /**< Event Data1 Compare 2 Mask. */
    INT8U   EventData3ANDMask;      /**< Event Data1 offset AND Mask. */
    INT8U   EventData3Cmp1;         /**< Event Data1 Compare 1 Mask. */
    INT8U   EventData3Cmp2;         /**< Event Data1 Compare 2 Mask. */

} PACKED  EvtFilterTblEntry_T;


/**
 * @struct AlertPolicyTblEntry_T
 * @brief Alert Policy Entry structure
**/
typedef struct
{
    INT8U   AlertNum;           /**< Alert policy no. */
    INT8U   ChannelDestSel;     /**< Destination selector. */
    INT8U   AlertStingkey;      /**< Alert String key. */

} PACKED  AlertPolicyTblEntry_T;


/**
 * @struct AlertStringTbl_T
 * Alert String Table
**/
typedef struct
{
    INT8U   EventFilterSel;
    INT8U   AlertStringSet;
    INT8U   AlertString [ALERT_STR_MAX_BLOCKS] [ALERT_STR_BLOCK_SIZE];

} PACKED  AlertStringTbl_T;


/* GetPEFCapRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   PEFVersion;
    INT8U   ActionSupport;
    INT8U   TotalEntries;

} PACKED  GetPEFCapRes_T;

/* ArmPEFTmrRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   PresentTmrVal;

} PACKED  ArmPEFTmrRes_T;

/* GetPEFConfigReq_T */
typedef struct
{
    INT8U   ParamSel;
    INT8U   SetSel;
    INT8U   BlockSel;

} PACKED  GetPEFConfigReq_T;

/* GetPEFConfigRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   PEFVersion;
} PACKED  GetPEFConfigRes_T;

/* SetLastEvtIDReq_T */
typedef struct 
{
    INT8U   SetRecIDType;
    INT16U  RecordID;

} PACKED  SetLastEvtIDReq_T;

/* GetLastEvtIDRes_T */
typedef struct 
{
    INT8U   CompletionCode;     
    INT32U  RecentTimestamp;    
    INT16U  LastSELRecord;      
    INT16U  LastSWProcessedID;  
    INT16U  LastBMCProcessedID; 

} PACKED  GetLastEvtIDRes_T;

/* AlertImmReq_T */
typedef struct 
{
    INT8U   ChannelNo;      
    INT8U   DestSel;        
    INT8U   AlertStrSel;    
    INT8U   GenID ;
    INT8U   EvMRev;
    INT8U   SensorType;
    INT8U   SensorNum;
    INT8U   EvtDirType;
    INT8U   EvtData1;
    INT8U   EvtData2;
    INT8U   EvtData3;

} PACKED  AlertImmReq_T;

typedef struct
{
    INT8U   CompletionCode;
    INT8U   Status;

} PACKED  AlertImmRes_T;

/* PETAckReq_T */
typedef struct 
{
    INT16U  SequenceNum;    
    INT32U  Timestamp;      
    INT8U   EventSrcType;
    INT8U   SensorDevice;   
    INT8U   SensorNum;      
    INT8U   EventData1;     
    INT8U   EventData2;     
    INT8U   EventData3; 
} PACKED  PETAckReq_T;

#pragma pack( )

#endif /* IPMI_PEFDEV_H */
