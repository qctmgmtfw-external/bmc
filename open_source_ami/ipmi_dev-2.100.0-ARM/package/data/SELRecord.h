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
 * SELRecord.h
 * SEL record structures.
 *
 *  Author: Rama Rao Bisa<ramab@ami.com>
 *
 ******************************************************************/
#ifndef SEL_RECORD_H
#define SEL_RECORD_H

#include "IPMI_SEL.h"

/*** External Definitions ***/
#define VALID_RECORD        0x5A



#pragma pack( 1 )

/**
 * @struct SELRec_T
 * @brief SEL Record
**/
typedef struct
{
    INT8U               Valid;
    INT8U               Len;
    SELEventRecord_T    EvtRecord;

} PACKED  SELRec_T;


/**
 * @struct SELRepository_T
 * @brief SEL Repository structure.
**/
typedef struct
{
    INT8U       Signature [4];      /* $SDR */
    INT16U      NumRecords;
    INT16U      Padding;
    INT32U      AddTimeStamp;
    INT32U      EraseTimeStamp;
    SELRec_T    *SELRecord;

} PACKED  SELRepository_T;

#pragma pack( )

#endif  /* SEL_RECORD_H */
