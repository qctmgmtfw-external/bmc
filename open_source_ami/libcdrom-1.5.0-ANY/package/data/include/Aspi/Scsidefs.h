////////////////////////////////////////////////////////////
//
// Module SCSIDEFS.H
//
// ASPI class library
// SCSI structures and definitions
//
// Project: A Programmer's Guide to SCSI
//
// Copyright (C) 1997, Brian Sawert
// Portions copyright (C) 1995, Larry Martin
// All rights reserved
//
////////////////////////////////////////////////////////////


#ifndef SCSIDEFS_H_INCLUDED
#define SCSIDEFS_H_INCLUDED

#pragma pack(push,1)

#define SCSI_Cmd_TestUnitReady      0x00
#define SCSI_Cmd_Rewind          0x01
#define SCSI_Cmd_RequestSense    0x03
#define SCSI_Cmd_FormatUnit         0x04
#define SCSI_Cmd_ReadBlockLimits 0x05
#define SCSI_Cmd_Read            0x08
#define SCSI_Cmd_Write           0x0A
#define SCSI_Cmd_WriteFilemarks     0x10
#define SCSI_Cmd_Space           0x11
#define SCSI_Cmd_Inquiry         0x12
#define SCSI_Cmd_ModeSelect         0x15
#define SCSI_Cmd_ReserveUnit     0x16
#define SCSI_Cmd_ReleaseUnit     0x17
#define SCSI_Cmd_Erase           0x19
#define SCSI_Cmd_ModeSense       0x1A
#define SCSI_Cmd_LoadUnload         0x1B
#define SCSI_Cmd_SendDiagnostic     0x1D
#define SCSI_Cmd_PreventAllow    0x1E
#define SCSI_Cmd_ReadCapacity    0x25
#define SCSI_Cmd_Locate          0x2B
#define SCSI_Cmd_ReadPosition    0x34
#define SCSI_Cmd_ReadToc         0x43



/* ********************* WARNING ********************************** */
/* The structures below expect that the compiler lays out bitfields */
/* LSB first.                                         */
/* **************************************************************** */

typedef struct SCSI_Cdb_Generic6_s {
   unsigned int CommandCode   : 8;
   unsigned int Reserved1     : 5;
   unsigned int Lun        : 3;
   unsigned int Reserved2     : 8;
   unsigned int Reserved3     : 8;
   unsigned int Reserved4     : 8;
   unsigned int Link       : 1;
   unsigned int Flag       : 1;
   unsigned int Reserved5     : 4;
   unsigned int VendorSpecific   : 2;
   } SCSI_Cdb_Generic6_t;

typedef struct SCSI_Cdb_TestUnitReady_s {
   unsigned int CommandCode   : 8;  /* should be 0x00 */
   unsigned int            : 5;  /* must be 0 */
   unsigned int Lun        : 3;
   unsigned int            : 8;  /* must be 0 */
   unsigned int            : 8;
   unsigned int            : 8;
   unsigned int Link       : 1;
   unsigned int Flag       : 1;
   unsigned int            : 4;
   unsigned int VendorSpecific   : 2;
   } SCSI_Cdb_TestUnitReady_t;

typedef struct SCSI_Cdb_Rewind_s {
   unsigned int CommandCode   : 8;  /* should be 0x01 */
   unsigned int Immediate     : 1;
   unsigned int            : 4;
   unsigned int Lun        : 3;
   unsigned int            : 8;
   unsigned int            : 8;
   unsigned int            : 8;
   unsigned int Link       : 1;
   unsigned int Flag       : 1;
   unsigned int            : 4;
   unsigned int VendorSpecific   : 2;
   } SCSI_Cdb_Rewind_t;

