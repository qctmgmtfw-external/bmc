/**
 * @file   libipmi_StorDevice.h
 * @author Anurag Bhatia
 * @date   15-Sep-2004
 *
 * @brief  Contains exported APIs by LIBIPMI for
 *  		communicating with the BMC for NetFn type Storage.
 *
 */

#ifndef __LIBIPMI_STORDEVICE_H__
#define __LIBIPMI_STORDEVICE_H__

/* LIIPMI core header files */
#include "libipmi_session.h"
#include "libipmi_errorcodes.h"
#include "libipmiifc.h"

/* command specific header files */
#include "IPMI_SEL.h"
#include "IPMI_FRU.h"
#include "IPMI_IPMB.h"
#include "IPMI_Storage.h"
#include "SELRecord.h"

#include "libipmi_sdr.h"
#include "std_macros.h"

#ifdef  __cplusplus
extern "C" {
#endif

/*****************************************************************************
	System Event Log
******************************************************************************/
LIBIPMI_API uint16 IPMICMD_GetSELInfo(IPMI20_SESSION_T *pSession,
					SELInfo_T *pResGetSELInfo,
					int timeout);
LIBIPMI_API uint16 IPMICMD_GetSELAllocationInfo(IPMI20_SESSION_T *pSession,
					SELAllocInfo_T *pResGetSELAllocInfo,
					int timeout);
LIBIPMI_API uint16 IPMICMD_ReserveSEL(IPMI20_SESSION_T *pSession,
					ReserveSELRes_T* pReserveSelRes,
					int timeout);
LIBIPMI_API uint16 IPMICMD_GetSELEntry(IPMI20_SESSION_T *pSession,
					GetSELReq_T* pGetSELReqData,
					uint8* pGetSELResData,
					uint32 ResDataLen,
					int timeout);
LIBIPMI_API uint16 IPMICMD_AddSELEntry(IPMI20_SESSION_T *pSession,
					SELEventRecord_T* pSELReqData,
					AddSELRes_T* pSELResData,
					int timeout);
LIBIPMI_API uint16 IPMICMD_DeleteSELEntry(IPMI20_SESSION_T *pSession,
					uint16 RecordID,
					int timeout);

/*---------- LIBIPMI Higher level routines -----------*/
LIBIPMI_API uint16 LIBIPMI_HL_GetNumOfSELEntries(IPMI20_SESSION_T *pSession,
					INT16U* nNumOfSELEntries,
					int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_GetMaxPossibleSELEntries(IPMI20_SESSION_T *pSession,
					INT16U* nMaxPossibleSELEntries,
					int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_GetAllSELEntries(IPMI20_SESSION_T *pSession,
					SELEventRecord_T *pSELEntriesBuffer,
					int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_GetAllSensorInfo(IPMI20_SESSION_T *pSession,
					uint8 **SDRBuff,
					int *SDRCount,
					int *MaxSDRLen,
					int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_GetSensorName(IPMI20_SESSION_T *pSession,
					uint8 SlaveAdd,
					uint8 LUNid,
					uint8 SensorNum,
					uint8 *SensorName,
					int timeout);
uint16 LIBIPMI_HL_GetAllFRUDevice(IPMI20_SESSION_T *pSession,
					uint8 *pFRUIDs,
					uint8 *nFRUs,
					uint8 nFRUsName[][16],
					int timeout);

/*****************************************************************************
	Field Replacement Unit (FRU)
******************************************************************************/

#pragma pack(1)
/*this is a structure used by an abstraction function
for getallselentries
we want all sel entires but we also want sensor names*/
typedef struct _SELEventRecordWithSensorName
{
	SELRec_T EventRecord;
	SELOEM1Record_T  OEM1EventRecord; // Quanta
	SELOEM2Record_T  OEM2EventRecord; // Quanta
	uint8 SensorName[65]; //65 is the max possible because SensorName len in SDR is indicated by 6 bits
	INT8U            SensorUnits2; // Quanta
	double            current_reading; // Quanta
	double            threshold_value; // Quanta
} PACKED
SELEventRecordWithSensorName_T;

#ifndef MAX_FRU_DEVICES
#define MAX_FRU_DEVICES	256
#endif

#pragma pack(1)
typedef struct _SDRHeader
{
    /** The ID of this SDR, LSB first */
    INT8U RecordID[ 2 ];
    /** Version of this SDR (Not always the same as the IPMI version!) */
    INT8U SDRVersion;
    /** The type of this SDR, usually \ref SDR_FULL or \ref SDR_COMPACT */
    INT8U RecordType;
    /** The length of the SDR that follows this header */
    INT8U RecordLength;
}PACKED  SDRHeader;

#pragma pack()

uint8 libipmi_GetBits(uint8 orig, uint8 startbit, uint8 endbit);

//Having this for backward compatibility. Projects still using GetBits
//Will remove this macro when all projects stop using it
#define GetBits libipmi_GetBits

LIBIPMI_API uint16 IPMICMD_GetFRUInventoryAreaInfo(IPMI20_SESSION_T *pSession,
				FRUInventoryAreaInfoReq_T *pReqFRUInventoryAreaInfo,
				FRUInventoryAreaInfoRes_T *pResFRUInventoryAreaInfo,
				int timeout);

LIBIPMI_API uint16 IPMICMD_ReadFRUData(IPMI20_SESSION_T *pSession,
				FRUReadReq_T* pFRUReadReqData,
				FRUReadRes_T* pFRUReadResData,
				int timeout);

LIBIPMI_API uint16 IPMICMD_WriteFRUData(IPMI20_SESSION_T *pSession,
				FRUWriteReq_T* pFRUWriteReqData,
				uint32 ReqDataLen /* Bytes to be written + sizeof(FRUWriteReq_T) */,
				FRUWriteRes_T* pFRUWriteResData,
				int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_GetAllFRUDevices(IPMI20_SESSION_T *pSession,
				uint8 *pFRUIDs,
				uint8 *pFRUNames,
				uint8 *nFRUs,
				int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_GetAllSelEntriesWithSensorNames(IPMI20_SESSION_T *pSession,
	SELEventRecordWithSensorName_T *pSELEntriesBuffer, INT16U* nNumOfSELEntries,
	int timeout);

LIBIPMI_API uint16 IPMICMD_ClearSEL(IPMI20_SESSION_T *pSession,
				int timeout);

LIBIPMI_API uint8 IPMICMD_GetSELTime(IPMI20_SESSION_T *pSession,
				GetSELTimeRes_T *pGetSELTimeRes, int timeout);

LIBIPMI_API uint8 IPMICMD_SetSELTime(IPMI20_SESSION_T *pSession,
				SetSELTimeReq_T *pSetSELTimeReq, int timeout);

LIBIPMI_API uint8 IPMICMD_GetSELTimeUTCOffset(IPMI20_SESSION_T *pSession,
				GetSELTimeUTCOffsetRes_T *pGetSELTimeUTCOffsetRes, int timeout);

LIBIPMI_API uint8 IPMICMD_SetSELTimeUTCOffset(IPMI20_SESSION_T *pSession,
				SetSELTimeUTCOffsetReq_T *pSetSELTimeUTCOffsetReq, int timeout);


#ifdef  __cplusplus
}
#endif

#endif
