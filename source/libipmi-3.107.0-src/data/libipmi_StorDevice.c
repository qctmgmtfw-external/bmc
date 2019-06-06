/*****************************************************************
******************************************************************
***                                                            ***
***        (C)Copyright 2008, American Megatrends Inc.         ***
***                                                            ***
***                    All Rights Reserved                     ***
***                                                            ***
***       5555 Oakbrook Parkway, Norcross, GA 30093, USA       ***
***                                                            ***
***                     Phone 770.246.8600                     ***
***                                                            ***
******************************************************************
******************************************************************
******************************************************************
*
* Filename: libipmi_StorDevice.c
*
* Description: Contains implementation of NetFn Storage
*   specific IPMI command functions
*
* Author: Anurag Bhatia
*
******************************************************************/
#include "libipmi_session.h"
#include "libipmi_errorcodes.h"
#include "libipmi_StorDevice.h"
#include "std_macros.h"
#include "IPMI_AMIDevice.h"
#include "libipmi_AMIOEM.h"
#include "sensor_helpers.h" // Quanta

#if ( _WIN64 || _WIN32 )
#define snprintf sprintf_s
#endif

#include <stdlib.h>
#include <string.h>
#include "dbgout.h"

/* Helper function */
uint8 libipmi_GetBits(uint8 orig, uint8 startbit, uint8 endbit)
{
	uint8  temp = orig;
	uint8  mask = 0x00;
	int i;
	for(i=startbit;i>=endbit;i--)
	{
		mask = mask | (1 << i);
	}

	return (temp & mask);
}

/*****************************************************************************
	System Event Log
******************************************************************************/
uint16 IPMICMD_GetSELInfo(IPMI20_SESSION_T *pSession,
				SELInfo_T *pResGetSELInfo,
				int timeout)
{
	uint16	wRet;
	uint32	dwResLen;
	uint8	DummyReqData[8];
	uint8	NetFnLunStorage = (NETFN_STORAGE << 2);

	dwResLen = sizeof(SELInfo_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
						NetFnLunStorage, CMD_GET_SEL_INFO,
						(uint8*)DummyReqData, 0,
						(uint8 *)pResGetSELInfo, &dwResLen,
						timeout);
	return wRet;
}



uint16 IPMICMD_GetSELAllocationInfo(IPMI20_SESSION_T *pSession,
					SELAllocInfo_T *pResGetSELAllocInfo,
					int timeout)
{
	uint16	wRet;
	uint32	dwResLen;
	uint8	DummyReqData[8];
	uint8	NetFnLunStorage = (NETFN_STORAGE << 2);

	dwResLen = sizeof(SELAllocInfo_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
						NetFnLunStorage, CMD_GET_SEL_ALLOCATION_INFO,
						(uint8*)DummyReqData, 0,
						(uint8 *)pResGetSELAllocInfo, &dwResLen,
						timeout);
	return wRet;
}

/* This function reserves SEL and returns SEL reservation ID */
uint16 IPMICMD_ReserveSEL(IPMI20_SESSION_T *pSession,
					ReserveSELRes_T* pReserveSelRes,
					int timeout)
{
	uint16	wRet = 0;
	uint32	dwResLen = 0;

	memset(pReserveSelRes, 0, sizeof(ReserveSELRes_T));

	dwResLen = sizeof(ReserveSELRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
						(NETFN_STORAGE << 2), CMD_RESERVE_SEL,
						NULL, 0,
						(uint8 *)pReserveSelRes, &dwResLen,
						timeout);

	if ( (wRet != 0) && (pReserveSelRes->CompletionCode != CC_INV_CMD) )
	{
		TCRIT("Could not get SEL reservation!!\n");
		return wRet;
	}

	/* If ReserveSEL command is not implemented, return 0x0000 as reservation ID */
	if ( pReserveSelRes->CompletionCode == CC_INV_CMD )
	{
		pReserveSelRes->ReservationID = 0x0000;
		wRet = 0;
	}

	return wRet;
}

/* This function gets an entry from SEL. */
uint16 IPMICMD_GetSELEntry(IPMI20_SESSION_T *pSession,
				GetSELReq_T* pGetSELReqData,
				uint8* pGetSELResData,
				uint32 ResDataLen,
				int timeout)
{
	uint16 wRet = 0;
	uint8 NetFnLunStorage = (NETFN_STORAGE << 2);

	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
						NetFnLunStorage, CMD_GET_SEL_ENTRY,
						(uint8*)pGetSELReqData, sizeof(GetSELReq_T),
						(uint8 *)pGetSELResData, &ResDataLen,
						timeout);
	return wRet;
}

