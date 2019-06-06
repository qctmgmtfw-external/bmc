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
* Filename: libipmi_PEF.c
*
******************************************************************/
#include "libipmi_session.h"
#include "libipmi_PEF.h"
#include "libipmi_StorDevice.h"			//For libipmi_GetBits

#include <stdlib.h>
#include <string.h>
#include "dbgout.h"
#include "std_macros.h"

#ifndef ALERT_POLICY_PER_CHANNEL
#define ALERT_POLICY_PER_CHANNEL   15
#endif

/* Get PEF Capabilities Command */
uint16	IPMICMD_GetPEFCapabilities( IPMI20_SESSION_T *pSession,
					GetPEFCapRes_T *pResGetPEFCap,
					int timeout)
{
	uint8	Req[20];
	uint16	wRet;
	uint32	dwResLen;


	dwResLen = sizeof(GetPEFCapRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
						NETFNLUN_IPMI_SENSOR, CMD_GET_PEF_CAPABILITIES,
						(uint8*)Req, 0,
						(uint8 *)pResGetPEFCap, &dwResLen,
						timeout);

	return wRet;
}

/* Arm PEF Postpone Timer Command */
uint16	IPMICMD_ArmPEFTimer( IPMI20_SESSION_T *pSession/*in*/,
					uint8*	pReqTmrTimeout/*in*/,
					ArmPEFTmrRes_T *pResArmPEFTmr/*out*/,
					int timeout/*in*/)
{
	uint16	wRet;
	uint32	dwResLen;


	dwResLen = sizeof(ArmPEFTmrRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
						NETFNLUN_IPMI_SENSOR, CMD_ARM_PEF_POSTPONE_TIMER,
						pReqTmrTimeout, sizeof(uint8),
						(uint8 *)pResArmPEFTmr, &dwResLen,
						timeout);

	return wRet;
}

/* Set PEF Configuration Parameters Command */
uint16	IPMICMD_SetPEFConf( IPMI20_SESSION_T *pSession/*in*/,
					uint8 *pReqSetPEFConf/*in*/,
					uint32 dwReqBufLen,
					uint8 *pResSetPEFConf/*out*/,
					int timeout/*in*/)
{
	uint16	wRet;
	uint32	dwResLen;


	dwResLen = sizeof(uint8);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
						NETFNLUN_IPMI_SENSOR, CMD_SET_PEF_CONFIG_PARAMS,
						pReqSetPEFConf, dwReqBufLen,
						(uint8 *)pResSetPEFConf, &dwResLen,
						timeout);

	return wRet;
}

/* Get PEF Configuration Parameters Command */
uint16	IPMICMD_GetPEFConf( IPMI20_SESSION_T *pSession/*in*/,
					GetPEFConfigReq_T *pReqGetPEFConf/*in*/,
					uint8 *pResGetPEFConf/*out*/,
					int timeout/*in*/)
{
	uint16	wRet;
	uint32	dwResLen;


	dwResLen = MAX_RESPONSE_SIZE;
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
						NETFNLUN_IPMI_SENSOR, CMD_GET_PEF_CONFIG_PARAMS,
						(uint8*)pReqGetPEFConf, sizeof(GetPEFConfigReq_T),
						pResGetPEFConf, &dwResLen,
						timeout);

	return wRet;
}

/* Set Last Processed Event ID Command */
uint16	IPMICMD_SetLastEventID( IPMI20_SESSION_T *pSession/*in*/,
					SetLastEvtIDReq_T *pReqSetLastEvt/*in*/,
					uint8 *pResSetLastEvt/*out*/,
					int timeout/*in*/)
{
	uint16	wRet;
	uint32	dwResLen;


	dwResLen = sizeof(uint8);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
						NETFNLUN_IPMI_SENSOR, CMD_SET_LAST_PROCESSED_EVENT_ID,
						(uint8*)pReqSetLastEvt, sizeof(SetLastEvtIDReq_T),
						pResSetLastEvt, &dwResLen,
						timeout);

	return wRet;
}

/* Get Last Processed Event ID Command */
uint16	IPMICMD_GetLastEventID( IPMI20_SESSION_T *pSession/*in*/,
						GetLastEvtIDRes_T *pResGetLastEvt/*out*/,
						int timeout/*in*/)
{
	uint8	Req[20];
	uint16	wRet;
	uint32	dwResLen;


	dwResLen = sizeof(GetLastEvtIDRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
						NETFNLUN_IPMI_SENSOR, CMD_GET_LAST_PROCESSED_EVENT_ID,
						(uint8*)Req, 0,
						(uint8*)pResGetLastEvt, &dwResLen,
						timeout);

	return wRet;
}

