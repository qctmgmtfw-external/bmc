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
 *
 * SDRRecord.h
 * SDR record structures.
 *
 *  Author: Govind Kothandapani <govindk@ami.com>
 *
 ******************************************************************/
#ifndef SDR_RECORD_H
#define SDR_RECORD_H

#include "Types.h"
#include "IPMI_SDRRecord.h"
#include "IPMI_IPM.h"

#pragma pack( 1 )

/*** External Definitions */
#define SHARED_RECD_COUNT   0x000F


/**
 * @brief Number of various record types
**/
#define SDR_REPOSITORY_HEADER   16


/**
 * @struct HdrFullSensorRec_T
 * @brief Record Tables With Valid & Length Fields Header
**/
typedef struct
{
    INT8U                   Valid;
    INT8U                   Len;
    FullSensorRec_T         FullSensorRec;

} PACKED  HdrFullSensorRec_T;


/**
 * @struct HdrMgmtCtrlrConfirmRec_T
 * @brief Management Controller Confirmation Record
**/
typedef struct
{
    INT8U                   Valid;
    INT8U                   Len;
    MgmtCtrlrConfirmRec_T   MgmtCtrlrConfirmRec;

} PACKED  HdrMgmtCtrlrConfirmRec_T;


/**
 * @struct HdrMgmtCtrlrDevLocator_T
 * @brief Management Controller Device Locator Record
**/
typedef struct
{
    INT8U                   Valid;
    INT8U                   Len;
    MgmtCtrlrDevLocator_T   MgmtCtrlrDevLocator;

} PACKED  HdrMgmtCtrlrDevLocator_T;


/**
 * @struct HdrBMCMsgChannelInfoRec_T
 * @brief BMC Message Channel Information Record
**/
typedef struct
{
    INT8U                   Valid;
    INT8U                   Len;
    BMCMsgChannelInfoRec_T  BMCMsgChannelInfoRec;

} PACKED  HdrBMCMsgChannelInfoRec_T;


/**
 * @struct HdrGnrcDevLocatorRec_T
 * @brief Generic Device Locator Record
**/
typedef struct
{
    INT8U                   Valid;
    INT8U                   Len;
    GnrcDevLocatorRec_T     GnrcDevLocatorRec;

} PACKED  HdrGnrcDevLocatorRec_T;


/**
 * @struct HdrCompactSensorRec_T
 * @brief Compact Sensor Record
**/
typedef struct
{
    INT8U                   Valid;
    INT8U                   Len;
    CompactSensorRec_T      CompactSensorRec;

} PACKED  HdrCompactSensorRec_T;


/**
 * @struct SDRRepository_T
 * @brief SDR Repository organisation
**/
typedef struct
{
    INT8U   Signature [4];      /* $SDR */

    INT16U  NumRecords;         /* Number of records */
    INT16U  Size;               /* Size of the SDR Records Data */

    INT32U  AddTimeStamp;       /* Most recent addition timestamp */
    INT32U  EraseTimeStamp;     /* Most recent erase timestamp */

    /* NOTE: If anything addede here - update SDR_NUM_FREE_BYTES */

    //INT8U   FreeBytes [SDR_NUM_FREE_BYTES]; /* Free Record bytes */

} PACKED  SDRRepository_T;

#pragma pack( )

#endif  /* SDR_RECORD_H */