/* This function adds an entry into SEL. */
uint16 IPMICMD_AddSELEntry(IPMI20_SESSION_T *pSession,
				SELEventRecord_T* pSELReqData,
				AddSELRes_T* pSELResData,
				int timeout)
{
	uint16 wRet = 0;
	uint32	dwResLen;
	uint8 NetFnLunStorage = (NETFN_STORAGE << 2);

	dwResLen = sizeof(AddSELRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
						NetFnLunStorage, CMD_ADD_SEL_ENTRY,
						(uint8*)pSELReqData, sizeof(SELEventRecord_T),
						(uint8 *)pSELResData, &dwResLen,
						timeout);
	return wRet;
}

/* This function deletes an entry from SEL. */
uint16 IPMICMD_DeleteSELEntry(IPMI20_SESSION_T *pSession,
				uint16 RecordID,
				int timeout)
{
	uint16			wRet = 0;
	uint32			dwResLen = 0;
	ReserveSELRes_T	ReserveSelRes;
	DeleteSELReq_T	DeleteSELReqData;
	DeleteSELRes_T	DeleteSELResData;

	memset(&ReserveSelRes, 0, sizeof(ReserveSelRes));
	memset(&DeleteSELReqData, 0, sizeof(DeleteSELReqData));
	memset(&DeleteSELResData, 0, sizeof(DeleteSELResData));

	//first do a SEL reservation
	dwResLen = sizeof(ReserveSELRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
						(NETFN_STORAGE << 2), CMD_RESERVE_SEL,
						NULL, 0,
						(uint8 *)&ReserveSelRes, &dwResLen,
						timeout);
	if ( (wRet != 0) && (ReserveSelRes.CompletionCode != CC_INV_CMD) )
	{
		TCRIT("Could not get SEL reservation while trying to delete event log!!\n");
		return wRet;
	}

	if (ReserveSelRes.CompletionCode == CC_INV_CMD)
	{
		// Reservation ID should be set as 0x0000 for implementations that don't
		// implement ReserveSEL command
		DeleteSELReqData.ReservationID = 0x0000;
	}
	else
	{
		DeleteSELReqData.ReservationID = ReserveSelRes.ReservationID;
	}

	DeleteSELReqData.RecID = RecordID;

	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
						(NETFN_STORAGE << 2), CMD_DELETE_SEL_ENTRY,
						(uint8*)&DeleteSELReqData, sizeof(DeleteSELReqData),
						(uint8 *)&DeleteSELResData, &dwResLen,
						timeout);
	return wRet;
}

uint16 IPMICMD_ClearSEL(IPMI20_SESSION_T *pSession,
				int timeout)
{
	uint16	wRet;
	uint32	dwResLen;
	ClearSELReq_T ClearSelReq;
	ClearSELRes_T ClearSelRes;
	ReserveSELRes_T ReserveSelRes;

	uint8 DummyReqData[8];

	uint8	NetFnLunStorage = (NETFN_STORAGE << 2);

	//first do a SEL reservation
 	dwResLen = sizeof(ReserveSELRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
						NetFnLunStorage, CMD_RESERVE_SEL,
						(uint8*)DummyReqData, 0,
						(uint8 *)&ReserveSelRes, &dwResLen,
						timeout);

	if ( (wRet != 0) && (ReserveSelRes.CompletionCode != CC_INV_CMD) )
	{
		TCRIT("Could not get SEL reservation while trying to clear event log!!\n");
		return wRet;
	}

	if (ReserveSelRes.CompletionCode == CC_INV_CMD)
	{
		// Reservation ID should be set as 0x0000 for implementations that don't
		// implement ReserveSEL command
		ClearSelReq.ReservationID = 0x0000;
	}
	else
	{
		ClearSelReq.ReservationID = ReserveSelRes.ReservationID;
	}

	ClearSelReq.CLR[0] = 'C';
	ClearSelReq.CLR[1] = 'L';
	ClearSelReq.CLR[2] = 'R';
	ClearSelReq.InitOrStatus = 0xAA;

	dwResLen = sizeof(ClearSELRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
						NetFnLunStorage, CMD_CLEAR_SEL,
						(uint8*)&ClearSelReq, sizeof(ClearSELReq_T),
						(uint8 *)&ClearSelRes, &dwResLen,
						timeout);
	return wRet;
}
/**************************************************************************************
*****************************Higher level commands*****************************
**************************************************************************************/
/* This function returns total number of SEL entries present in the system at that time */
uint16 LIBIPMI_HL_GetNumOfSELEntries(IPMI20_SESSION_T *pSession,
					INT16U* nNumOfSELEntries,
					int timeout)
{
	uint16 wRet = 0;
	SELInfo_T SELInfo;

	wRet =  IPMICMD_GetSELInfo(pSession,&SELInfo,timeout);
	if(wRet != 0)
	{
		TCRIT("LIBIPMI_HL_GetNumOfSELEntries: Error getting SEL info\n");
		return wRet;
	}
	*nNumOfSELEntries = ipmitoh_u16(SELInfo.RecCt);
	return wRet;
}