/* Alert Immediate Command */
uint16	IPMICMD_AlertImmediate( IPMI20_SESSION_T *pSession/*in*/,
						AlertImmReq_T *pReqAlertImmediate/*in*/,
						AlertImmRes_T *pResAlertImmediate/*out*/,
						int timeout/*in*/)
{
	uint16	wRet;
	uint32	dwResLen;
	uint32	dwReqLen = 0x03;


	dwResLen = sizeof(AlertImmRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
						NETFNLUN_IPMI_SENSOR, CMD_ALERT_IMMEDIATE,
						(uint8*)pReqAlertImmediate, dwReqLen,
						(uint8*)pResAlertImmediate, &dwResLen,
						timeout);

	return wRet;
}

/* PET Acknowledge Command */
uint16	IPMICMD_PETAck( IPMI20_SESSION_T *pSession/*in*/,
						PETAckReq_T *pReqPETAck /*out*/,
						uint8 *pResPETACK /*out*/,
						int timeout/*in*/)
{
	uint16	wRet;
	uint32	dwResLen;

	dwResLen = sizeof(uint8);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
						NETFNLUN_IPMI_SENSOR, CMD_PET_ACKNOWLEDGE,
						(uint8*)pReqPETAck, sizeof(PETAckReq_T),
						(uint8*)pResPETACK, &dwResLen,
						timeout);

	return wRet;
}

/******************************************************************************
*****************************Higher level commands*****************************
******************************************************************************/


/*********** Alert Table **********/
uint16 LIBIPMI_HL_GetNumOfAlertPolicyEntries(IPMI20_SESSION_T *pSession,
					INT8U* pNumOfAlertPolicyEntries,
					int timeout)
{
	uint16 wRet = 0;
	uint8  PEFConf[MAX_RESPONSE_SIZE];
	GetPEFConfigReq_T GetPEFConfigReq;
	uint8 num = 0;

	memset(&GetPEFConfigReq,0x00,sizeof(GetPEFConfigReq_T));
	GetPEFConfigReq.ParamSel = 0x08;

	wRet = IPMICMD_GetPEFConf(pSession,&GetPEFConfigReq,PEFConf,timeout);
	if(wRet != 0)
	{
		TCRIT("LIBIPMI_HL_GetNumOfAlertPolicyEntries: Error getting PEF Conf\n");
		return wRet;
	}
	num = *(PEFConf + sizeof(GetPEFConfigRes_T));
	*pNumOfAlertPolicyEntries = GetBits(num,6,0);

	//TCRIT("Number of Alert Policy entries : %d\n",*pNumOfAlertPolicyEntries);

	return wRet;
}

uint16 LIBIPMI_HL_GetAnAlertEntry(IPMI20_SESSION_T *pSession,
					INT8U AlertEntryNumber,
					AlertPolicyTblEntry_T* pAlertEntry,
					int timeout)
{
	uint16 wRet = 0;
	uint8  PEFConf[MAX_RESPONSE_SIZE];
	GetPEFConfigReq_T GetPEFConfigReq;

	memset(&GetPEFConfigReq,0x00,sizeof(GetPEFConfigReq_T));
	GetPEFConfigReq.ParamSel = 0x09;
	GetPEFConfigReq.SetSel = AlertEntryNumber;

	wRet = IPMICMD_GetPEFConf(pSession,&GetPEFConfigReq,PEFConf,timeout);
	if(wRet != 0)
	{
		TCRIT("LIBIPMI_HL_GetAnAlertEntry: Error getting PEF Conf\n");
		return wRet;
	}
	memcpy(pAlertEntry,(PEFConf + sizeof(GetPEFConfigRes_T) + 1),sizeof(AlertPolicyTblEntry_T));

	return wRet;
}