typedef struct SCSI_Cdb_RequestSense_s {
   unsigned int CommandCode      : 8;  /* should be 0x03 */
   unsigned int               : 5;  /* must be 0 */
   unsigned int Lun           : 3;
   unsigned int               : 8;  /* must be 0 */
   unsigned int               : 8;
   unsigned int AllocationLength : 8;
   unsigned int Link          : 1;
   unsigned int Flag          : 1;
   unsigned int               : 4;
   unsigned int VendorSpecific      : 2;
   } SCSI_Cdb_RequestSense_t;

typedef struct SCSI_SenseData_s {
   unsigned int ErrorCode           : 7;
   unsigned int Valid               : 1;
   unsigned int SegmentNumber       : 8;
   unsigned int SenseKey            : 4;
   unsigned int                  : 1;
   unsigned int IllegalLength       : 1;
   unsigned int EndOfMedia          : 1;
   unsigned int FileMark            : 1;
   unsigned int InformationByte3    : 8;
   unsigned int InformationByte2    : 8;
   unsigned int InformationByte1    : 8;
   unsigned int InformationByte0    : 8;
   unsigned int AdditionalLength    : 8;
   unsigned int                  : 8;
   unsigned int                  : 8;
   unsigned int                  : 8;
   unsigned int                  : 8;
   unsigned int Asc              : 8;
   unsigned int Asq              : 8;
   unsigned int                  : 8;
   unsigned int BitPointer          : 3;
   unsigned int BitPointerValid     : 1;
   unsigned int                  : 2;
   unsigned int CdbError            : 1;
   unsigned int SenseKeySpecificValid  : 1;
   } SCSI_SenseData_t;

// Sense keys
#define SCSI_Skey_NoSense        0x00
#define SCSI_Skey_RecoveredError 0x01
#define SCSI_Skey_NotReady       0x02
#define SCSI_Skey_MediumError    0x03
#define SCSI_Skey_HardwareError     0x04
#define SCSI_Skey_IllegalRequest 0x05
#define SCSI_Skey_UnitAttention     0x06
#define SCSI_Skey_DataProtect    0x07
#define SCSI_Skey_BlankCheck     0x08
#define SCSI_Skey_VendorSpecific 0x09
#define SCSI_Skey_CopyAborted    0x0A
#define SCSI_Skey_AbortedCommand 0x0B
#define SCSI_Skey_Equal          0x0C
#define SCSI_Skey_VolumeOverflow 0x0D
#define SCSI_Skey_Miscompare     0x0E
#define SCSI_Skey_Reserved       0x0F

typedef struct SCSI_Cdb_ReadBlockLimits_s {
   unsigned int CommandCode   : 8;  /* should be 0x05 */
   unsigned int            : 5;
   unsigned int Lun        : 3;
   unsigned int            : 8;
   unsigned int            : 8;
   unsigned int            : 8;
   unsigned int Link       : 1;
   unsigned int Flag       : 1;
   unsigned int            : 4;
   unsigned int VendorSpecific   : 2;
   } SCSI_Cdb_ReadBlockLimits_t;

typedef struct SCSI_BlockLimits_s {
   unsigned int               : 8;
   unsigned int MaxBlockSize2    : 8;
   unsigned int MaxBlockSize1    : 8;
   unsigned int MaxBlockSize0    : 8;
   unsigned int MinBlockSize1    : 8;
   unsigned int MinBlockSize0    : 8;
   } SCSI_BlockLimits_t;

typedef struct SCSI_Cdb_Read_s {
   unsigned int CommandCode      : 8;  /* should be 0x08 */
   unsigned int LBA2             : 5;
   unsigned int Lun              : 3;
   unsigned int LBA1             : 8;
   unsigned int LBA0             : 8;
   unsigned int Length           : 8;
   unsigned int Link             : 1;
   unsigned int Flag             : 1;
   unsigned int                  : 4;
   unsigned int VendorSpecific   : 2;
   } SCSI_Cdb_Read_t;