/* This function returns max possible number of SEL entries in the system at any time */
uint16 LIBIPMI_HL_GetMaxPossibleSELEntries(IPMI20_SESSION_T *pSession,
						INT16U* nMaxPossibleSELEntries,
						int timeout)
{
	uint16 wRet = 0;
	SELAllocInfo_T SELAllocInfo;

	wRet =  IPMICMD_GetSELAllocationInfo(pSession,&SELAllocInfo,timeout);
	if(wRet != 0)
	{
		TCRIT("LIBIPMI_HL_GetMaxPossibleSELEntries: Error getting SEL Alloc info\n");
		return wRet;
	}
	*nMaxPossibleSELEntries = ipmitoh_u16(SELAllocInfo.NumAllocUnits);
	return wRet;
}

/* This function returns total number of SEL entries present in the system at that time.
   IMPORTANT:
   Before you call this function, call LIBIPMI_HL_GetNumOfSELEntries() to get the number of
   SEL entries present in the system, then allocate a buffer size of ((Num of Entries) * SELEventRecord_T)
   to hold the total number of SEL entries. Pass the pointer to this buffer as pSELEntriesBuffer when
   calling this function.
   Alternatively, you can call LIBIPMI_HL_GetMaxPossibleSELEntries() to get the maximum possible
   number of SEL entries present in the system at any given time, and allocate the buffer to hold those
   many entries. It's just that this approach would allocate a bigger buffer size. */
uint16 LIBIPMI_HL_GetAllSELEntries(IPMI20_SESSION_T *pSession,
					SELEventRecord_T *pSELEntriesBuffer,
					int timeout)
{
	GetSELReq_T GetSELReqData = {0x00,0x00,0x00,0xFF}; // Request packet to get the very
							// first SEL record and complete record,
													// starting at offset 0 of each record.
	uint8 *GetSELResData = NULL;
	SELEventRecord_T *ptrToSELRecord  = NULL;
	uint32 DataLen = 0;
	uint16 wRet = 0;

	/* We want to read full records */
	DataLen = sizeof(GetSELRes_T) + sizeof(SELEventRecord_T);
	GetSELResData = (uint8 *) malloc(DataLen);
	if (NULL == GetSELResData)
	{
		return LIBIPMI_E_NO_ENOUGH_MEMORY;
	}

	/* Fill in the Record data first */
	while(1)
	{
		wRet = IPMICMD_GetSELEntry(pSession, &GetSELReqData, GetSELResData, DataLen,timeout);
		if(wRet == LIBIPMI_E_SUCCESS)
		{
			ptrToSELRecord = (SELEventRecord_T *) (GetSELResData + sizeof(GetSELRes_T));
			memcpy(pSELEntriesBuffer,ptrToSELRecord,sizeof(SELEventRecord_T));
			pSELEntriesBuffer++;
			GetSELReqData.RecID = ((GetSELRes_T *) GetSELResData)->NextRecID;
			if(  ((GetSELRes_T *) GetSELResData)->NextRecID == 0xFFFF)
			{
				/* End of SEL reached. */
				break;
			}
		}
		else
		{
			TCRIT("LIBIPMI_HL_GetAllSELEntries: Error getting SEL entry\n");
			break;
		}
	}

	free(GetSELResData);
	return wRet;
}
uint16 LIBIPMI_HL_GetAllSensorInfo(IPMI20_SESSION_T *pSession,
					uint8 **SDRBuff,
					int *SDRCount,
					int *MaxSDRLen,
					int timeout)
{
	uint16 wRet = 0;
	SDRRepositoryAllocInfo_T SDRAllocationInfo;
	SDRRepositoryInfo_T SDRReposInfo;
    uint8 *pSDRBuff=NULL;
    	int MaxLen = 0;
    	int SDRCnt = 0;

	/* Get max buffer size to be allocated to read all SDRs in the system */
	wRet = IPMICMD_GetSDRRepositoryAllocInfo(pSession, &SDRAllocationInfo, timeout);
	if( wRet != 0 )
	{
		TCRIT("Error getting SDR Repository Allocation Info\n");
		return wRet;
	}
	/* Get repository information */
	wRet = IPMICMD_GetSDRRepositoryInfo(pSession, &SDRReposInfo,timeout);
	if( wRet != 0 )
	{
		TCRIT("Error getting SDR Repository Info\n");
		return wRet;
	}

	/* Determine max SDR entry length */
	MaxLen = SDRAllocationInfo.MaxRecSize * ipmitoh_u16( SDRAllocationInfo.AllocUnitSize );

	/* Determine total number of SDRs stored */
	SDRCnt = ipmitoh_u16( SDRReposInfo.RecCt );

	/* Allocate the buffer */
	pSDRBuff = (uint8 *) malloc(MaxLen * SDRCnt);
	if (NULL == pSDRBuff)
	{
		return LIBIPMI_E_NO_ENOUGH_MEMORY;
	}


	/* Call the function to read all the SDRs into this buffer */
	wRet = LIBIPMI_HL_GetAllSDRs(pSession,
                                     pSDRBuff,(MaxLen * SDRCnt),
                                     timeout*4);
	*MaxSDRLen=MaxLen;
	*SDRCount=SDRCnt;
	*SDRBuff=pSDRBuff;

	return wRet;
}