uint16 LIBIPMI_HL_EnDisPolicyTableEntry(IPMI20_SESSION_T *pSession,INT8U IndexNum, int AlertNumEnable, INT8U DestIndex,
			INT8U Channel, int timeout)
{
	uint16 wRet = 0;
	AlertPolicyTblEntry_T AlertEntry;

	wRet = LIBIPMI_HL_GetAnAlertEntry(pSession,IndexNum,&AlertEntry,timeout);
	if(wRet != 0)
	{
		TCRIT("Critical error trying to Enable Disable alert policy - getting for index %d\n",IndexNum);
		return wRet;
	}
	AlertEntry.AlertNum=AlertNumEnable;
	//AlertEntry.ChannelDestSel |= DestIndex;
	//AlertEntry.ChannelDestSel = (Channel << 4) | DestIndex;
	AlertEntry.ChannelDestSel = (Channel << 4) | ((DestIndex-1)%(ALERT_POLICY_PER_CHANNEL)+1);

	wRet = LIBIPMI_HL_SetAnAlertEntry(pSession,IndexNum,&AlertEntry,timeout);
	if(wRet != 0)
	{
		TCRIT("Critical error trying to Enable Disable alert policy - setting\n");
		return wRet;
	}

	return wRet;
}

uint16 LIBIPMI_HL_SetAnAlertEntry(IPMI20_SESSION_T *pSession,
					INT8U AlertEntryNumber,
					AlertPolicyTblEntry_T* pAlertEntry,
					int timeout)
{
	uint16 wRet = 0;
	uint8 SetPEFConfigReq[MAX_RESPONSE_SIZE];
	uint32 reqLen = 0;
	uint8 response = 0;

	memset(SetPEFConfigReq,0x00,MAX_RESPONSE_SIZE);
	SetPEFConfigReq[0] = 0x09;
	SetPEFConfigReq[1] = AlertEntryNumber;

	memcpy(&SetPEFConfigReq[2],pAlertEntry,sizeof(AlertPolicyTblEntry_T));
	reqLen = 2 + sizeof(AlertPolicyTblEntry_T);

	wRet = IPMICMD_SetPEFConf(pSession,SetPEFConfigReq,reqLen,&response,timeout);
	if(wRet != 0)
	{
		TCRIT("LIBIPMI_HL_SetAnAlertEntry: Error setting PEF Conf\n");
	}
	return wRet;
}

uint16 LIBIPMI_HL_GetAlertTable(IPMI20_SESSION_T *pSession,
					AlertPolicyTblEntry_T* pAlertTable,
					INT8U* pNumOfAlertEntries,
					int timeout)
{
	uint16 wRet = 0;
	uint8 i,nAlertEntries = 0;
	AlertPolicyTblEntry_T AlertEntry;

	wRet = LIBIPMI_HL_GetNumOfAlertPolicyEntries(pSession,&nAlertEntries,timeout);
	if(wRet != 0)
	{
		TCRIT("LIBIPMI_HL_GetAlertTable: Error getting number of Alert policy entries\n");
		return wRet;
	}

	*pNumOfAlertEntries = nAlertEntries;

	for(i = 1; i <= nAlertEntries; i++)
	{
		wRet = LIBIPMI_HL_GetAnAlertEntry(pSession,(INT8U)i,&AlertEntry,timeout);
		if(wRet != 0)
		{
			TCRIT("LIBIPMI_HL_GetAlertTable: Error getting an Alert Entry\n");
			return wRet;
		}
		/* Copy this entry to pAlertTable */
		memcpy(pAlertTable,&AlertEntry,sizeof(AlertPolicyTblEntry_T));
		pAlertTable++;
	}
	return wRet;
}


/**************
IPMI makes it fiendishly difficult to organize alerts.
This is our mechanism
You can choose to get Information and Above, Warning and above, Critical and Above or Non Recoverable and above alerts

There are predefined Event Filter Table entries with All these categories
Information,Warning, Critical, Nonrecoverable with Slert actions. (Other actions need to bbe configurd by the OEM)

So when we choose a IP and select one of the levels above,
We go to the PolicyTable (this is another table)
There we have 4 entries per index of the destination.

So Let us say you define an IP a.b.c.d for 1 and choose critical and above
We go to the policytable and locate the 4 entries for index 1 (0 -3 )
0 will have a predetremined policy number corresponding to Information, 1 will have a policy number corresponding to Warning etc
So we enable index 2 and 3 since we want critical and above.

When an event occurs the PEF code, looks up warning alert action sees policy number x and since we have set the destination in the
policy table alerts will go out.

*******************/
#define NUM_ALERT_CATEGORIES 4 //we have 4 possible categories
#define ALERT_LEVEL_DISBALE		0
#define ALERT_LEVEL_INFO_AND_ABOVE     	1
#define ALERT_LEVEL_WARN_AND_ABOVE     	1
#define ALERT_LEVEL_CRIT_AND_ABOVE     	1
#define ALERT_LEVEL__AND_ABOVE     	1

