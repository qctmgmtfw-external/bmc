/*****************************************************************
 *****************************************************************
 ***                                                            **
 ***    (C)Copyright 2005-2006, American Megatrends Inc.        **
 ***                                                            **
 ***            All Rights Reserved.                            **
 ***                                                            **
 ***        6145-F, Northbelt Parkway, Norcross,                **
 ***                                                            **
 ***        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 ***                                                            **
 *****************************************************************
 *****************************************************************
 *****************************************************************
 ******************************************************************
 *
 * SDRRecord.h
 * SDR record structures.
 *
 *  Author: Govind Kothandapani <govindk@ami.com>
 *
 ******************************************************************/
#ifndef IPMI_SDR_RECORD_H
#define IPMI_SDR_RECORD_H

#include "Types.h"

#pragma pack( 1 )

/**
 * @brief SDR Type
**/
#define FULL_SDR_REC                    0x01
#define COMPACT_SDR_REC                 0x02
#define EVENT_ONLY_SDR_REC              0x03
#define ENTITY_ASSOCIATION_SDR_REC      0x08
#define DEV_REL_ENTITY_ASSOCIATION_SDR_REC 0x09
#define GENERIC_DEVICE_LOCATOR_SDR_REC  0x10
#define FRU_DEVICE_LOCATOR_SDR_REC      0x11
#define MGMT_CTRL_DEV_LOCATOR_SDR_REC   0x12
#define MGMT_CTRL_CONFIRMATION_SDR_REC  0x13
#define BMC_MSG_CHANNEL_INFO_REC        0x14  
#define OEM_SDRFRU_REC                  0xD0
#define OEM_SDRNM_REC                   0xC0


/**
 * @brief Maximum Length of the ID String
**/
#define MAX_ID_STR_LEN  16
#define MAX_FRU_SDR_STR_SIZE 40

/**
 * @struct SDRRecHdr_T
 * @brief SDR Record Header
**/
typedef struct
{
    INT16U  ID;
    INT8U   Version;
    INT8U   Type;
    INT8U   Len;

} PACKED  SDRRecHdr_T;


/**
 * @struct FullSensorRec_T
 * @brief Full Sensor Record
**/
typedef struct
{
    /* SENSOR RECORD HEADER */
    SDRRecHdr_T     hdr;

    /* RECORD KEY BYTES */
    INT8U           OwnerID;
    INT8U           OwnerLUN;
    INT8U           SensorNum;

    /* RECORD BODY BYTES */
    INT8U           EntityID;
    INT8U           EntityIns;
    INT8U           SensorInit;
    INT8U           SensorCaps;
    INT8U           SensorType;
    INT8U           EventTypeCode;
    INT16U          AssertionEventMask;
    INT16U          DeAssertionEventMask;
    INT16U          DiscreteReadingMask;
    INT8U           Units1;
    INT8U           Units2;
    INT8U           Units3;
    INT8U           Linearization;
    INT8U           M;
    INT8U           M_Tolerance;
    INT8U           B;
    INT8U           B_Accuracy;
    INT8U           Accuracy;
    INT8U           R_B_Exp;
    INT8U           Flags;
    INT8U           NominalReading;
    INT8U           NormalMax;
    INT8U           NormalMin;
    INT8U           MaxReading;
    INT8U           MinReading;
    INT8U           UpperNonRecoverable;
    INT8U           UpperCritical;
    INT8U           UpperNonCritical;
    INT8U           LowerNonRecoverable;
    INT8U           LowerCritical;
    INT8U           LowerNonCritical;
    INT8U           PositiveHysterisis;
    INT8U           NegativeHysterisis;
    INT8U           Reserved1;
    INT8U           Reserved2;
    INT8U           OEMField;
    INT8U           IDStrTypeLen;
    INT8S           IDStr [MAX_ID_STR_LEN];

} PACKED  FullSensorRec_T;

