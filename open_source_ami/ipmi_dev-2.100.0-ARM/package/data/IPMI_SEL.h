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
 *****************************************************************
 *
 * ipmi_sel.h
 * Sel device exports
 *
 * Author: Bakka Ravinder Reddy <bakkar@ami.com>
 *
 *****************************************************************/
#ifndef IPMI_SEL_H
#define IPMI_SEL_H

#include "Types.h"


/*** External Definitions ***/
#define SEL_VERSION     0x51
#define UNSPECIFIED_UTC_OFFSET 0x07FF
#define SEL_UTC_MIN_RANGE   -1440
#define SEL_UTC_MAX_RANGE   1440
#define SEL_RECORD_SIZE             16

/**
 * @struct SELRecHdr_T
 * @brief SEL Record Header
**/
#pragma pack(1)
typedef struct
{
    INT16U  ID;
    INT8U   Type;
    INT32U  TimeStamp;

} PACKED  SELRecHdr_T;

/**
 * @struct SELEventRecord_T
 * @brief SEL Event Record
**/
typedef struct
{
    /* SEL ENTRY RECORD HEADER */
    SELRecHdr_T hdr;

    /* RECORD BODY BYTES */
    INT8U   GenID [2];
    INT8U   EvMRev;
    INT8U   SensorType;
    INT8U   SensorNum;
    INT8U   EvtDirType;
    INT8U   EvtData1;
    INT8U   EvtData2;
    INT8U   EvtData3;

} PACKED  SELEventRecord_T;

/**
 * @struct SELOEM1Record_T
 * @brief SEL OEM1 Record
**/
typedef struct
{
    /* SEL ENTRY RECORD HEADER */
    INT16U  ID;
    INT8U   Type;
	
    /* RECORD BODY BYTES */
    INT32U  TimeStamp;
    INT8U   MftrID [3];
    INT8U   OEMData [6];

} PACKED  SELOEM1Record_T;


/**
 * @struct SELOEM2Record_T
 * @brief SEL OEM2 Record
**/
typedef struct
{
    /* SEL ENTRY RECORD HEADER */
	//    SELRecHdr_T hdr;
    INT16U  ID;
    INT8U   Type;

    /* RECORD BODY BYTES */
    INT8U   OEMData [13];

} PACKED  SELOEM2Record_T;




/* SELInfo_T */
typedef struct
{
    INT8U   CompletionCode;     /* Completion Code */
    INT8U   Version;            /* Version of the SEL 0x51 */

    INT16U  RecCt;              /* Record Count */

    INT16U  FreeSpace;          /* Free space LSB first */

    INT32U  AddTimeStamp;       /* Most recent addition timestamp */
    INT32U  EraseTimeStamp;     /* Most recent erase timestamp */

    INT8U   OpSupport;          /* Operation Support */

} PACKED  SELInfo_T;


/* SELAllocInfo_T */
typedef struct
{
    INT8U   CompletionCode;
    INT16U  NumAllocUnits;      /* Number of possible allocation units */
    INT16U  AllocUnitSize;      /* Allocation unit size in bytes */
    INT16U  NumFreeAllocUnits;  /* Number of free allocation units */
    INT16U  LargestFreeBlock;   /* Largest free block in allocation units */
    INT8U   MaxRecSize;         /* Maximimum record size in allocation units */

} PACKED  SELAllocInfo_T;


/* ReserveSELRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT16U  ReservationID;

} PACKED  ReserveSELRes_T;


/* GetSELReq_T */
typedef struct
{
    INT16U  ReservationID;
    INT16U  RecID;
    INT8U   Offset;
    INT8U   Size;

} PACKED  GetSELReq_T;


/* GetSELRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT16U  NextRecID;

} PACKED  GetSELRes_T;


/* AddSELRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT16U  RecID;

} PACKED  AddSELRes_T;


/* PartialAddSELReq_T */
typedef struct
{
    INT8U      LSBReservationID;
	INT8U		MSBReservationID;
	INT8U		LSBRecordID;
	INT8U		MSBRecordID;
    INT8U   Offset;
    INT8U   Progress;
	//Added to support PartialAddselEntry Cmd ../
    INT8U	  RecordData[SEL_RECORD_SIZE];

} PACKED  PartialAddSELReq_T;


/* PartialAddSELRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT16U  RecID;

} PACKED  PartialAddSELRes_T;


/* DeleteSELReq_T */
typedef struct 
{
    INT16U  ReservationID;
    INT16U  RecID;
    
} PACKED  DeleteSELReq_T;


/* DeleteSELRes_T */
typedef struct 
{
    INT8U   CompletionCode;
    INT16U  RecID;
    
} PACKED  DeleteSELRes_T;


/* ClearSELReq_T */
typedef struct
{
    INT16U  ReservationID;
    INT8U   CLR [3];
    INT8U   InitOrStatus;

} PACKED  ClearSELReq_T;


/* ClearSELRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   EraseProgress;
} PACKED  ClearSELRes_T;


/* GetSELTimeRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT32U  Time;

} PACKED  GetSELTimeRes_T;


/* SetSELTimeReq_T */
typedef struct
{
    INT32U  Time;

} PACKED  SetSELTimeReq_T;


/* GetSELAuxiliaryMCALogStatusRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT32U  Time;
    INT32U  NumEntries;

} PACKED  GetSELAuxiliaryMCALogStatusRes_T;


/* GetSELAuxiliaryOEMLogStatusRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT32U  Time;
    INT8U   OEMId[3];
    INT8U   LogStatusBytes[8];

} PACKED  GetSELAuxiliaryOEMLogStatusRes_T;


/* SetSELAuxiliaryMCALogStatusReq_T */
typedef struct
{
    INT8U   LogType;
    INT32U  Time;
    INT32U  NumEntries;

} PACKED  SetSELAuxiliaryMCALogStatusReq_T;


/* SetSELAuxiliaryOEMLogStatusReq_T */
typedef struct
{
    INT8U   LogType;
    INT32U  Time;
    INT8U   OEMId[3];
    INT8U   LogStatusBytes[8];

} PACKED  SetSELAuxiliaryOEMLogStatusReq_T;

/* GetSELTimeUTCoffsetRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT16S  UTCOffset;
} PACKED  GetSELTimeUTCOffsetRes_T;


/* SetSELTimeUTCOffsetReq_T*/
typedef struct
{
    INT16S UTCOffset;
} PACKED  SetSELTimeUTCOffsetReq_T;

/* SetSELTimeUTCOffsetRes_T*/
typedef struct
{
    INT8U   CompletionCode;
} PACKED  SetSELTimeUTCOffsetRes_T;

#pragma pack()
#endif  /* IPMI_SEL_H */
