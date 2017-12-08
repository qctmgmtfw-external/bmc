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
 ******************************************************************
 *
 * ipmi_fru.h
 * IPMI fru structures
 *
 *  Author: Rama Bisa <govindk@ami.com>
 *
 ******************************************************************/
#ifndef IPMI_FRU_H
#define IPMI_FRU_H

#include "Types.h"

#define FRU_COMMON_HEADER           1
#define FRU_INTERNAL_USE_AREA       2
#define FRU_CHASSIS_INFO_AREA       3
#define FRU_BOARD_INFO_AREA         4
#define FRU_PRODUCT_INFO_AREA       5
#define FRU_MULTI_RECORD_AREA       6
#define FRU_MULTI_RECORD_HEADER     FRU_MULTI_RECORD_AREA
#define FRU_MULTI_RECORD_DATA       7

#define FRU_END_OF_RECORD           0xF

#define FRU_BLOCK_SIZE              8

#pragma pack(1)

/* FRUInventoryAreaInfoReq_T */
typedef struct
{
    INT8U FRUDeviceID;  /* 0xff is reserved */

} PACKED  FRUInventoryAreaInfoReq_T;


/* FRUInventoryAreaInfoRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT16U  Size;
    INT8U   DeviceAccessMode;

} PACKED  FRUInventoryAreaInfoRes_T;


/* FRUReadReq_T */
typedef struct
{
    INT8U   FRUDeviceID;
    INT16U  Offset;
    INT8U   CountToRead;

} PACKED  FRUReadReq_T;


/* FRUReadRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   CountReturned;

} PACKED  FRUReadRes_T;


/* FRUWriteReq_T */
typedef struct
{
    INT8U   FRUDeviceID;
    INT16U  Offset;

} PACKED  FRUWriteReq_T;


/* FRUWriteRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   CountWritten;

} PACKED  FRUWriteRes_T;

/**************************************************************/
/* FRU Data structures */
/**************************************************************/

/* Common Header */
typedef struct
{
    INT8U   CommonHeaderFormatVersion;
    INT8U   InternalUseAreaStartOffset; /* In multiple of 8 bytes. 0 means area not present */
    INT8U   ChassisInfoAreaStartOffset; /* In multiple of 8 bytes. 0 means area not present */
    INT8U   BoardInfoAreaStartOffset; 	/* In multiple of 8 bytes. 0 means area not present */
    INT8U   ProductInfoAreaStartOffset; /* In multiple of 8 bytes. 0 means area not present */
    INT8U   MultiRecordAreaStartOffset; /* In multiple of 8 bytes. 0 means area not present */
    INT8U   PAD;			/* 0x00 */
    INT8U   ZeroChecksum;		/* Common Header Zero checksum */
} PACKED  FRUCommonHeader_T;

/* Internal Use Area */
typedef struct
{
    INT8U   FormatVersion;
    INT8U   *Data;
} PACKED  FRUInternalInfo_T;

/* Chassis Info Area */
typedef enum
{
    Other = 0x01,
    ChassisUnknown,
    Desktop,
    LowProfileDesktop,
    PizzaBox,
    MiniTower,
    Tower,
    Portable,
    LapTop,
    Notebook,
    HandHeld,
    DockingStation,
    AllinOne,
    SubNotebook,
    SpaceSaving,
    LunchBox,
    MainServerChassis,
    ExpansionChassis,
    SubChassis,
    BusExpansionChassis,
    PeripheralChassis,
    RAIDChassis,
    RackMountChassis
} ChassisType_T;

typedef struct
{
    INT8U		ChassisInfoAreaFormatVersion;
    INT8U		ChassisInfoAreaLength;	/* In multiple of 8 bytes. */
    INT8U		*ChassisType;
    INT8U   		ChassisPartNumTypeLength;
    INT8U   		*ChassisPartNum;
    INT8U   		ChassisSerialNumTypeLength;
    INT8U   		*ChassisSerialNum;
    INT8U   		**CustomFields;
    INT8U           Total_Chassis_CustomField;
} PACKED  FRUChassisInfo_T;

/* Board Info Area */
typedef enum
{
    English = 0
} LanguageType_T;

typedef struct
{
    INT8U		BoardInfoAreaFormatVersion;
    INT8U		BoardInfoAreaLength;	/* In multiple of 8 bytes. */
    INT8U		Language;
    INT8U   		MfgDateTime[3];
    INT8U   		BoardMfrTypeLength;
    INT8U   		*BoardMfr;
    INT8U   		BoardProductNameTypeLength;
    INT8U   		*BoardProductName;
    INT8U   		BoardSerialNumTypeLength;
    INT8U   		*BoardSerialNum;
    INT8U   		BoardPartNumTypeLength;
    INT8U   		*BoardPartNum;
    INT8U   		FRUFileIDTypeLength;
    INT8U   		*FRUFileID;
    INT8U   		**CustomFields;
    INT8U           Total_Board_CustomField;

} PACKED  FRUBoardInfo_T;