uint16 LIBIPMI_HL_GetSensorName(IPMI20_SESSION_T *pSession,
					uint8 SlaveAdd,
					uint8 LUNid,
					uint8 SensorNum,
					uint8 *SensorName,
					int timeout)
{
	uint16 wRet = 0;
	SDRRepositoryAllocInfo_T SDRAllocationInfo;
	SDRRepositoryInfo_T SDRReposInfo;
    	int MaxSDRLen = 0;
    	int i,SDRCount = 0;
	uint8 *pSDRBuff = NULL;
	SDRHeader *pSDRHeader = NULL;
	uint8 *pSDRStart = NULL;
	if(0)
	{
		SlaveAdd=SlaveAdd;  /*-Wextra: Flag added for strict compilation*/
		LUNid=LUNid;
	}

	/* Get max buffer size to be allocated to read all SDRs in the system */
	wRet = IPMICMD_GetSDRRepositoryAllocInfo(pSession, &SDRAllocationInfo, timeout);
	if( wRet != 0 )
	{
		TCRIT("Error getting SDR Repository Allocation Info\n");
		return wRet;
	}
	/* Get repository information */
	wRet = IPMICMD_GetSDRRepositoryInfo(pSession, &SDRReposInfo,timeout);
	if( wRet != 0 )
	{
		TCRIT("Error getting SDR Repository Info\n");
		return wRet;
	}

	/* Determine max SDR entry length */
	MaxSDRLen = SDRAllocationInfo.MaxRecSize * ipmitoh_u16( SDRAllocationInfo.AllocUnitSize );

	/* Determine total number of SDRs stored */
	SDRCount = ipmitoh_u16( SDRReposInfo.RecCt );

	/* Allocate the buffer */
	pSDRBuff = (uint8 *) malloc(MaxSDRLen * SDRCount);
	if (NULL == pSDRBuff)
	{
		return LIBIPMI_E_NO_ENOUGH_MEMORY;
	}
	pSDRStart = pSDRBuff;

	/* Call the function to read all the SDRs into this buffer */
	wRet = LIBIPMI_HL_GetAllSDRs(pSession,
                                     pSDRBuff,(MaxSDRLen * SDRCount),
                                     timeout*4);

	for( i = 0; i < SDRCount; i++)
	{
		pSDRHeader = (SDRHeader *) pSDRBuff;
		if((pSDRHeader->RecordType >= 0x01) && (pSDRHeader->RecordType <= 0x03))
		{
			/* Compare SlaveAdd and LUNid fields */
			uint8 *pTmp = pSDRBuff + sizeof(SDRHeader);
			uint8 len = 0;

			//if((SlaveAdd == *pTmp) && (LUNid == *(pTmp+1)))
			//{
				/* Now compare the sensor number */
				if(SensorNum == *(pTmp+2))
				{
					/* Get the Name and return */
					if(pSDRHeader->RecordType == 0x01)
						pTmp = pSDRBuff + sizeof(SDRHeader) + 42;
					else if(pSDRHeader->RecordType == 0x02)
						pTmp = pSDRBuff + sizeof(SDRHeader) + 26;
					else
						pTmp = pSDRBuff + sizeof(SDRHeader) + 11;

					len = GetBits(*pTmp,5,0);
					if(len)
					{
						pTmp++;
						memcpy(SensorName,pTmp,len);
						SensorName[len] = 0;
					}
					else
						SensorName[0]='\0';
					break; //Sensorname is obtained for given sensornumber. So no Need to check remaining Sensor.
				}
			//}
		}
		pSDRBuff += MaxSDRLen;
	}
	free(pSDRStart);
	return wRet;
}