typedef struct SCSI_Cdb_Write_s {
   unsigned int CommandCode         : 8;  /* should be 0x08 */
   unsigned int Fixed               : 1;
   unsigned int                  : 4;
   unsigned int Lun              : 3;
   unsigned int Count2              : 8;
   unsigned int Count1              : 8;
   unsigned int Count0              : 8;
   unsigned int Link             : 1;
   unsigned int Flag             : 1;
   unsigned int                  : 4;
   unsigned int VendorSpecific         : 2;
   } SCSI_Cdb_Write_t;


typedef struct SCSI_Cdb_WriteFilemarks_s {
   unsigned int CommandCode   : 8;  /* should be 0x10 */
   unsigned int Immediate     : 1;
   unsigned int Setmark    : 1;
   unsigned int            : 3;
   unsigned int Lun        : 3;
   unsigned int Count2        : 8;
   unsigned int Count1        : 8;
   unsigned int Count0        : 8;
   unsigned int Link       : 1;
   unsigned int Flag       : 1;
   unsigned int            : 4;
   unsigned int VendorSpecific   : 2;
   } SCSI_Cdb_WriteFilemarks_t;

typedef struct SCSI_Cdb_Space_s {
   unsigned int CommandCode   : 8;  /* should be 0x11 */
   unsigned int Code       : 3;
   unsigned int            : 2;
   unsigned int Lun        : 3;
   unsigned int Count2        : 8;
   unsigned int Count1        : 8;
   unsigned int Count0        : 8;
   unsigned int Link       : 1;
   unsigned int Flag       : 1;
   unsigned int            : 4;
   unsigned int VendorSpecific   : 2;
   } SCSI_Cdb_Space_t;

typedef struct SCSI_Cdb_Inquiry_s {
   unsigned int CommandCode      : 8;  /* should be 0x12 */
   unsigned int Evpd          : 1;
   unsigned int               : 4;
   unsigned int Lun           : 3;
   unsigned int PageCode         : 8;
   unsigned int               : 8;
   unsigned int AllocationLength : 8;
   unsigned int Link          : 1;
   unsigned int Flag          : 1;
   unsigned int               : 4;
   unsigned int VendorSpecific      : 2;
   } SCSI_Cdb_Inquiry_t;

typedef struct SCSI_InquiryData_s {
   unsigned int DeviceType             : 5;
   unsigned int DeviceQualifier        : 3;
   unsigned int DeviceModifier            : 7;
   unsigned int RemovableMedia            : 1;
   unsigned int AnsiVersion            : 3;
   unsigned int EcmaVersion            : 3;
   unsigned int IsoVersion             : 2;
   unsigned int ResponseDataFormat        : 4;
   unsigned int                     : 2;
   unsigned int TermIoProcessSupport      : 1;
   unsigned int AsyncEventNotifySupport   : 1;
   unsigned int AdditionalLength       : 8;
   unsigned int                     : 8;
   unsigned int                     : 8;
   unsigned int SoftResetSupport       : 1;
   unsigned int CommandQueueSupport    : 1;
   unsigned int                     : 1;
   unsigned int LinkedCommandSupport      : 1;
   unsigned int SynchronousTransferSupport : 1;
   unsigned int WideBus16Support       : 1;
   unsigned int WideBus32Support       : 1;
   unsigned int RelativeAddressingSupport : 1;
   unsigned char VendorId[8];
   unsigned char ProductId[16];
   unsigned char ProductRevisionLevel[4];
   } SCSI_InquiryData_t;

typedef struct SCSI_Cdb_Erase_s {
   unsigned int CommandCode   : 8;
   unsigned int Long       : 1;
   unsigned int Immediate     : 1;
   unsigned int            : 3;
   unsigned int Lun        : 3;
   unsigned int            : 8;
   unsigned int            : 8;
   unsigned int            : 8;
   unsigned int Link       : 1;
   unsigned int Flag       : 1;
   unsigned int            : 4;
   unsigned int VendorSpecific   : 2;
   } SCSI_Cdb_Erase_t;