uint16 LIBIPMI_HL_SetGraspAlertLevel(IPMI20_SESSION_T *pSession,
					INT8U IndexNum,
					INT8U AlertLevel, //NOTE: we use our alert levels and the input AlertNum is the structure arg is not used
					INT8U Channel,
					int timeout)
{
	//alert level can be info and above, warning and above,Critical and above,nonrecoverable and above
	//we use 0-disable,1-info and above,2-warning and above,3-crit and above,4-nonrec and above

	uint16 wRet = 0;
	int loopindex = 0,i=0;
	int AlertNumEnable = 0;
	int AlertEntryIndex = IndexNum * NUM_ALERT_CATEGORIES;


	for(i=AlertEntryIndex-NUM_ALERT_CATEGORIES +1,loopindex = 1;i <= (AlertEntryIndex );i++,loopindex++)
	{
		//we assume ordering of policy entries
		//Information, Warning, Critical and Non Recoverable - 0 is info etc..
		//so if our alert level is say 1, which means information and above, we have to set all policies
		//for our destination to enable
		//example - let us say we get warning and above which is 3
		//looping through we see that for the information case we want to disable.Since 3 is less than 1...
		//if alert level is 0 which is disable we never enable it.
		//printf("LIBIPMI_HL_SetGraspAlertLevel:  AlertLevel selected is : %d\n", AlertLevel);
		//if((loopindex <=  AlertLevel ) && (AlertLevel != 0))
		if((loopindex >=  AlertLevel ) && (AlertLevel != 0))
			AlertNumEnable = ((loopindex<< 0x4) | 0x08);
		else
			AlertNumEnable = ((loopindex << 0x4) & 0xF7);


			wRet = LIBIPMI_HL_EnDisPolicyTableEntry(pSession,(INT8U)i,AlertNumEnable,IndexNum,Channel,timeout);
			if(wRet != 0)
			{
				TCRIT("Error while enabling, disabling Policy Table entry\n");
				return wRet;
			}
	}

	return 0;
}


//this will just return the alert level that has been set for a prticular destination
uint16 LIBIPMI_HL_GetGraspAlertLevel(IPMI20_SESSION_T *pSession,
					INT8U IndexNum,
					INT8U* AlertLevel, //NOTE: we use our alert levels and the input AlertNum is the structure arg is not used
					INT8U* DestIndex,
					int timeout)
{
	//alert level can be info and above, warning and above,Critical and above,nonrecoverable and above
	//we use 0-disable,1-info and above,2-warning and above,3-crit and above,4-nonrec and above

	uint16 wRet = 0;
	AlertPolicyTblEntry_T AlertEntry;
	int AlertEntryIndex = (IndexNum) * NUM_ALERT_CATEGORIES;
	int i = 0,loopindex=0;

	int tmpAlertLevel = 0;

	for(i=(AlertEntryIndex - NUM_ALERT_CATEGORIES +1),loopindex = 1;i <= (AlertEntryIndex);i++,loopindex++)
	{

		wRet = LIBIPMI_HL_GetAnAlertEntry(pSession,(INT8U)i,&AlertEntry,timeout);
		if(wRet != 0)
		{
			TCRIT("Critical error trying to get alert policy for index num %d\n",i);
			return wRet;
		}

		//now we check the alert entry and arrive at the alert level number;
		//if this entry is disabled
		 //if( AlertEntry.AlertNum & 0x08 )
		 if( (AlertEntry.AlertNum & 0x08) != 0)
		 {
		 	tmpAlertLevel = loopindex;
			break;
		 }
	}

	*AlertLevel = tmpAlertLevel;
	*DestIndex = AlertEntry.ChannelDestSel;
	return wRet;
}