/*this function wants to be nice to all nice people in the world who display
sensor information. Instead of letting them go nuts getting all sel entires first
an then looping thru all sel entires to go get all the sensor names and then each time to
get a sensor name read SDR
it just does that for them
GetAllSelEntires
Also GetAllSDR stuff once
then loop and fill it all up internally*/
uint16 LIBIPMI_HL_GetAllSelEntriesWithSensorNames(IPMI20_SESSION_T *pSession,
					SELEventRecordWithSensorName_T *pSELEntriesBuffer, INT16U* nNumOfSELEntries,
					int timeout)
{

	// first SEL record and complete record,
	// starting at offset 0 of each record.
	//uint32 DataLen = 0;
	uint16 wRet = 0;
	SDRRepositoryAllocInfo_T SDRAllocationInfo;
	SDRRepositoryInfo_T SDRReposInfo;
    	int MaxSDRLen = 0;
    	int i,SDRCount = 0;
	uint8 *pSDRBuff = NULL;
	SDRHeader *pSDRHeader = NULL;
	uint8 *pSDRStart = NULL;
	uint32 nNumSelEntries = 0;
	unsigned int q = 0;
	int tempret=0;/*To check return value of snprintf, it is not used any where else*/


	/***********************************Get all Sel Entries now*********************************************/
	/* We want to read full records */
	//DataLen = sizeof(GetSELRes_T) + sizeof(SELEventRecord_T);

	wRet = LIBIPMI_HL_AMIGetSELEntires(pSession,pSELEntriesBuffer,&nNumSelEntries,timeout);
	if(wRet != LIBIPMI_E_SUCCESS)
	{
		TCRIT("LIBIPMI_HL_AMIGetSELEntires : Error getting SEL Entries \n");
		return wRet;
	}

	*nNumOfSELEntries = (INT16U)nNumSelEntries;

	/*************************************Get The SDR once so that we can get sensor names*******************/
	/* Get max buffer size to be allocated to read all SDRs in the system */
	wRet = IPMICMD_GetSDRRepositoryAllocInfo(pSession, &SDRAllocationInfo, timeout);
	if( wRet != 0 )
	{
		TCRIT("Error getting SDR Repository Allocation Info\n");
		return wRet;
	}
	/* Get repository information */
	wRet = IPMICMD_GetSDRRepositoryInfo(pSession, &SDRReposInfo,timeout);
	if( wRet != 0 )
	{
		TCRIT("Error getting SDR Repository Info\n");
		return wRet;
	}

	/* Determine max SDR entry length */
	MaxSDRLen = SDRAllocationInfo.MaxRecSize * ipmitoh_u16( SDRAllocationInfo.AllocUnitSize );

	/* Determine total number of SDRs stored */
	SDRCount = ipmitoh_u16( SDRReposInfo.RecCt );

	/* Allocate the buffer */
	pSDRBuff = (uint8 *) malloc(MaxSDRLen * SDRCount);
	if(!pSDRBuff)
	{
		TCRIT("Error allocating memopry for SDRs in GetAllEventsWithSensorNames\n");
		wRet = STATUS_CODE(IPMI_ERROR_FLAG,CC_OUT_OF_SPACE);
		return wRet;
	}
	pSDRStart = pSDRBuff;

	/* Call the function to read all the SDRs into this buffer */
	wRet = LIBIPMI_HL_GetAllSDRs(pSession,
                                     pSDRBuff,(MaxSDRLen * SDRCount),
                                     timeout*4);

	/*************************************Get The SDR once so that we can get sensor names*******************/

	/******************Now parse through sensor headers and get names corresponding to each event************/
	for(q=0;q<nNumSelEntries;q++)
	{

		tempret=snprintf((char*)pSELEntriesBuffer[q].SensorName,sizeof(pSELEntriesBuffer[q].SensorName),"%s",(char*)"Unknown");
		if((tempret<0)||(tempret>=(signed)sizeof(pSELEntriesBuffer[q].SensorName)))
                {
                    TCRIT("Buffer Overflow");
                    return -1;
                }

		pSDRBuff = pSDRStart;
		for( i = 0; i < SDRCount; i++)
		{
			pSDRHeader = (SDRHeader *) pSDRBuff;
			if((pSDRHeader->RecordType >= 0x01) && (pSDRHeader->RecordType <= 0x03))
			{
				/* Compare SlaveAdd and LUNid fields */
				uint8 *pTmp = pSDRBuff + sizeof(SDRHeader);
				uint8 len = 0;



				//if((pSELEntriesBuffer[q].EventRecord.EvtRecord.GenID[0] == *pTmp) && (pSELEntriesBuffer[q].EventRecord.EvtRecord.GenID[1] == *(pTmp+1)))
				//{ // Quanta - not compare generate ID

					/* Now compare the sensor number */
					if(pSELEntriesBuffer[q].EventRecord.EvtRecord.SensorNum == *(pTmp+2))
					{
						/* Get the Name and return */
						if(pSDRHeader->RecordType == 0x01)
							pTmp = pSDRBuff + sizeof(SDRHeader) + 42;
						else if(pSDRHeader->RecordType == 0x02)
							pTmp = pSDRBuff + sizeof(SDRHeader) + 26;
						else
							pTmp = pSDRBuff + sizeof(SDRHeader) + 11;

						len = GetBits(*pTmp,5,0);
						if(len)
						{
							pTmp++;

							memcpy(pSELEntriesBuffer[q].SensorName,pTmp,len);
							pSELEntriesBuffer[q].SensorName[len] = 0;

						}
						else
						{
					    	    tempret=snprintf((char*)pSELEntriesBuffer[q].SensorName,sizeof(pSELEntriesBuffer[q].SensorName),"%s",(char*)"Unknown");
		                                    if((tempret<0)||(tempret>=(signed)sizeof(pSELEntriesBuffer[q].SensorName)))
                                                    {
                                                        TCRIT("Buffer Overflow");
                                                        return -1;
                                                    }
					        }
									// Quanta --- Get current reading and threshold value for threshold sensor.
									if((pSELEntriesBuffer[q].EventRecord.EvtRecord.EvtDirType & 0x3F) == 0x01)
									{
										ipmi_convert_reading(pSDRBuff, pSELEntriesBuffer[q].EventRecord.EvtRecord.EvtData2, &pSELEntriesBuffer[q].current_reading);

										ipmi_convert_reading(pSDRBuff, pSELEntriesBuffer[q].EventRecord.EvtRecord.EvtData3, &pSELEntriesBuffer[q].threshold_value);
									}
									// --- Quanta
					}
				//}// --- Quanta
				if(pSELEntriesBuffer[q].EventRecord.EvtRecord.hdr.Type == 0xDF)
				{
					tempret=snprintf((char*)pSELEntriesBuffer[q].SensorName,sizeof(pSELEntriesBuffer[q].SensorName),"%s",(char*)"Extended SEL");
		                        if((tempret<0)||(tempret>=(signed)sizeof(pSELEntriesBuffer[q].SensorName)))
                                        {
                                            TCRIT("Buffer Overflow");
                                            return -1;
                                        }
				}
			}
			pSDRBuff += MaxSDRLen;
		}
	}
	free(pSDRStart);
	return wRet;
}