typedef struct SCSI_Cdb_Format_s {
   unsigned int CommandCode      : 8;
   unsigned int DefectListFormat : 3;
   unsigned int CompleteList     : 1;
   unsigned int FormatData       : 1;
   unsigned int Lun           : 3;
   unsigned int               : 8;
   unsigned int InterleaveByte1  : 8;
   unsigned int InterleaveByte0  : 8;
   unsigned int Link          : 1;
   unsigned int Flag          : 1;
   unsigned int               : 4;
   unsigned int VendorSpecific      : 2;
   } SCSI_Cdb_Format_t;

typedef struct SCSI_Cdb_LoadUnload_s {
   unsigned int CommandCode   : 8;  /* should be 0x1B */
   unsigned int Immediate     : 1;
   unsigned int               : 4;
   unsigned int Lun           : 3;
   unsigned int               : 8;
   unsigned int               : 8;
   unsigned int Start         : 1;
   unsigned int LoadEject     : 1;
   unsigned int               : 6;
   unsigned int Link          : 1;
   unsigned int Flag          : 1;
   unsigned int               : 4;
   unsigned int VendorSpecific   : 2;
   } SCSI_Cdb_LoadUnload_t;


typedef struct SCSI_Cdb_ModeSense_s {
   unsigned int CommandCode            : 8;  /* should be 0x1A */
   unsigned int                     : 3;
   unsigned int DisableBlockDescriptors   : 1;
   unsigned int                     : 1;
   unsigned int Lun                 : 3;
   unsigned int PageCode               : 6;
   unsigned int PageControl            : 2;
   unsigned int                     : 8;
   unsigned int AllocationLength       : 8;
   unsigned int Link                : 1;
   unsigned int Flag                : 1;
   unsigned int                     : 4;
   unsigned int VendorSpecific            : 2;
   } SCSI_Cdb_ModeSense_t;

typedef struct SCSI_ModeSenseData_s {
   unsigned int SenseDataLength        : 8;
   unsigned int MediumType             : 8;
   unsigned int Device_Specific        : 8;
   unsigned int BlockDescriptorLength     : 8;
   unsigned int DensityCode            : 8;
   unsigned int NumBlocks2             : 8;
   unsigned int NumBlocks1             : 8;
   unsigned int NumBlocks0             : 8;
   unsigned int Reserved1              : 8;
   unsigned int BlockLength2           : 8;
   unsigned int BlockLength1           : 8;
   unsigned int BlockLength0           : 8;
   } SCSI_ModeSenseData_t;

typedef struct SCSI_Cdb_ModeSelect_s {
   unsigned int CommandCode            : 8;  /* should be 0x1A */
   unsigned int SavePages              : 1;
   unsigned int                     : 3;
   unsigned int PageFormat             : 1;
   unsigned int Lun                 : 3;
   unsigned int                     : 8;
   unsigned int                     : 8;
   unsigned int ParameterListLength    : 8;
   unsigned int Link                : 1;
   unsigned int Flag                : 1;
   unsigned int                     : 4;
   unsigned int VendorSpecific            : 2;
   } SCSI_Cdb_ModeSelect_t;

typedef struct SCSI_ModeSelectData_s {
   unsigned int                     : 8;
   unsigned int                     : 8;
   unsigned int Speed                  : 4;
   unsigned int BufferedMode           : 3;
   unsigned int                     : 1;
   unsigned int BlockDescriptorLength     : 8;
   unsigned int DensityCode            : 8;
   unsigned int NumBlocks2             : 8;
   unsigned int NumBlocks1             : 8;
   unsigned int NumBlocks0             : 8;
   unsigned int                     : 8;
   unsigned int BlockLength2           : 8;
   unsigned int BlockLength1           : 8;
   unsigned int BlockLength0           : 8;
   } SCSI_ModeSelectData_t;