/**
 * @struct CompactSensorRec_T
 * @brief Compact Sensor Record
**/
typedef struct
{
    /* SENSOR RECORD HEADER */
    SDRRecHdr_T     hdr;

    /* RECORD KEY BYTES */
    INT8U           OwnerID;
    INT8U           OwnerLUN;
    INT8U           SensorNum;

    /* RECORD BODY BYTES */
    INT8U           EntityID;
    INT8U           EntityIns;
    INT8U           SensorInit;
    INT8U           SensorCaps;
    INT8U           SensorType;
    INT8U           EventTypeCode;
    INT16U          AssertionEventMask;
    INT16U          DeAssertionEventMask;
    INT16U          DiscreteReadingMask;
    INT8U           Units1;
    INT8U           Units2;
    INT8U           Units3;
    INT16U          RecordSharing;
    INT8U           PositiveHysteris;
    INT8U           NegativeHysterisis;
    INT8U           Reserved1;
    INT8U           Reserved2;
    INT8U           Reserved3;
    INT8U           OEMField;
    INT8U           IDStrTypeLen;
    INT8S           IDStr [MAX_ID_STR_LEN];

} PACKED  CompactSensorRec_T;

/**
 * @struct
 * @brief Common structure for both Full and Compact sensor Record.
 */
typedef struct
{
    /* SENSOR RECORD HEADER */
    SDRRecHdr_T     hdr;

    /* RECORD KEY BYTES */
    INT8U           OwnerID;
    INT8U           OwnerLUN;
    INT8U           SensorNum;

    /* RECORD BODY BYTES */
    INT8U           EntityID;
    INT8U           EntityIns;
    INT8U           SensorInit;
    INT8U           SensorCaps;
    INT8U           SensorType;
    INT8U           EventTypeCode;
    INT16U          AssertionEventMask;
    INT16U          DeAssertionEventMask;
    INT16U          DiscreteReadingMask;
    INT8U           Units1;
    INT8U           Units2;
    INT8U           Units3;
} PACKED CommonSensorRec_T;


/**
 * @struct MgmtCtrlrConfirmRec_T
 * @brief Management Controller Confirmation Record
**/
typedef struct
{
    /* SENSOR RECORD HEADER */
    SDRRecHdr_T     hdr;

    /* RECORD KEY BYTES */
    INT8U           DevSlaveAddr;
    INT8U           DevID;
    INT8U           ChannelNum;

    /* RECORD BODY BYTES */
    INT8U           FirmwareRev1;
    INT8U           FirmwareRev2;
    INT8U           IPMIVer;
    INT8U           MftrID [3];
    INT8U           ProdID [2];
    INT8U           DevGUID [MAX_ID_STR_LEN];

} PACKED  MgmtCtrlrConfirmRec_T;


/**
 * @struct MgmtCtrlrDevLocator_T
 * @brief Management Controller Device Locator Record
**/
typedef struct
{
    /* SENSOR RECORD HEADER */
    SDRRecHdr_T     hdr;

    /* RECORD KEY BYTES */
    INT8U           DevSlaveAddr;
    INT8U           ChannelNum;

    /* RECORD BODY BYTES */
    INT8U           PowerStateNotification;
    INT8U           DeviceCaps;
    INT8U           reserved [3];
    INT8U           EntityID;
    INT8U           EntityIns;
    INT8U           OEMField;
    INT8U           IDStrLen;
    INT8S           DevIdStr [MAX_ID_STR_LEN];

} PACKED  MgmtCtrlrDevLocator_T;


/**
 * @struct GnrcDevLocatorRec_T
 * @brief Generic Device Locator Record
**/
typedef struct
{
    /* SENSOR RECORD HEADER */
    SDRRecHdr_T     hdr;

    /* RECORD KEY BYTES */
    INT8U           DevAccessAddr;
    INT8U           DevSlaveAddr;
    INT8U           LUNBusID;

    /* RECORD BODY BYTES */
    INT8U           AddrSpan;
    INT8U           Reserved;
    INT8U           DevType;
    INT8U           DevTypeModifier;
    INT8U           EntityID;
    INT8U           EntityIns;
    INT8U           OEMField;
    INT8U           IDStrLen;
    INT8S           DevIdStr [MAX_ID_STR_LEN];

} PACKED  GnrcDevLocatorRec_T;

