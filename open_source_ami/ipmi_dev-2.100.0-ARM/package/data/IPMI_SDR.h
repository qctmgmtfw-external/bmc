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
 * ipmi_sdr.c
 * IPMI SDR Record
 *
 *  Author: Govind Kothandapani <govindk@ami.com>
 *
 ******************************************************************/
#ifndef IPMI_SDR_H
#define IPMI_SDR_H

#include "Types.h"


/*** External Definitions ***/
#define INVALID_RECORD_ID                   0x00
#define OVERFLOW_FLAG                       0x80

#pragma pack( 1 )

/* SDRRepositoryInfo_T */
typedef struct
{
    INT8U   CompletionCode;     /* Completion Code */
    INT8U   Version;            /* Version of the SDR 0x51 */
    INT16U  RecCt;              /* Record Count */
    INT16U  FreeSpace;          /* Free space LSB first */
    INT32U  AddTimeStamp;       /* Most recent addition timestamp */
    INT32U  EraseTimeStamp;     /* Most recent erase timestamp */
    INT8U   OpSupport;          /* Operation Support */

} PACKED  SDRRepositoryInfo_T;


/* SDRRepositoryAllocInfo_T */
typedef struct
{
    INT8U   CompletionCode;
    INT16U  NumAllocUnits;      /* Number of possible allocation units */
    INT16U  AllocUnitSize;      /* Allocation unit size in bytes */
    INT16U  NumFreeAllocUnits;  /* Number of free allocation units */
    INT16U  LargestFreeBlock;   /* Largest free block in allocation units */
    INT8U   MaxRecSize;         /* Maximimum record size in allocation units */

} PACKED  SDRRepositoryAllocInfo_T;


/* ReserveSDRRepositoryRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT16U  ReservationID;

} PACKED  ReserveSDRRepositoryRes_T;


/* GetSDRReq_T */
typedef struct
{
    INT16U  ReservationID;
    INT16U  RecID;
    INT8U   Offset;
    INT8U   Size;

} PACKED  GetSDRReq_T;


/* GetSDRRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT16U  NextRecID;

} PACKED  GetSDRRes_T;


/* AddSDRRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT16U  RecID;

} PACKED  AddSDRRes_T;


/* PartialAddSDRReq_T */
typedef struct
{
    INT16U  ReservationID;
    INT16U  RecID;
    INT8U   Offset;
    INT8U   Progress;

} PACKED  PartialAddSDRReq_T;

/* PartialAddSDRRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT16U  RecID;

} PACKED  PartialAddSDRRes_T;

/* DeleteSDRReq_T */
typedef struct 
{
    INT16U  ReservationID;
    INT16U  RecID;
    
} PACKED  DeleteSDRReq_T;


/* DeleteSDRRes_T */
typedef struct 
{
    INT8U   CompletionCode;
    INT16U  RecID;
    
} PACKED  DeleteSDRRes_T;


/* ClearSDRReq_T */
typedef struct
{
    INT16U  ReservationID;
    INT8U   CLR [3];
    INT8U   InitOrStatus;

} PACKED  ClearSDRReq_T;


/* ClearSDRRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   EraseProgress;

} PACKED  ClearSDRRes_T;


/* GetSDRRepositoryTimeRes_T  */
typedef struct
{
    INT8U   CompletionCode;
    INT32U  Time;

} PACKED  GetSDRRepositoryTimeRes_T;


/* SetSDRRepositoryTimeReq_T */
typedef struct
{
    INT32U  Time;

} PACKED  SetSDRRepositoryTimeReq_T;


/* EnterSDRUpdateModeRes_T */
typedef struct
{
    INT8U   CompletionCode;

} PACKED  EnterSDRUpdateModeRes_T;


/* ExitSDRUpdateModeRes_T */
typedef struct
{
    INT8U   CompletionCode;

} PACKED  ExitSDRUpdateModeRes_T;


/* RunInitAgentReq_T */
typedef struct
{
    INT8U   RunStatus;

} PACKED  RunInitAgentReq_T;


/* RunInitAgentRes_T  */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   Status;

} PACKED  RunInitAgentRes_T;

#pragma pack()
#endif  /* IPMI_SDR_H */