typedef struct SCSI_Cdb_ReserveUnit_s {
   unsigned int CommandCode            : 8;  /* should be 0x16 */
   unsigned int                     : 1;
   unsigned int ThirdPartyDeviceId        : 3;
   unsigned int ThirdPartyFlag            : 1;
   unsigned int Lun                 : 3;
   unsigned int                     : 8;
   unsigned int                     : 8;
   unsigned int                     : 8;
   unsigned int Link                : 1;
   unsigned int Flag                : 1;
   unsigned int                     : 4;
   unsigned int VendorSpecific            : 2;
   } SCSI_Cdb_ReserveUnit_t;

typedef struct SCSI_Cdb_ReleaseUnit_s {
   unsigned int CommandCode            : 8;  /* should be 0x17 */
   unsigned int                     : 1;
   unsigned int ThirdPartyDeviceId        : 3;
   unsigned int ThirdPartyFlag            : 1;
   unsigned int Lun                 : 3;
   unsigned int                     : 8;
   unsigned int                     : 8;
   unsigned int                     : 8;
   unsigned int Link                : 1;
   unsigned int Flag                : 1;
   unsigned int                     : 4;
   unsigned int VendorSpecific            : 2;
   } SCSI_Cdb_ReleaseUnit_t;


typedef struct SCSI_Cdb_SendDiagnostic_s {
   unsigned int CommandCode      : 8;  /* should be 0x12 */
   unsigned int UnitOfflineOk    : 1;
   unsigned int DeviceOfflineOk  : 1;
   unsigned int SelfTest         : 1;
   unsigned int               : 1;
   unsigned int PageFormat       : 1;
   unsigned int Lun           : 3;
   unsigned int               : 8;
   unsigned int ParmListLenByte1 : 8;
   unsigned int ParmListLenByte0 : 8;
   unsigned int Link          : 1;
   unsigned int Flag          : 1;
   unsigned int               : 4;
   unsigned int VendorSpecific      : 2;
   } SCSI_Cdb_SendDiagnostic_t;

typedef struct SCSI_Cdb_ReadCapacity_s {
   unsigned int CommandCode      : 8;  /* should be 0x25 */
   unsigned int RelAdr           : 1;
   unsigned int                  : 4;
   unsigned int Lun              : 3;
   unsigned int LBA3             : 8;
   unsigned int LBA2             : 8;
   unsigned int LBA1             : 8;
   unsigned int LBA0             : 8;
   unsigned int                  : 8;
   unsigned int                  : 8;
   unsigned int Partial          : 1;
   unsigned int Partition        : 7;
   unsigned int Link             : 1;
   unsigned int Flag             : 1;
   unsigned int                  : 4;
   unsigned int VendorSpecific   : 2;
   } SCSI_Cdb_ReadCapacity_t;


typedef struct SCSI_ReadCapacityData_s {
   unsigned int BlockAddress3    : 8;
   unsigned int BlockAddress2    : 8;
   unsigned int BlockAddress1    : 8;
   unsigned int BlockAddress0    : 8;
   unsigned int BlockSize3       : 8;
   unsigned int BlockSize2       : 8;
   unsigned int BlockSize1       : 8;
   unsigned int BlockSize0       : 8;
   } SCSI_ReadCapacityData_t;


typedef struct SCSI_Cdb_Locate_s {
   unsigned int CommandCode         : 8;  /* should be 0x2B */
   unsigned int Immediate           : 1;
   unsigned int ChangePartition     : 1;
   unsigned int BlockType           : 1;
   unsigned int                  : 2;
   unsigned int Lun              : 3;
   unsigned int                  : 8;
   unsigned int Count3              : 8;
   unsigned int Count2              : 8;
   unsigned int Count1              : 8;
   unsigned int Count0              : 8;
   unsigned int                  : 8;
   unsigned int Partition           : 8;
   unsigned int Link             : 1;
   unsigned int Flag             : 1;
   unsigned int                  : 4;
   unsigned int VendorSpecific         : 2;
   } SCSI_Cdb_Locate_t;