/*****************************************************************************
	Field Replacement Unit (FRU)
******************************************************************************/
uint16 IPMICMD_GetFRUInventoryAreaInfo(IPMI20_SESSION_T *pSession,
				FRUInventoryAreaInfoReq_T *pReqFRUInventoryAreaInfo,
				FRUInventoryAreaInfoRes_T *pResFRUInventoryAreaInfo,
				int timeout)
{
	uint16	wRet,tmp;
	uint32	dwResLen;
	uint8	NetFnLunStorage = (NETFN_STORAGE << 2);

	dwResLen = sizeof(FRUInventoryAreaInfoRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
						NetFnLunStorage, CMD_FRU_INVENTORY_AREA_INFO,
						(uint8 *)pReqFRUInventoryAreaInfo, sizeof(FRUInventoryAreaInfoReq_T),
						(uint8 *)pResFRUInventoryAreaInfo, &dwResLen,
						timeout);
	/* Do we need to do the following??? I guess so...test and make sure */
	tmp = ipmitoh_u16(pResFRUInventoryAreaInfo->Size);
	pResFRUInventoryAreaInfo->Size = tmp;
	return wRet;
}

uint16 IPMICMD_ReadFRUData(IPMI20_SESSION_T *pSession,
				FRUReadReq_T* pFRUReadReqData,
				FRUReadRes_T* pFRUReadResData,
				int timeout)
{
	uint16 wRet = 0;
	uint32	dwResLen;
	uint8 NetFnLunStorage = (NETFN_STORAGE << 2);

	dwResLen = sizeof(FRUReadRes_T) + (pFRUReadReqData->CountToRead);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
						NetFnLunStorage, CMD_READ_FRU_DATA,
						(uint8 *)pFRUReadReqData, sizeof(FRUReadReq_T),
						(uint8 *)pFRUReadResData, &dwResLen,
						timeout);
	return wRet;
}

uint16 IPMICMD_WriteFRUData(IPMI20_SESSION_T *pSession,
				FRUWriteReq_T* pFRUWriteReqData,
				uint32 ReqDataLen /* Bytes to be written + sizeof(FRUWriteReq_T) */,
				FRUWriteRes_T* pFRUWriteResData,
				int timeout)
{
	uint16 wRet = 0;
	uint32	dwResLen;
	uint8 NetFnLunStorage = (NETFN_STORAGE << 2);

	dwResLen = sizeof(FRUWriteRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
						NetFnLunStorage, CMD_WRITE_FRU_DATA,
						(uint8 *)pFRUWriteReqData, ReqDataLen,
						(uint8 *)pFRUWriteResData, &dwResLen,
						timeout);
	return wRet;
}