/* Product Info Area */
typedef struct
{
    INT8U		ProductInfoAreaFormatVersion;
    INT8U		ProductInfoAreaLength;	/* In multiple of 8 bytes. */
    INT8U		Language;
    INT8U   		MfrNameTypeLength;
    INT8U   		*MfrName;
    INT8U   		ProductNameTypeLength;
    INT8U   		*ProductName;
    INT8U   		ProductPartNumTypeLength;
    INT8U   		*ProductPartNum;
    INT8U   		ProductVersionTypeLength;
    INT8U   		*ProductVersion;
    INT8U   		ProductSerialNumTypeLength;
    INT8U   		*ProductSerialNum;
    INT8U   		AssetTagTypeLength;
    INT8U   		*AssetTag;
    INT8U   		FRUFileIDTypeLength;
    INT8U   		*FRUFileID;
    INT8U   		**CustomFields;
    INT8U           Total_Product_CustomField;
} PACKED  FRUProductInfo_T;

/* MultiRecord Area */

typedef enum
{
    PowerSupplyInformation,
    DCOutput,
    DCLoad,
    ManagementAccessRecord,
    BaseCompatibilityRecord,
    ExtendedCompatibilityRecord
} RecordType_T;

typedef struct
{
    INT16U	OverallCapacity;
    INT16U	PeakVA;
    INT8U	InrushCurrent;
    INT8U	InrushIntervalinMSecs;
    INT16U	LowEndInVoltageRange1;	/* 10mV */
    INT16U	HighEndInVoltageRange1;	/* 10mV */
    INT16U	LowEndInVoltageRange2;	/* 10mV. 0 if single range */
    INT16U	HighEndInVoltageRange2;	/* 10mV. 0 if single range */
    INT8U	LowEndInFreqRange;
    INT8U	HighEndInFreqRange;
    INT8U	ACDropoutToleranceinMSecs;
    INT8U	FlagsPolarity;
    INT16U	PeakWattage;
    INT8U	CombinedWattage[3];
    INT8U	TechMtrLowerThresh;
} PACKED  PowerSupplyInformation_T;

typedef struct
{
    INT8U	OutputInfo;
    INT16U	NominalVoltage;		/* 10mV */
    INT16U	MaxNegVolDeviation;	/* 10mV */
    INT16U	MaxPosVolDeviation;	/* 10mV */
    INT16U	RippleNoise;		/* mV */
    INT16U	MinCurrentDraw;		/* mA */
    INT16U	MaxCurrentDraw;		/* mA */
} PACKED  DCOutput_T;

typedef struct
{
    INT8U	VoltageReqd;
    INT16U	NominalVoltage;		/* 10mV */
    INT16U	SpecdMinVoltage;	/* 10mV */
    INT16U	SpecdMaxVoltage;	/* 10mV */
    INT16U	SpecdRippleNoise;	/* mV */
    INT16U	MinCurrentLoad;		/* mA */
    INT16U	MaxCurrentLoad;		/* mA */
} PACKED  DCLoad_T;

typedef struct
{
    INT8U		RecordTypeID;
    INT8U		RecordFormatVersionEOL;
    INT8U		RecordLength;
    INT8U		RecordChecksum;	/* zero checksum */
    INT8U		HeaderChecksum;	/* zero checksum */
    INT8U		*RecordData;
} PACKED  MultiRecordHeader_T;

typedef struct
{
    MultiRecordHeader_T	RecordHeader;
    INT8U		*RecordData;
} PACKED  Record_T;

#define	MAX_MULTIRECORDS	64
typedef struct
{
    Record_T	MultiRecordsData[MAX_MULTIRECORDS];
} PACKED  FRUMultiRecordInfo_T;

/* Complete FRU Data */
typedef struct
{
    FRUCommonHeader_T   	CommonHeader;
    FRUInternalInfo_T		InternalInfo;
    FRUChassisInfo_T		ChassisInfo;
    FRUBoardInfo_T			BoardInfo;
    FRUProductInfo_T		ProductInfo;
    FRUMultiRecordInfo_T 	MultiRecordInfo;
} PACKED  FRUData_T;

#pragma pack()

#endif  /* IPMI_FRU_H */