/************* PEF Table **************/
uint16 LIBIPMI_HL_GetNumOfPEFEntries(IPMI20_SESSION_T *pSession,
					INT8U* pNumOfPEFEntries,
					int timeout)
{
	uint16 wRet = 0;
	uint8  PEFConf[MAX_RESPONSE_SIZE];
	GetPEFConfigReq_T GetPEFConfigReq;
	uint8 num = 0;

	memset(&GetPEFConfigReq,0x00,sizeof(GetPEFConfigReq_T));
	GetPEFConfigReq.ParamSel = 0x05;

	wRet = IPMICMD_GetPEFConf(pSession,&GetPEFConfigReq,PEFConf,timeout);
	if(wRet != 0)
	{
		TCRIT("LIBIPMI_HL_GetNumOfPEFEntries: Error getting PEF Conf\n");
		return wRet;
	}
	num = *(PEFConf + sizeof(GetPEFConfigRes_T));
	*pNumOfPEFEntries = GetBits(num,6,0);

	//TCRIT("Number of PEF entries : %d\n",*pNumOfPEFEntries);

	return wRet;
}

uint16 LIBIPMI_HL_SetPEFEntry(IPMI20_SESSION_T *pSession,
					INT8U PEFEntryNumber,
					EvtFilterTblEntry_T* pPEFEntry,
					int timeout)
{
	uint16 wRet = 0;
	uint8 SetPEFConfigReq[MAX_RESPONSE_SIZE];
	uint32 reqLen = 0;
	uint8 response = 0;

	memset(SetPEFConfigReq,0x00,MAX_RESPONSE_SIZE);
	SetPEFConfigReq[0] = 0x06;
	SetPEFConfigReq[1] = PEFEntryNumber;

	memcpy(&SetPEFConfigReq[2],pPEFEntry,sizeof(EvtFilterTblEntry_T));
	reqLen = 2 + sizeof(EvtFilterTblEntry_T);

	wRet = IPMICMD_SetPEFConf(pSession,SetPEFConfigReq,reqLen,&response,timeout);
	if(wRet != 0)
	{
		TCRIT("LIBIPMI_HL_SetPEFEntry: Error setting PEF Conf\n");
	}
	return wRet;
}

uint16 LIBIPMI_HL_GetPEFEntry(IPMI20_SESSION_T *pSession,
					INT8U PEFEntryNumber,
					EvtFilterTblEntry_T* pPEFEntry,
					int timeout)
{
	uint16 temp, wRet = 0;
	uint8  PEFConf[MAX_RESPONSE_SIZE];
	GetPEFConfigReq_T GetPEFConfigReq;

	memset(&GetPEFConfigReq,0x00,sizeof(GetPEFConfigReq_T));
	GetPEFConfigReq.ParamSel = 0x06;
	GetPEFConfigReq.SetSel = PEFEntryNumber;

	wRet = IPMICMD_GetPEFConf(pSession,&GetPEFConfigReq,PEFConf,timeout);
	if(wRet != 0)
	{
		TCRIT("LIBIPMI_HL_GetPEFEntry: Error getting PEF Conf\n");
		return wRet;
	}
	memcpy(pPEFEntry,(PEFConf + sizeof(GetPEFConfigRes_T) + 1),sizeof(EvtFilterTblEntry_T));

	temp = ipmitoh_u16(pPEFEntry->EventData1OffsetMask);
	pPEFEntry->EventData1OffsetMask = temp;

	return wRet;
}

uint16 LIBIPMI_HL_GetPEFTable(IPMI20_SESSION_T *pSession,
					EvtFilterTblEntry_T* pPEFTable,
					INT8U* pNumOfPEFEntries,
					int timeout)
{
	uint16 wRet = 0;
	uint8 i,nPEFEntries = 0;
	EvtFilterTblEntry_T PEFEntry;

	wRet = LIBIPMI_HL_GetNumOfPEFEntries(pSession,&nPEFEntries,timeout);
	if(wRet != 0)
	{
		TCRIT("LIBIPMI_HL_GetPEFTable: Error getting number of PEF entries\n");
		return wRet;
	}

	*pNumOfPEFEntries = nPEFEntries;

	for(i = 1; i <= nPEFEntries; i++)
	{
		wRet = LIBIPMI_HL_GetPEFEntry(pSession,i,&PEFEntry,timeout);
		if(wRet != 0)
		{
			TCRIT("LIBIPMI_HL_GetPEFTable: Error getting a PEF Entry\n");
			return wRet;
		}
		/* Copy this entry to pPEFTable */
		memcpy(pPEFTable,&PEFEntry,sizeof(EvtFilterTblEntry_T));
		pPEFTable++;
	}
	return wRet;
}