uint16 LIBIPMI_HL_GetAllFRUDevices(IPMI20_SESSION_T *pSession,
					uint8 *pFRUIDs,
					uint8 *pFRUNames,
					uint8 *nFRUs,
					int timeout)
{
	uint16 wRet = 0;
	SDRRepositoryAllocInfo_T SDRAllocationInfo;
	SDRRepositoryInfo_T SDRReposInfo;
	uint8 *pSDRBuff = NULL;
    	int MaxSDRLen = 0;
    	int i,SDRCount = 0;
	SDRHeader *pSDRHeader = NULL;
	uint8 *pSDRStart = NULL;

	*nFRUs = 0;

	/* Get max buffer size to be allocated to read all SDRs in the system */
	wRet = IPMICMD_GetSDRRepositoryAllocInfo(pSession, &SDRAllocationInfo, timeout);
	if( wRet != 0 )
	{
		TCRIT("Error getting SDR Repository Allocation Info\n");
		return wRet;
	}
	/* Get repository information */
	wRet = IPMICMD_GetSDRRepositoryInfo(pSession, &SDRReposInfo,timeout);
	if( wRet != 0 )
	{
		TCRIT("Error getting SDR Repository Info\n");
		return wRet;
	}

	/* Determine max SDR entry length */
	MaxSDRLen = SDRAllocationInfo.MaxRecSize * ipmitoh_u16( SDRAllocationInfo.AllocUnitSize );

//	printf("MaxSDRLen = %d\n",MaxSDRLen);
	/* Determine total number of SDRs stored */
	SDRCount = ipmitoh_u16( SDRReposInfo.RecCt );
//	printf("SDRCount = %d\n",SDRCount);

	/* Allocate the buffer */
	pSDRBuff = (uint8 *) malloc(MaxSDRLen * SDRCount);
	if (NULL == pSDRBuff)
	{
		return LIBIPMI_E_NO_ENOUGH_MEMORY;
	}
	memset(pSDRBuff,0,(MaxSDRLen*SDRCount));
	pSDRStart = pSDRBuff;

	/* Call the function to read all the SDRs into this buffer */
	wRet = LIBIPMI_HL_GetAllSDRs(pSession,
                                     pSDRBuff,(MaxSDRLen * SDRCount),
                                     timeout*4);
	if(wRet != 0)
	{
		TCRIT("Error getting All SDRs\n");
        free(pSDRBuff);  //Quanta coverity 
		return wRet;
	}
	/* Search for all the SDRs of type '0x11' and '0x12' */
	/* Check SDR type '0x12' to see if FRU device is supported.
	   If yes, then FRU device id = 0.
	 	OR
	   Check SDR type '0x11' to get FRU device id.

	   Store these device ids in FRUDeviceIDs[].
	*/
	for( i = 0; i < SDRCount; i++)
	{
		uint32 len = 0;

		pSDRHeader = (SDRHeader *) pSDRBuff;
		if(pSDRHeader->RecordType == 0x11)
		{
			uint8 *pTmp = pSDRBuff + sizeof(SDRHeader) + 1;
			pFRUIDs[*nFRUs] = *pTmp;

//			printf("FRU Id = %d\n",pFRUIDs[*nFRUs]);
			/* Get device name now */
			pTmp = pSDRBuff + sizeof(SDRHeader) + 10;
			len = GetBits(*pTmp,5,0);
			if(len)
			{
				pTmp++;
				memcpy(pFRUNames,pTmp,len);
				pFRUNames[len] = 0;
			}
			else
				pFRUNames[0] = '\0';

//			printf("libipmi: FRUName = %s\n",pFRUNames);
			pFRUNames += 33;
			(*nFRUs)++;
		}
		else if(pSDRHeader->RecordType == 0x12)
		{
			uint8 *pTmp = pSDRBuff + sizeof(SDRHeader) + 3;
			if(GetBits(*pTmp,3,3))
			{
				pFRUIDs[*nFRUs] = 0;
//				printf("FRU Id = %d\n",pFRUIDs[*nFRUs]);
				/* Get device name now */
				pTmp = pSDRBuff + sizeof(SDRHeader) + 10;
				len = GetBits(*pTmp,5,0);
				if(len)
				{
					pTmp++;
					memcpy(pFRUNames,pTmp,len);
					pFRUNames[len] = 0;
				}
				else
					pFRUNames[0] = '\0';

//				printf("libipmi: FRUName = %s\n",pFRUNames);
				pFRUNames += 33;
				(*nFRUs)++;
			}
		}
		pSDRBuff += MaxSDRLen;
	}
	free(pSDRStart);
//	printf("numFRUs = %d\n",*nFRUs);
	return wRet;
}
uint16 LIBIPMI_HL_GetAllFRUDevice(IPMI20_SESSION_T *pSession,
					uint8 *pFRUIDs,
					uint8 *nFRUs,
					uint8 nFRUsName[][16],
					int timeout)
{
    uint16 wRet = 0,i=0;
    AMIGetFruDetailReq_T GetFruDetReq;
    AMIGetFruDetailRes_T GetFruDetRes;
    int tempret=0;/*To check return value of snprintf, it is not used any where else*/

     GetFruDetReq.FruReq = 0xFF;
     /* To get the total fru's */
    wRet = IPMICMD_FRUDetails(pSession, &GetFruDetReq,&GetFruDetRes, timeout);
    if( wRet != 0 )
    {
        TCRIT("Error getting total fru's\n");
        return wRet;
    }
   *nFRUs = GetFruDetRes.TotalFru;
    for (i=0;i< *nFRUs;i++)
    {
        GetFruDetReq.FruReq = i;
        /* To get the total fru's */
        wRet = IPMICMD_FRUDetails(pSession, &GetFruDetReq,&GetFruDetRes, timeout);
        if( wRet != 0 )
        {
            TCRIT("Error getting total fru's\n");
            return wRet;
        }
        pFRUIDs[i] = GetFruDetRes.DeviceNo;
        tempret=snprintf((char *)nFRUsName[i],sizeof(nFRUsName[i]),"%s",(char *)GetFruDetRes.FRUName);
	if((tempret<0)||(tempret>=(signed)sizeof(nFRUsName[i])))
	{
	    TCRIT("Buffer Overflow");
	    return -1;
	}
    }
    return 0;
}