typedef struct SCSI_Cdb_ReadPosition_s {
   unsigned int CommandCode         : 8;  /* should be 0x34 */
   unsigned int BlockType           : 1;
   unsigned int                  : 4;
   unsigned int Lun              : 3;
   unsigned int                  : 8;
   unsigned int                  : 8;
   unsigned int                  : 8;
   unsigned int                  : 8;
   unsigned int                  : 8;
   unsigned int                  : 8;
   unsigned int                  : 8;
   unsigned int Link             : 1;
   unsigned int Flag             : 1;
   unsigned int                  : 4;
   unsigned int VendorSpecific         : 2;
   } SCSI_Cdb_ReadPosition_t;


typedef struct SCSI_ReadPositionData_s {
   unsigned int                  : 2;
   unsigned int BlockPositionUnknown   : 1;
   unsigned int                  : 3;
   unsigned int EndOfPartition         : 1;
   unsigned int BeginningOfPartition   : 1;
   unsigned int Partition           : 8;
   unsigned int                  : 8;
   unsigned int                  : 8;
   unsigned int FirstBlock3         : 8;
   unsigned int FirstBlock2         : 8;
   unsigned int FirstBlock1         : 8;
   unsigned int FirstBlock0         : 8;
   unsigned int LastBlock3          : 8;
   unsigned int LastBlock2          : 8;
   unsigned int LastBlock1          : 8;
   unsigned int LastBlock0          : 8;
   unsigned int                  : 8;
   unsigned int BlocksInBuffer2     : 8;
   unsigned int BlocksInBuffer1     : 8;
   unsigned int BlocksInBuffer0     : 8;
   unsigned int BytesInBuffer3         : 8;
   unsigned int BytesInBuffer2         : 8;
   unsigned int BytesInBuffer1         : 8;
   unsigned int BytesInBuffer0         : 8;
   } SCSI_ReadPositionData_t;


typedef struct SCSI_Cdb_PreventAllow_s {
   unsigned int CommandCode   : 8;
   unsigned int Reserved1     : 5;
   unsigned int Lun           : 3;
   unsigned int Reserved2     : 8;
   unsigned int Reserved3     : 8;
   unsigned int Prevent       : 1;
   unsigned int Reserved4     : 7;
   unsigned int Link          : 1;
   unsigned int Flag          : 1;
   unsigned int Reserved5     : 4;
   unsigned int VendorSpecific   : 2;
   } SCSI_Cdb_PreventAllow_t;


typedef struct SCSI_Cdb_ReadToc_s {
   unsigned int CommandCode      : 8;  /* should be 0x43 */
   unsigned int                  : 1;
   unsigned int MSF              : 1;
   unsigned int                  : 3;
   unsigned int Lun              : 3;
   unsigned int                  : 8;
   unsigned int                  : 8;
   unsigned int                  : 8;
   unsigned int                  : 8;
   unsigned int StartTrack       : 8;
   unsigned int Length1          : 8;
   unsigned int Length0          : 8;
   unsigned int Link             : 1;
   unsigned int Flag             : 1;
   unsigned int                  : 4;
   unsigned int VendorSpecific   : 2;
   } SCSI_Cdb_ReadToc_t;


typedef struct SCSI_ReadTocData_s {
   unsigned int Length1          : 8;
   unsigned int Length0          : 8;
   unsigned int FirstTrack       : 8;
   unsigned int LastTrack        : 8;
   } SCSI_ReadTocData_t;

typedef struct SCSI_TocDescriptor_s {
   unsigned int                  : 8;
   unsigned int Control          : 4;
   unsigned int ADR              : 4;
   unsigned int TrackNumber      : 8;
   unsigned int                  : 8;
   unsigned int AbsAddress3      : 8;
   unsigned int AbsAddress2      : 8;
   unsigned int AbsAddress1      : 8;
   unsigned int AbsAddress0      : 8;
   } SCSI_TocDescriptor_t;



#pragma pack(pop)

#endif
