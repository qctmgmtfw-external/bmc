/*****************************************************************
 *****************************************************************
 ***                                                            **
 ***    (C)Copyright 2002-2003, American Megatrends Inc.        **
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
 * $Header:
 *
 * $Revision:
 *
 * $Date:
 *
 ******************************************************************
 ******************************************************************
 *
 * GUID.h
 * 	GUID Generation
 *
 *  Author:
 ******************************************************************/
#ifndef GUID_H
#define GUID_H

#define INT64U unsigned long long

#define GUID_GEN_VERSION  0x01 	// "Time based" GUID Generation.
#define CLOCK_SEQ         0xD7 	// Addition of ASCII Characters "AMI" for clockSeq seed

#define INVALID_GUID 1
#define VALID_GUID   2

#pragma pack (1)

typedef struct
{
   INT8U node[6];
}PACKED Node_T;

typedef struct
{
   Node_T nodeID;
   INT16U clockSeq_Res;
   INT16U timeHigh_Ver;
   INT16U timeMid;
   INT32U timeLow;
} PACKED IPMI_GUID_T;
#pragma pack()

extern void CreateGUID( IPMI_GUID_T* GUID, INT8U* GUIDFileName);
#endif