/**
 * @struct FRUcDevLocatorRec_T
 * @brief FRU Device Locator Record
**/
typedef struct
{
    /* SENSOR RECORD HEADER */
    SDRRecHdr_T     hdr;

    /* RECORD KEY BYTES */
    INT8U           DevAccessAddr;
    INT8U           FRUIDSlaveAddr;
    INT8U           AccessLUNBusID;
    INT8U           ChannelNumber;

    /* RECORD BODY BYTES */
    INT8U           Reserved;
    INT8U           DevType;
    INT8U           DevTypeModifier;
    INT8U           EntityID;
    INT8U           EntityIns;
    INT8U           OEMField;
    INT8U           IDStrLen;
    INT8S           DevIdStr [MAX_ID_STR_LEN];

} PACKED  FRUDevLocatorRec_T;

/**
 * @struct OEM_FRURec_T
 * @brief OEM FRU Record Info
**/
typedef struct
{
    /*SENSOR RECORD HEADER*/
    SDRRecHdr_T   hdr;

    /*RECORD BODY BYTES*/
    INT8U        Mfg_ID1;
    INT8U        Mfg_ID2;
    INT8U        Mfg_ID3;
    INT8U        OEM_Fru;
    INT8U        DeviceID;
    INT16U       Size;
    INT8U        AccessType;
    INT8U        EntityID;
    INT8U        EntityIns;
    INT8U        IDStrLen;
    INT8S        FilePath[MAX_FRU_SDR_STR_SIZE];

} PACKED OEM_FRURec_T;

/**
 * @struct OEM_NMRec_T
 * @brief OEM NM Record Info
**/
typedef struct
{

  /*SENSOR RECORD HEADER */
  SDRRecHdr_T hdr;

  /*RECORD BODY BYTES*/
  INT8U  Mfg_ID1;
  INT8U  Mfg_ID2;
  INT8U  Mfg_ID3;
  INT8U  RecordSubType;
  INT8U  VersionNo;
  INT8U  NMDevSlaveAddress;
  INT8U  ChannelNumber;
  INT8U  NMHealthEvtSensor;
  INT8U  NMExceptionEvtSensor;
  INT8U  NMOpCapSensor;
  INT8U  NMAlertThresExSensor;

}PACKED OEM_NMRec_T;


/**
 * @struct BMCMsgChannelInfoRec_T
 * @brief BMC Mesage Channel info Record
**/
typedef struct
{
    /* SENSOR RECORD HEADER */
    SDRRecHdr_T     hdr;

    /* RECORD BODY BYTES */
    INT8U           MsgChannel0Info;
    INT8U           MsgChannel1Info;
    INT8U           MsgChannel2Info;
    INT8U           MsgChannel3Info;
    INT8U           MsgChannel4Info;
    INT8U           MsgChannel5Info;
    INT8U           MsgChannel6Info;
    INT8U           MsgChannel7Info;
    INT8U           MessagingINTType;
    INT8U           EvtMsgBufINTType;
    INT8U           Reserved;

} PACKED  BMCMsgChannelInfoRec_T;


/**
 * @struct SDRRec_T
 * @brief Complete unionized record structure with unions for
 *        easy reference to different types
**/
typedef struct
{
    SDRRecHdr_T     hdr;
    union
    {
        FullSensorRec_T full_sensor_rec;
        CompactSensorRec_T compact_sensor_rec;
        MgmtCtrlrConfirmRec_T mc_confim_rec;
        MgmtCtrlrDevLocator_T mc_dev_locator_rec;
        GnrcDevLocatorRec_T  gen_dev_locator_rec;
        FRUDevLocatorRec_T  fru_dev_locator_rec;
        BMCMsgChannelInfoRec_T bmc_msg_chnl_info_rec;

    }
    type;
} PACKED SDRRec_T;


#pragma pack( )

#endif  /* IPMI_SDR_RECORD_H */