uint8 IPMICMD_GetSELTime(IPMI20_SESSION_T *pSession,
				GetSELTimeRes_T *pGetSELTimeRes, int timeout)
{
	uint16	wRet;
	uint32	dwResLen;
	uint8	DummyReqData[8];
	uint8	NetFnLunStorage = (NETFN_STORAGE << 2);

	dwResLen = sizeof(GetSELTimeRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
						NetFnLunStorage, CMD_GET_SEL_TIME,
						(uint8*)DummyReqData, 0,
						(uint8 *)pGetSELTimeRes, &dwResLen,
						timeout);
	if (wRet != 0)
	{
		TCRIT("Error getting SEL Time::%x\n",wRet);
	}
	return wRet;
}

uint8 IPMICMD_SetSELTime(IPMI20_SESSION_T *pSession,
				SetSELTimeReq_T *pSetSELTimeReq, int timeout)
{
	uint16	wRet;
	uint32	dwResLen;
	uint8	DummyResData[8];
	uint8	NetFnLunStorage = (NETFN_STORAGE << 2);

	dwResLen = sizeof(INT8U);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
						NetFnLunStorage, CMD_SET_SEL_TIME,
						(uint8 *)pSetSELTimeReq, sizeof(SetSELTimeReq_T),
						(uint8 *)DummyResData, &dwResLen,
						timeout);
	if (wRet != 0)
	{
		TCRIT("Error setting SEL Time::%x\n",wRet);
	}
	return wRet;
}

uint8 IPMICMD_GetSELTimeUTCOffset(IPMI20_SESSION_T *pSession,
				GetSELTimeUTCOffsetRes_T *pGetSELTimeUTCOffsetRes, int timeout)
{
	uint16	wRet;
	uint32	dwResLen;
	uint8	DummyReqData[8];
	uint8	NetFnLunStorage = (NETFN_STORAGE << 2);

	dwResLen = sizeof(GetSELTimeUTCOffsetRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
						NetFnLunStorage, CMD_GET_SEL_TIME_UTC_OFFSET,
						(uint8*)DummyReqData, 0,
						(uint8 *)pGetSELTimeUTCOffsetRes, &dwResLen,
						timeout);
	if (wRet != 0)
	{
		TCRIT("Error getting SEL Time UTC Offset::%x\n",wRet);
	}
	return wRet;
}

uint8 IPMICMD_SetSELTimeUTCOffset(IPMI20_SESSION_T *pSession,
				SetSELTimeUTCOffsetReq_T *pSetSELTimeUTCOffsetReq, int timeout)
{
	uint16	wRet;
	uint32	dwResLen;
	SetSELTimeUTCOffsetRes_T pSetSELTimeUTCOffsetRes;
	uint8	NetFnLunStorage = (NETFN_STORAGE << 2);

	dwResLen = sizeof(SetSELTimeUTCOffsetRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
						NetFnLunStorage, CMD_SET_SEL_TIME_UTC_OFFSET,
						(uint8 *)pSetSELTimeUTCOffsetReq, sizeof(SetSELTimeUTCOffsetReq_T),
						(uint8 *)&pSetSELTimeUTCOffsetRes, &dwResLen,
						timeout);
	if (wRet != 0)
	{
		TCRIT("Error setting SEL Time UTC Offset::%x\n",wRet);
	}
	return wRet;
}
