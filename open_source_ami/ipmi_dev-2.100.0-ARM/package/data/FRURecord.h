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
 ******************************************************************
 *
 * FRURecord.h
 * FRU record structures.
 *
 *  Author: Rama Bisa <ramab@ami.com>
 *
 ******************************************************************/
#ifndef FRU_RECORD_H
#define FRU_RECORD_H

#include "Types.h"

#pragma pack( 1 )

/**
 * @struct FRUCommonHdr_T
 * @brief FRU Common Header
**/
typedef struct
{
    INT8U CommonHeaderFormatVersion;
    INT8U InternalUseAreaStartOffset;
    INT8U ChassisInfoAreaStartOffset;
    INT8U BoardAreaStartOffset;
    INT8U ProductInfoAreaStartOffset;
    INT8U MultiRecordAreaStartOffset;
    INT8U PAD;
    INT8U CommonHeaderChecksum;

} PACKED  FRUCommonHdr_T;


/*** External Definitions ***/
#define MAX_FRU_DATA_AREA_SIZE  (MAX_FRU_AREA_INFO_SIZE - sizeof(FRUCommonHdr_T) )
#define FRU_DATA_START(pfru)    ((_FAR_ INT8U*)(&(((_FAR_ FRURepository_T*) pfru)->FruInfo)))


/**
 * @struct FRUInfoArea_T
 * @brief FRU Info Area
**/
typedef struct
{
    FRUCommonHdr_T  CommonHeader;
    INT8U           FRUData [MAX_FRU_DATA_AREA_SIZE];

} PACKED  FRUInfoArea_T;


/**
 * @struct FRUInternalUseAreaHdr
 * @brief FRU Internal Use Area Header
**/
typedef struct
{
    INT8U   InternalUseAreaFormatVersion;
    INT8U   InternalUseAreaInfoLength;

} PACKED  FRUInternalUseAreaHdr;


/** 
 * @struct FRURepository_T
 * @brief FRU Repository organisation 
**/
typedef struct
{
    INT8U           Signature [4];  /* $FRU */
    FRUInfoArea_T   FruInfo[MAX_FRU_DEVICES];

} PACKED  FRURepository_T;

#pragma pack( )

#endif /* FRU_RECORD_H */
