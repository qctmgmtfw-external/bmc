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
* Filename: libipmi_XportDevice.c
*
* Description: Contains implementation of NetFn Transport
*   specific IPMI command functions
*
* Author: Anurag Bhatia
*
******************************************************************/
#include "libipmi_session.h"
#include "libipmi_errorcodes.h"
#include "libipmi_XportDevice.h"
#include "libipmi_AppDevice.h"
#include "std_macros.h"
#include "LANConfig.h"
#include "IPMI_AMIConf.h"
#include "IPMI_LANConfig.h"

#include <stdlib.h>
#include <string.h>
#include "dbgout.h"
#include <ctype.h>

extern INT8U GetLanOEMParamValue (INT8U ParamSelect);

/* Test */
/*****************************************************************************
	LAN Configuration related IPMI Command Interface
******************************************************************************/
uint16 IPMICMD_GetLANConfig(IPMI20_SESSION_T *pSession,
				GetLanConfigReq_T* pGetLANConfigReqData,
				GetLanConfigRes_T* pGetLANConfigResData,
				int timeout)
{
	uint16 wRet = 0;
	uint8 NetFnLunXport = (NETFN_TRANSPORT << 2);
	uint32	dwResLen;
	dwResLen = sizeof(GetLanConfigRes_T);

	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
						NetFnLunXport, CMD_GET_LAN_CONFIGURATION_PARAMETERS,
						(uint8*)pGetLANConfigReqData, sizeof(GetLanConfigReq_T),
						(uint8 *)pGetLANConfigResData, &dwResLen,
						timeout);
	return wRet;
}

uint16 IPMICMD_SetLANConfig(IPMI20_SESSION_T *pSession,
				SetLanConfigReq_T* pSetLANConfigReqData,
				unsigned int ReqDataLen,
				SetLanConfigRes_T* pSetLANConfigResData,
				int timeout)
{
	uint16 wRet = 0;
	uint8 NetFnLunXport = (NETFN_TRANSPORT << 2);
	uint32	dwResLen;
	dwResLen = sizeof(SetLanConfigRes_T);

	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
						NetFnLunXport, CMD_SET_LAN_CONFIGURATION_PARAMETERS,
						(uint8*)pSetLANConfigReqData, ReqDataLen,
						(uint8 *)pSetLANConfigResData, &dwResLen,
						timeout);
	return wRet;
}


/**************************************************************************************
*********************************Higher level commands*********************************
***************************************************************************************/
uint16 LIBIPMI_HL_GetNumOfLANDestinationEntries(IPMI20_SESSION_T *pSession,
					INT8U* pNumOfLANDestEntries,
					uint8 Channel,
					int timeout)
{
	uint16 wRet = 0;
	GetLanConfigReq_T GetLANReqBuff;
	GetLanConfigRes_T GetLANResBuff;

	GetLANReqBuff.ChannelNum = Channel;
	GetLANReqBuff.ParameterSelect = LAN_PARAM_DEST_NUM;
	GetLANReqBuff.SetSelect = 0;
	GetLANReqBuff.BlockSelect = 0;

	wRet = IPMICMD_GetLANConfig(pSession, &GetLANReqBuff,
					&GetLANResBuff, timeout);
	if(wRet != 0)
	{
		TCRIT("LIBIPMI_HL_GetNumOfLANDestinationEntries, Error getting LAN configuration\n");
		return wRet;
	}

	TDBG ("Number of Destinations::%d\n", GetLANResBuff.ConfigData.NumDest);
	*pNumOfLANDestEntries = GetLANResBuff.ConfigData.NumDest;
	return wRet;
}

uint16 LIBIPMI_HL_GetLANDestinationType(IPMI20_SESSION_T *pSession,
					uint8 Channel,
					uint8  index,
					LANDestType_T *LANDestTypeRes,
					int timeout)
{
	uint16 wRet = 0;
	GetLanConfigReq_T GetLANReqBuff;
	GetLanConfigRes_T GetLANResBuff;

	GetLANReqBuff.ChannelNum = Channel;
	GetLANReqBuff.ParameterSelect = LAN_PARAM_SELECT_DEST_TYPE;
	GetLANReqBuff.SetSelect = index;
	GetLANReqBuff.BlockSelect = 0;

	wRet = IPMICMD_GetLANConfig(pSession, &GetLANReqBuff,
					&GetLANResBuff, timeout);
	if(wRet != 0)
	{
		TCRIT("LIBIPMI_HL_GetLANDestinationType, Error getting LAN configuration\n");
		return wRet;
	}

	TDBG("Success getting LAN Destination type for index::%d\n",index);
	memcpy(LANDestTypeRes, &(GetLANResBuff.ConfigData.DestType), sizeof(LANDestType_T));
	return wRet;
}

uint16
	LIBIPMI_HL_GetLANDestinationAddress(IPMI20_SESSION_T *pSession,
					uint8 Channel,
					uint8  index,
					LANDestAddr_T *LANDestAddrRes,
					int timeout)
{
	uint16 wRet = 0;
	GetLanConfigReq_T GetLANReqBuff;
	GetLanConfigRes_T GetLANResBuff;

	GetLANReqBuff.ChannelNum = Channel;
	GetLANReqBuff.ParameterSelect = LAN_PARAM_SELECT_DEST_ADDR;
	GetLANReqBuff.SetSelect = index;
	GetLANReqBuff.BlockSelect = 0;

	wRet = IPMICMD_GetLANConfig(pSession, &GetLANReqBuff,
					&GetLANResBuff, timeout);
	if(wRet != 0)
	{
		TCRIT("LIBIPMI_HL_GetLANDestinationAddress, Error getting LAN configuration\n");
		return wRet;
	}

	TDBG("Success getting LAN Destination address for index::%d\n",index);
	memcpy(LANDestAddrRes, &(GetLANResBuff.ConfigData.DestAddr), sizeof(LANDestAddr_T));
	return wRet;
}

uint16 LIBIPMI_HL_GetLANDestinationV6Address(IPMI20_SESSION_T *pSession,
					uint8 Channel,
					uint8  index,
					LANDestv6Addr_T *LANDestv6AddrRes,
					int timeout)
{
	uint16 wRet = 0;
	GetLanConfigReq_T GetLANReqBuff;
	GetLanConfigRes_T GetLANResBuff;

	GetLANReqBuff.ChannelNum = Channel;
	GetLANReqBuff.ParameterSelect = GetLanOEMParamValue (LAN_PARAMS_AMI_OEM_SNMPV6_DEST_ADDR);
	GetLANReqBuff.SetSelect = index;
	GetLANReqBuff.BlockSelect = 0;

	wRet = IPMICMD_GetLANConfig(pSession, &GetLANReqBuff,
					&GetLANResBuff, timeout);
	if(wRet != 0)
	{
		TCRIT("LIBIPMI_HL_GetLANDestinationAddress, Error getting LAN configuration\n");
		return wRet;
	}

	TDBG("Success getting LAN Destination V6 address for index::%d\n", index);
	memcpy(LANDestv6AddrRes, &(GetLANResBuff.ConfigData.Destv6Addr), sizeof(LANDestv6Addr_T));
	return wRet;
}

uint16 LIBIPMI_HL_GetLANDestinationType_AddrTable(IPMI20_SESSION_T *pSession,
					LANDestType_T* pDestTypeTable,
					LANDestAddr_T* pDestAddrTable,
					uint8* pNumOfLANDestEntries,
					uint8 Channel,
					int timeout)
{
	uint16 wRet = 0;
	uint8 i,nLANDestEntries = 0;
	LANDestType_T LANDestTypeRes;
	LANDestAddr_T LANDestAddrRes;

	wRet = LIBIPMI_HL_GetNumOfLANDestinationEntries(pSession, &nLANDestEntries,
												Channel, timeout);
	if(wRet != 0)
	{
		TCRIT("LIBIPMI_HL_GetLANDestinationType_AddrTable: Error getting number of LAN Destination entries\n");
		return wRet;
	}

	*pNumOfLANDestEntries = nLANDestEntries;
	for(i = 1; i <= nLANDestEntries; i++)
	{
		/*Getting LAN Destination Type*/
		memset(&LANDestTypeRes, 0, sizeof(LANDestType_T));
		wRet = LIBIPMI_HL_GetLANDestinationType(pSession, Channel, (INT8U)i,  &LANDestTypeRes, timeout);
		if(wRet != 0)
		{
			TCRIT("LIBIPMI_HL_GetLANDestinationType_AddrTable: Error getting an Destination Type Entry\n");
			return wRet;
		}
		memcpy(pDestTypeTable,&LANDestTypeRes,sizeof(LANDestType_T));
		pDestTypeTable++;

		/*Getting LAN Destination Address*/
		memset(&LANDestAddrRes, 0, sizeof(LANDestAddr_T));
		wRet = LIBIPMI_HL_GetLANDestinationAddress(pSession, Channel, (INT8U)i, &LANDestAddrRes, timeout);
		if(wRet != 0)
		{
			TCRIT("LIBIPMI_HL_GetLANDestinationType_AddrTable: Error getting an Destination Address Entry\n");
			return wRet;
		}
		memcpy(pDestAddrTable,&LANDestAddrRes,sizeof(LANDestAddr_T));
		pDestAddrTable++;
	}
	return wRet;
}

/*
 *@fn LIBIPMI_HL_GetNumOfLANInterfaces
 *@brief This function gets the total number of network interfaces present in the system
 *@param ifname - Interface Name
 *@returns 0 for MDIO, 1 for NCSI and -1 on error
*/
uint16 LIBIPMI_HL_GetNumOfLANInterfaces(IPMI20_SESSION_T *pSession, INT8U* pLanInfo,
					INT8U* pNumOfLANInterfaces, int timeout)
{
	uint16 wRet = 0;
	int index = 1;
	GetChInfoReq_T reqGetChInfo = {0};
	GetChInfoRes_T resGetChInfo = {0,0,0,0,0,"0","0"};
	int count = 0;

	if(0)
	{
		timeout=timeout; /*-Wextra: Flag added for strict compilation.*/
	}

	for(; index <= 15; index++)
	{
		memset(&resGetChInfo, 0, sizeof(GetChInfoRes_T));
		reqGetChInfo.ChannelNum = index;
		wRet = IPMICMD_GetChannelInfo(pSession, &reqGetChInfo, &resGetChInfo, DEFAULT_TIMEOUT);
		if(wRet != 0)
		{
			TDBG("LIBIPMI_HL_GetNumOfLANInterfaces: Error getting channel info in FindNetworkChannel for channel %d\n",index);
			// Make wRet as 0 because if channel 15 fails then the function return error.
			wRet = 0;
			continue;
		}
		else
		{
			/* To filter the LAN channels*/
			if(resGetChInfo.ChannelMedium == CHANNEL_MEDIUM_TYPE_LAN)
			{
				pLanInfo[count++] = index;
			}
		}
	}
	*pNumOfLANInterfaces = count;
	return wRet;
}
/*
 *@fn LIBIPMI_HL_GetNCSIConfigNUM
 *@brief This function gets the NCSIConfig Port number
 *@param[in] - IPMI session header,
 *@param[in] - Channel number,
 *@param[out] - ConfigNum NCSI Port number,
 *@param[in] - Session Timeout value
 *@returns 0 for Success; Others for specific error codes
*/
uint16 LIBIPMI_HL_GetNCSIConfigNUM(IPMI20_SESSION_T *pSession, INT8U Channel,
                                                                            INT8U *ConfigNum, int timeout)
{
    uint16 wRet = 0;
    GetLanConfigReq_T getLANReqBuff;
    GetLanConfigRes_T getLANResBuff;

    getLANReqBuff.ChannelNum = Channel;
    getLANReqBuff.ParameterSelect = GetLanOEMParamValue (LAN_PARAMS_AMI_OEM_NCSI_CONFIG_NUM);
    getLANReqBuff.SetSelect = 0;
    getLANReqBuff.BlockSelect = 0;

    wRet = IPMICMD_GetLANConfig(pSession, &getLANReqBuff, &getLANResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("LIBIPMI_HL_GetNCSIConfigNUM: Error getting LAN configuration for channel %d\n", Channel);
        return wRet;
    }

    TDBG("Success getting NCSIConfigNUM Configuration settings for channel: %d\n", Channel);
    *ConfigNum = getLANResBuff.ConfigData.NumNCSIPortConfigs;

    return wRet;
}
/*
 *@fn LIBIPMI_HL_GetNCSISetting
 *@brief This function gets the NCSI Settings for the given Index.
 *@param[in] - IPMI session header,
 *@param[in] - Channel number,
 *@param[out] - Index Configuration structure,
 *@param[out] - ChannelID - NCSI channel ID.
 *@param[out] - PackageId - NCSI package ID.
 *@param[in] - Session Timeout value
 *@returns 0 for Success; Others for specific error codes
*/

uint16 LIBIPMI_HL_GetNCSISettingByPortNum(IPMI20_SESSION_T *pSession, INT8U Channel, INT8U Port,
                                                                    INT8U *Index,INT8U *ChannelID,INT8U *PackageID, int timeout)
{
    uint16 wRet = 0;
    GetLanConfigReq_T getLANReqBuff;
    GetLanConfigRes_T getLANResBuff;

    getLANReqBuff.ChannelNum = Channel;
    getLANReqBuff.ParameterSelect = GetLanOEMParamValue (LAN_PARAMS_AMI_OEM_NCSI_SETTINGS_BY_PORT_NUM);
    getLANReqBuff.SetSelect = Port;
    getLANReqBuff.BlockSelect = 0;

    wRet = IPMICMD_GetLANConfig(pSession, &getLANReqBuff, &getLANResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("LIBIPMI_HL_GetNCSISetting: Error getting LAN configuration for channel %d\n", Channel);
        return wRet;
    }

    TDBG("Success getting NCSI Configuration settings for channel: %d\n", Channel);
    *Index = getLANResBuff.ConfigData.NCSICfg.InterfaceIndex;
    *ChannelID = getLANResBuff.ConfigData.NCSICfg.ChannelId;
    *PackageID = getLANResBuff.ConfigData.NCSICfg.PackageId;

    return wRet;
}

/*
 *@fn LIBIPMI_HL_SetNCSISetting
 *@brief This function gets the NCSI Settings for the given Index.
 *@param[in] - IPMI session header,
 *@param[in] - Channel number,
 *@param[in] - Index Configuration structure,
 *@param[in] - ChannelID - NCSI channel ID.
 *@param[in] - PackageId - NCSI package ID.
 *@param[in] - Session Timeout value
 *@returns 0 for Success; Others for specific error codes
*/
uint16 LIBIPMI_HL_SetNCSISetting(IPMI20_SESSION_T *pSession, INT8U Channel, INT8U Index,
                                                                   INT8U ChannelID, INT8U PackageID, int timeout)
{
    uint16 wRet = 0;
    SetLanConfigReq_T setLANReqBuff;
    SetLanConfigRes_T setLANResBuff;
    unsigned int ReqDataLen = 0;

    memset(&(setLANReqBuff), 0, sizeof(SetLanConfigReq_T));
    setLANReqBuff.ChannelNum = Channel;
    setLANReqBuff.ParameterSelect = GetLanOEMParamValue (LAN_PARAMS_AMI_OEM_NCSI_SETTINGS);

    setLANReqBuff.ConfigData.NCSIPortConfig.Interface = Index;
    setLANReqBuff.ConfigData.NCSIPortConfig.ChannelId = ChannelID;
    setLANReqBuff.ConfigData.NCSIPortConfig.PackageId = PackageID;

    ReqDataLen = 5; // Channel Number + Parameter Selector + Configuration Parameter Data
    wRet = IPMICMD_SetLANConfig(pSession, &setLANReqBuff, ReqDataLen, &setLANResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("LIBIPMI_HL_SetNCSISetting: Error setting LAN configuration for channel %d\n", Channel);
        return wRet;
    }

    TDBG("Success setting NCSI configuration for channel::%d\n", Channel);
    return wRet;
}


/*
 *@fn LIBIPMI_HL_GetNCSIMode
 *@brief This function gets the NCSI Mode for the given Index.
 *@param[in] - IPMI session header,
 *@param[in] - Channel number,
 *@param[in] - Index Configuration structure,
 *@param[in] - Session Timeout value
 *@param[out] - NCSI Mode
 *@returns 0 for Success; Others for specific error codes
*/

uint16 LIBIPMI_HL_GetNCSIMode(IPMI20_SESSION_T *pSession, INT8U Channel,
                                                                    INT8U Index,INT8U *AutoSelect, int timeout)
{
    uint16 wRet = 0;
    GetLanConfigReq_T getLANReqBuff;
    GetLanConfigRes_T getLANResBuff;

    getLANReqBuff.ChannelNum = Channel;
    getLANReqBuff.ParameterSelect = GetLanOEMParamValue (LAN_PARAMS_AMI_OEM_NCSI_MODE_CHANGE);
    getLANReqBuff.SetSelect = Index;
    getLANReqBuff.BlockSelect = 0;

    wRet = IPMICMD_GetLANConfig(pSession, &getLANReqBuff, &getLANResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("LIBIPMI_HL_GetNCSIMode: Error getting LAN configuration for channel %d\n", Channel);
        return wRet;
    }

    TDBG("Success getting NCSI Mode: %d\n", Channel);
    *AutoSelect = getLANResBuff.ConfigData.NCSIModeConfig.NCSIMode;

    return wRet;
}

/*
 *@fn LIBIPMI_HL_SetNCSIMode
 *@brief This function sets the NCSI Mode for the given Index.
 *@param[in] - IPMI session header,
 *@param[in] - Channel number,
 *@param[in] - Index Configuration structure,
 *@param[in] - NCSI Mode,
 *@param[in] - Session Timeout value
 *@returns 0 for Success; Others for specific error codes
*/
uint16 LIBIPMI_HL_SetNCSIMode(IPMI20_SESSION_T *pSession, INT8U Channel, INT8U Index,
                                                                   INT8U AutoSelect, int timeout)
{
    uint16 wRet = 0;
    SetLanConfigReq_T setLANReqBuff;
    SetLanConfigRes_T setLANResBuff;
    unsigned int ReqDataLen = 0;

    memset(&(setLANReqBuff), 0, sizeof(SetLanConfigReq_T));
    setLANReqBuff.ChannelNum = Channel;
    setLANReqBuff.ParameterSelect = GetLanOEMParamValue (LAN_PARAMS_AMI_OEM_NCSI_MODE_CHANGE);

    setLANReqBuff.ConfigData.NCSIModeConfig.Interface = Index;
    setLANReqBuff.ConfigData.NCSIModeConfig.NCSIMode = AutoSelect;

    ReqDataLen = 4; // Channel Number + Parameter Selector + Interfaced index + Mode
    wRet = IPMICMD_SetLANConfig(pSession, &setLANReqBuff, ReqDataLen, &setLANResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("LIBIPMI_HL_SetNCSIMode: Error setting LAN configuration for channel %d\n", Channel);
        return wRet;
    }

    TDBG("Success setting NCSI Mode::%d\n", Channel);
    return wRet;
}

/*
 *@fn LIBIPMI_HL_GetPHYConfig
 *@brief This function gets the network link modes for the given Channel number.
 *@param[in] - IPMI session header,
 *@param[in] - Channel number,
 *@param[out] - PHY Configuration structure,
 *@param[in] - Session Timeout value
 *@returns 0 for Success; Others for specific error codes
*/

uint16 LIBIPMI_HL_GetPHYConfig(IPMI20_SESSION_T *pSession, INT8U Channel,
     			       PHYConfig_T *PHYConfigRes, int timeout)
{
	uint16 wRet = 0;
	GetLanConfigReq_T getLANReqBuff;
	GetLanConfigRes_T getLANResBuff;

	getLANReqBuff.ChannelNum = Channel;
	getLANReqBuff.ParameterSelect = GetLanOEMParamValue (LAN_PARAMS_AMI_OEM_PHY_SETTINGS);
	getLANReqBuff.SetSelect = 0;
	getLANReqBuff.BlockSelect = 0;

	wRet = IPMICMD_GetLANConfig(pSession, &getLANReqBuff, &getLANResBuff, timeout);
	if(wRet != 0)
	{
		TCRIT("LIBIPMI_HL_GetPHYConfig: Error getting LAN configuration for channel %d\n", Channel);
		return wRet;
	}

	TDBG("Success getting PHY Configuration settings for channel: %d\n", Channel);
	memcpy(PHYConfigRes, &(getLANResBuff.ConfigData.PHYConfig), sizeof(PHYConfig_T));

	return wRet;
}

/*
 *@fn LIBIPMI_HL_GetPHYConfigTable
 *@brief This function gets the network link modes for all the channels present in the system.
 *@param[in] - IPMI session header,
 *@param[out] - PHY Configuration Table,
 *@param[out] - Total number of ethernet interfaces,
 *@param[in] - Session Timeout value
 *@returns 0 for Success; Others for specific error codes
*/

uint16 LIBIPMI_HL_GetPHYConfigTable(IPMI20_SESSION_T *pSession, PHYConfig_T* pPHYConfigTable,
 				    INT8U* pNumOfLANInterfaces, int timeout)
{
    INT8U wRet = 0;
    INT8U index = 0;
    INT8U BondEnable[2] = {0};
    INT8U LANChannels[MAX_LAN_CHANNEL] = {0};
    INT8U numOfLANInterfaces = 0, IfcCount = 0, Channel = 0;
    PHYConfig_T phyConfigRes;

    wRet = LIBIPMI_HL_IsBondEnabled(pSession, BondEnable,  timeout);
    if(wRet != 0)
    {
        TCRIT("LIBIPMI_HL_GetPHYConfigTable: Error getting number of LAN Interface entries\n");
        return wRet;
    }

    wRet = LIBIPMI_HL_GetLANCount( pSession, &numOfLANInterfaces, LANChannels, timeout);
    if(wRet !=0)
    {
        TCRIT("LIBIPMI_HL_GetPHYConfigTable: Error in getting LAN count\n");
        return wRet;
    }

    for(; index < numOfLANInterfaces; index++)
    {
        /*Skip if bond is enabled*/
        if(BondEnable[0] == 1)
        {
            if(BondEnable[1] == LANChannels[index])
            {
                continue;
            }
        }

        wRet =LIBIPMI_HL_GetChannelNum(pSession, LANChannels[index],&Channel, timeout);
        if(wRet !=0)
        {
            TCRIT("Error in Getting Channel number for Index :%d\n",LANChannels[index]);
            continue;
        }

        /*Getting PHY Configuration settings*/
        memset(&phyConfigRes, 0, sizeof(phyConfigRes));
        wRet = LIBIPMI_HL_GetPHYConfig(pSession, Channel, &phyConfigRes, timeout);
        if(wRet != 0)
        {
            TCRIT("LIBIPMI_HL_GetPHYConfigTable: Error getting PHY Configuration Entry for channel %d\n", LANChannels[index]);
            return wRet;
        }
        memcpy(pPHYConfigTable,&phyConfigRes,sizeof(PHYConfig_T));
        pPHYConfigTable++;
        IfcCount++;
    }

    *pNumOfLANInterfaces = IfcCount;
    return wRet;
}


uint16 LIBIPMI_HL_SetPHYConfig(IPMI20_SESSION_T *pSession, INT8U Channel, INT8U AutoNegotiation,
			       INT16U Speed, INT8U Duplex, int timeout)
{
	uint16 wRet = 0;
	SetLanConfigReq_T setLANReqBuff;
	SetLanConfigRes_T setLANResBuff;
	unsigned int ReqDataLen = 0;

	memset(&(setLANReqBuff), 0, sizeof(SetLanConfigReq_T));
	setLANReqBuff.ChannelNum = Channel;
	setLANReqBuff.ParameterSelect = GetLanOEMParamValue (LAN_PARAMS_AMI_OEM_PHY_SETTINGS);

	setLANReqBuff.ConfigData.PHYConfig.AutoNegotiationEnable = AutoNegotiation;
	if(AutoNegotiation == TRUE)
	{
		setLANReqBuff.ConfigData.PHYConfig.Speed = 0xFFFF;
		setLANReqBuff.ConfigData.PHYConfig.Duplex = 0xFF;
	}
	else
	{
	setLANReqBuff.ConfigData.PHYConfig.Speed = Speed;
	setLANReqBuff.ConfigData.PHYConfig.Duplex = Duplex;
	}

	ReqDataLen = 6; // Channel Number + Parameter Selector + Configuration Parameter Data
	wRet = IPMICMD_SetLANConfig(pSession, &setLANReqBuff, ReqDataLen, &setLANResBuff, timeout);
	if(wRet != 0)
	{
		TCRIT("LIBIPMI_HL_SetPHYConfig: Error setting LAN configuration for channel %d\n", Channel);
		return wRet;
	}

	TDBG("Success setting PHY configuration for channel::%d\n", Channel);
	return wRet;
}
uint16 LIBIPMI_HL_SetIPv4Source(IPMI20_SESSION_T *pSession, INT8U Source,INT8U Channel, int timeout)
{
    uint16 wRet = 0;
    SetLanConfigReq_T setLANReqBuff;
    SetLanConfigRes_T setLANResBuff;
    unsigned int ReqDataLen = 0;

    memset(&(setLANReqBuff), 0, sizeof(SetLanConfigReq_T));
    setLANReqBuff.ChannelNum = Channel;
    setLANReqBuff.ParameterSelect = LAN_PARAM_IP_ADDRESS_SOURCE;

    setLANReqBuff.ConfigData.IPAddrSrc= Source;

    ReqDataLen = 2+(sizeof(setLANReqBuff.ConfigData.IPAddrSrc));
    wRet = IPMICMD_SetLANConfig(pSession, &setLANReqBuff, ReqDataLen, &setLANResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("LIBIPMI_HL_SetIPv4Source: Error setting LAN configuration for channel %d\n", Channel);
        return wRet;
    }

    TDBG("Success setting IPv4 Address Source configuration for channel::%d\n", Channel);
    return wRet;
}

uint16 LIBIPMI_HL_GetIPv4Source(IPMI20_SESSION_T *pSession, INT8U *Source,INT8U Channel, int timeout)
{
    uint16 wRet = 0;
    GetLanConfigReq_T getLANReqBuff;
    GetLanConfigRes_T getLANResBuff;

    getLANReqBuff.ChannelNum = Channel;
    getLANReqBuff.ParameterSelect = LAN_PARAM_IP_ADDRESS_SOURCE;
    getLANReqBuff.SetSelect = 0;
    getLANReqBuff.BlockSelect = 0;

    wRet = IPMICMD_GetLANConfig(pSession, &getLANReqBuff, &getLANResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("LIBIPMI_HL_GetIPv4Source: Error getting LAN configuration for channel %d\n", Channel);
        return wRet;
    }

    TDBG("Success getting IPv4 Address Source Configuration settings for channel: %d\n", Channel);
    *Source = getLANResBuff.ConfigData.IPAddrSrc;

    return wRet;
}

uint16 LIBIPMI_HL_GetMACAddress(IPMI20_SESSION_T *pSession, INT8U *MACAddr,INT8U Channel, int timeout)
{
    uint16 wRet = 0;
    GetLanConfigReq_T getLANReqBuff;
    GetLanConfigRes_T getLANResBuff;

    getLANReqBuff.ChannelNum = Channel;
    getLANReqBuff.ParameterSelect = LAN_PARAM_MAC_ADDRESS;
    getLANReqBuff.SetSelect = 0;
    getLANReqBuff.BlockSelect = 0;

    wRet = IPMICMD_GetLANConfig(pSession, &getLANReqBuff, &getLANResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("LIBIPMI_HL_GetMACAddress: Error getting LAN configuration for channel %d\n", Channel);
        return wRet;
    }

    TDBG("Success getting MAC Address Configuration settings for channel: %d\n", Channel);
    memcpy(MACAddr,&(getLANResBuff.ConfigData.MACAddr),MAC_ADDR_LEN);

    return wRet;

}

uint16 LIBIPMI_HL_SetIPv6Source(IPMI20_SESSION_T *pSession, INT8U Source,INT8U Channel, int timeout)
{
    uint16 wRet = 0;
    SetLanConfigReq_T setLANReqBuff;
    SetLanConfigRes_T setLANResBuff;
    unsigned int ReqDataLen = 0;

    memset(&(setLANReqBuff), 0, sizeof(SetLanConfigReq_T));
    setLANReqBuff.ChannelNum = Channel;
    setLANReqBuff.ParameterSelect = GetLanOEMParamValue (LAN_PARAMS_AMI_OEM_IPV6_IP_ADDR_SOURCE);

    setLANReqBuff.ConfigData.IPv6_IPAddrSrc= Source;

    ReqDataLen = 2+(sizeof(setLANReqBuff.ConfigData.IPv6_IPAddrSrc));
    wRet = IPMICMD_SetLANConfig(pSession, &setLANReqBuff, ReqDataLen, &setLANResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("LIBIPMI_HL_SetIPv6Source: Error setting LAN configuration for channel %d\n", Channel);
        return wRet;
    }

    TDBG("Success setting IPv6 Address Source configuration for channel::%d\n", Channel);
    return wRet;
}

uint16 LIBIPMI_HL_GetIPv6Source(IPMI20_SESSION_T *pSession, INT8U *Source,INT8U Channel, int timeout)
{
    uint16 wRet = 0;
    GetLanConfigReq_T getLANReqBuff;
    GetLanConfigRes_T getLANResBuff;

    getLANReqBuff.ChannelNum = Channel;
    getLANReqBuff.ParameterSelect = GetLanOEMParamValue (LAN_PARAMS_AMI_OEM_IPV6_IP_ADDR_SOURCE);
    getLANReqBuff.SetSelect = 0;
    getLANReqBuff.BlockSelect = 0;

    wRet = IPMICMD_GetLANConfig(pSession, &getLANReqBuff, &getLANResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("LIBIPMI_HL_GetIPv6Source: Error getting LAN configuration for channel %d\n", Channel);
        return wRet;
    }

    TDBG("Success getting IPv6 Address Source Configuration settings for channel: %d\n", Channel);
    *Source = getLANResBuff.ConfigData.IPv6_IPAddrSrc;
    return wRet;
}

uint16 LIBIPMI_HL_Get_IPv6Source(IPMI20_SESSION_T *pSession, INT8U *Source,INT8U Channel, int timeout)
{
    uint16 wRet = 0;
    GetLanConfigReq_T getLANReqBuff;
    GetLanConfigRes_T getLANResBuff;

    getLANReqBuff.ChannelNum = Channel;
    getLANReqBuff.ParameterSelect = (LAN_PARAM_IPV6_STATIC_ADDRESS);
    getLANReqBuff.SetSelect = 0;
    getLANReqBuff.BlockSelect = 0;

    wRet = IPMICMD_GetLANConfig(pSession, &getLANReqBuff, &getLANResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("LIBIPMI_HL_GetIPv6Source: Error getting LAN configuration for channel %d\n", Channel);
        return wRet;
    }

    if(getLANResBuff.ConfigData.IPv6Addrs.IPv6_AddrSrcType & 0x80)
    {
        *Source=0x1; //Static Source
    }
    else if(getLANResBuff.ConfigData.IPv6Addrs.IPv6_AddrSrcType == 0x0)
    {
       *Source=0x2;  //DHCP Source
    }
    else
    {
       /* Nothing to do*/
    }
    return wRet;
}

uint16 LIBIPMI_HL_SetIPv4Address(IPMI20_SESSION_T *pSession, INT8U *Address,INT8U Channel, int timeout)
{
    uint16 wRet = 0;
    SetLanConfigReq_T setLANReqBuff;
    SetLanConfigRes_T setLANResBuff;
    unsigned int ReqDataLen = 0;

    memset(&(setLANReqBuff), 0, sizeof(SetLanConfigReq_T));
    setLANReqBuff.ChannelNum = Channel;
    setLANReqBuff.ParameterSelect = LAN_PARAM_IP_ADDRESS;

    memcpy(setLANReqBuff.ConfigData.IPAddr,Address,IP_ADDR_LEN);

    ReqDataLen = 2+(sizeof(setLANReqBuff.ConfigData.IPAddr));
    wRet = IPMICMD_SetLANConfig(pSession, &setLANReqBuff, ReqDataLen, &setLANResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("LIBIPMI_HL_SetIPv4Address: Error setting LAN configuration for channel %d\n", Channel);
        return wRet;
    }

    TDBG("Success setting IPv4 Address  configuration for channel::%d\n", Channel);
    return wRet;
}

uint16 LIBIPMI_HL_EnableVlan (IPMI20_SESSION_T *pSession, INT8U VlanStatus, INT16U vlanID, INT8U Priority, INT8U Channel, int timeout)
{
    uint16 wRet = 0;
    SetLanConfigReq_T setLANReqBuff;
    SetLanConfigRes_T setLANResBuff;
    unsigned int ReqDataLen = 0;

    /*Enable & Give Vlan ID */
    memset(&(setLANReqBuff), 0, sizeof(SetLanConfigReq_T));
    setLANReqBuff.ChannelNum = Channel;
    setLANReqBuff.ParameterSelect = LAN_PARAM_VLAN_ID;

    if ((VlanStatus > 0x01) || (vlanID > 0x0FFF))
    {
        wRet = CC_PARAM_OUT_OF_RANGE;
        TCRIT("LIBIPMI_HL_SetVlanPriority: Error setting LAN configuration - Parameter Out of Range");
        return wRet;
    }

    setLANReqBuff.ConfigData.VLANID = (vlanID | (VlanStatus <<15));
    ReqDataLen = 2+(sizeof(setLANReqBuff.ConfigData.VLANID));
    wRet = IPMICMD_SetLANConfig(pSession, &setLANReqBuff, ReqDataLen, &setLANResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("LIBIPMI_HL_EnableVlan: Error setting LAN configuration for channel %d\n", Channel);
        return wRet;
    }

    /*If Vlan Enabled Set the Priority*/
    if (VlanStatus == 0x01)
    {
        memset(&(setLANReqBuff), 0, sizeof(SetLanConfigReq_T));
        setLANReqBuff.ChannelNum = Channel;
        setLANReqBuff.ParameterSelect = LAN_PARAM_VLAN_PRIORITY;
        if (Priority > 0x07)
        {
            wRet = CC_PARAM_OUT_OF_RANGE;
            TCRIT("LIBIPMI_HL_SetVlanPriority: Error setting LAN configuration - Parameter Out of Range");
            return wRet;
        }
        setLANReqBuff.ConfigData.VLANPriority = Priority;
        ReqDataLen = 2+(sizeof(setLANReqBuff.ConfigData.VLANPriority));
        wRet = IPMICMD_SetLANConfig(pSession, &setLANReqBuff, ReqDataLen, &setLANResBuff, timeout);
        if(wRet != 0)
        {
            TCRIT("LIBIPMI_HL_SetVlanPriority: Error setting LAN configuration for channel %d\n", Channel);
            return wRet;
        }
    }
    return wRet;
}


uint16 LIBIPMI_HL_GetVlanStatus (IPMI20_SESSION_T *pSession, INT8U *VlanStatus, INT16U *vlanID, INT8U *Priority, INT8U Channel, int timeout)
{
    uint16 wRet = 0;
    GetLanConfigReq_T getLANReqBuff;
    GetLanConfigRes_T getLANResBuff;

    getLANReqBuff.ChannelNum = Channel;
    getLANReqBuff.ParameterSelect = LAN_PARAM_VLAN_ID;
    getLANReqBuff.SetSelect = 0;
    getLANReqBuff.BlockSelect = 0;
    wRet = IPMICMD_GetLANConfig(pSession, &getLANReqBuff, &getLANResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("LIBIPMI_HL_GetVlanStatus: Error getting LAN configuration for channel %d\n", Channel);
        return wRet;
    }
    *vlanID = (getLANResBuff.ConfigData.VLANID & 0x0FFF);
    *VlanStatus = 0;
    *Priority=0;
    if ((getLANResBuff.ConfigData.VLANID & VLAN_STATUS_BIT) == VLAN_STATUS_BIT)
    {
        *VlanStatus = 1;
        memset(&(getLANReqBuff), 0, sizeof(GetLanConfigReq_T));
        memset(&(getLANResBuff), 0, sizeof(GetLanConfigRes_T));
        getLANReqBuff.ChannelNum = Channel;
        getLANReqBuff.ParameterSelect = LAN_PARAM_VLAN_PRIORITY;
        getLANReqBuff.SetSelect = 0;
        getLANReqBuff.BlockSelect = 0;

        wRet = IPMICMD_GetLANConfig(pSession, &getLANReqBuff, &getLANResBuff, timeout);
        if(wRet != 0)
        {
            TCRIT("LIBIPMI_HL_GetVlanPriority: Error getting LAN configuration for channel %d\n", Channel);
            return wRet;
        }
        *Priority = getLANResBuff.ConfigData.VLANPriority;
    }

     return wRet;
}



uint16 LIBIPMI_HL_GetIPv4Address(IPMI20_SESSION_T *pSession, INT8U *Address,INT8U Channel, int timeout)
{
    uint16 wRet = 0;
    GetLanConfigReq_T getLANReqBuff;
    GetLanConfigRes_T getLANResBuff;

    getLANReqBuff.ChannelNum = Channel;
    getLANReqBuff.ParameterSelect = LAN_PARAM_IP_ADDRESS;
    getLANReqBuff.SetSelect = 0;
    getLANReqBuff.BlockSelect = 0;

    wRet = IPMICMD_GetLANConfig(pSession, &getLANReqBuff, &getLANResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("LIBIPMI_HL_GetIPv4Address: Error getting LAN configuration for channel %d\n", Channel);
        return wRet;
    }

    memcpy(Address,getLANResBuff.ConfigData.IPAddr,4);
    TDBG("Success getting IPv4 Address Configuration settings for channel: %d\n", Channel);

    return wRet;
}

uint16 LIBIPMI_HL_SetIPv6Address(IPMI20_SESSION_T *pSession, INT8U *Address,INT8U Channel, int timeout)
{
    uint16 wRet = 0;
    SetLanConfigReq_T setLANReqBuff;
    SetLanConfigRes_T setLANResBuff;
    unsigned int ReqDataLen = 0;

    memset(&(setLANReqBuff), 0, sizeof(SetLanConfigReq_T));
    setLANReqBuff.ChannelNum = Channel;
    setLANReqBuff.ParameterSelect = GetLanOEMParamValue (LAN_PARAMS_AMI_OEM_IPV6_IP_ADDR);

    memcpy(setLANReqBuff.ConfigData.IPv6Addr.IPv6_IPAddr,Address,IP6_ADDR_LEN);
    setLANReqBuff.ConfigData.IPv6Addr.IPv6_Cntr = 0;

    ReqDataLen = 2 + (sizeof(IPv6Addr_T));
    wRet = IPMICMD_SetLANConfig(pSession, &setLANReqBuff, ReqDataLen, &setLANResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("LIBIPMI_HL_SetIPv6Address: Error setting LAN configuration for channel %d\n", Channel);
        return wRet;
    }

    TDBG("Success setting IPv6 Address  configuration for channel::%d\n", Channel);
    return wRet;
}

uint16 LIBIPMI_HL_Set_IPv6Address(IPMI20_SESSION_T *pSession, INT8U *Address,INT8U Prefix,INT8U Channel, int timeout,INT8U flag,INT8U v6Index)
{
    uint16 wRet = 0;
    SetLanConfigReq_T setLANReqBuff;
    SetLanConfigRes_T setLANResBuff;
    unsigned int ReqDataLen = 0;

    memset(&(setLANReqBuff), 0, sizeof(SetLanConfigReq_T));
    setLANReqBuff.ChannelNum = Channel;
    setLANReqBuff.ParameterSelect = GetLanOEMParamValue (LAN_PARAM_IPV6_STATIC_ADDRESS);
    memcpy(setLANReqBuff.ConfigData.IPv6Addrs.IPv6_Address,Address,IP6_ADDR_LEN);
  //  setLANReqBuff.ConfigData.IPv6Addrs.IPv6_AddrSrcType=0x80; //Static IP Address Source &th bit Enabled.
    if(flag == 0x2)
        setLANReqBuff.ConfigData.IPv6Addrs.IPv6_AddrSrcType=0x00;
    else
        setLANReqBuff.ConfigData.IPv6Addrs.IPv6_AddrSrcType=0x80;

    setLANReqBuff.ConfigData.IPv6Addrs.IPv6_PrefixLength= Prefix;
    setLANReqBuff.ConfigData.IPv6Addrs.SetSelector = v6Index;

    ReqDataLen = 2 + (sizeof(IPv6Addrs_T));
    wRet = IPMICMD_SetLANConfig(pSession, &setLANReqBuff, ReqDataLen, &setLANResBuff, timeout);

    if(wRet != 0)
    {
        TCRIT("LIBIPMI_HL_SetIPv6Address: Error setting LAN configuration for channel %d\n", Channel);
        return wRet;
    }

    TDBG("Success setting IPv6 Address  configuration for channel::%d\n", Channel);
    return wRet;
}

 uint16 LIBIPMI_HL_Set_IPv6Source(IPMI20_SESSION_T *pSession, INT8U Source,INT8U Channel, int timeout)
 {
      uint16 wRet = 0;
      SetLanConfigReq_T setLANReqBuff;
      SetLanConfigRes_T setLANResBuff;
      unsigned int ReqDataLen = 0;

      memset(&(setLANReqBuff), 0, sizeof(SetLanConfigReq_T));
      setLANReqBuff.ChannelNum = Channel;
      setLANReqBuff.ParameterSelect = GetLanOEMParamValue (LAN_PARAM_IPV6_STATIC_ADDRESS);

        setLANReqBuff.ConfigData.IPv6Addrs.IPv6_AddrSrcType=Source;

      ReqDataLen = 2+ (sizeof(IPv6Addrs_T));
      wRet = IPMICMD_SetLANConfig(pSession, &setLANReqBuff, ReqDataLen, &setLANResBuff, timeout);
      if(wRet != 0)
      {
          TCRIT("LIBIPMI_HL_SetIPv6Source: Error setting LAN configuration for channel %d\n", Channel);

      }
     return wRet;;
 }


uint16 LIBIPMI_HL_GetAllIPv6Addresses(IPMI20_SESSION_T *pSession, INT8U Channel, INT8U *Address, INT8U *pIPCount, int timeout)
{
	uint16 wRet = 0, i = 0;
	INT8U tmp[IP6_ADDR_LEN];
	GetLanConfigReq_T getLANReqBuff;
	GetLanConfigRes_T getLANResBuff;


	GetAllIPv6Address_T *pReqAddress = (GetAllIPv6Address_T *)Address;
	*pIPCount = 0;

	getLANReqBuff.ChannelNum = Channel;
	getLANReqBuff.ParameterSelect = GetLanOEMParamValue (LAN_PARAMS_AMI_OEM_IPV6_IP_ADDR);
	getLANReqBuff.BlockSelect = 0;

	memset(tmp,0x0,sizeof(tmp)); // To identify empty IPv6 Address

	// Retrieving all the IPV6 Addresses for the channel
	for ( i = 0 ; i < MAX_IPV6_ADDRS ; i++ )
	{
		getLANReqBuff.SetSelect = i;
		wRet = IPMICMD_GetLANConfig(pSession, &getLANReqBuff, &getLANResBuff, timeout);
		if(wRet != 0)
		{
		        TCRIT("LIBIPMI_HL_GetAllIPv6Addresses: Error getting LAN configuration for channel %d\n", Channel);
		        return wRet;
		}

		// If the retrieved address is valid
		if ( memcmp(&(getLANResBuff.ConfigData.IPv6_LinkAddr),tmp,IP6_ADDR_LEN) != 0 )
		{
			GetLanConfigReq_T getLANCfgReqPrefix;
			GetLanConfigRes_T getLANCfgResPrefix;

			//Retrieving IPv6 Subnet Mask Prefix
			getLANCfgReqPrefix.ChannelNum = Channel;
		        getLANCfgReqPrefix.ParameterSelect = GetLanOEMParamValue (LAN_PARAMS_AMI_OEM_IPV6_PREFIX_LENGTH);
		        getLANCfgReqPrefix.BlockSelect = 0;
			getLANCfgReqPrefix.SetSelect = i;

			wRet = IPMICMD_GetLANConfig(pSession, &getLANCfgReqPrefix, &getLANCfgResPrefix, timeout);
	                if(wRet != 0)
        	        {
                	        TCRIT("LIBIPMI_HL_GetAllIPv6Addresses: Error getting LAN configuration for channel %d\n", Channel);
                        	return wRet;
	                }

			// Copying the IPv6 Address and subnet mask prefix
			memcpy(&(pReqAddress->Address[*pIPCount]),&(getLANResBuff.ConfigData.IPv6_LinkAddr),IP6_ADDR_LEN);

			pReqAddress->Prefix[*pIPCount] = getLANCfgResPrefix.ConfigData.IPv6_LinkAddrPrefix;
			*pIPCount += 1;
		}
	}

	// Retriving IPv6 Source info for the corresponding channel
 	if ( (wRet = LIBIPMI_HL_GetIPv6Source(pSession,&(pReqAddress->Source),Channel,timeout)) != 0 )
	{
		TCRIT("LIBIPMI_HL_GetAllIPv6Addresses: Error getting LAN configuration for channel %d\n", Channel);
                return wRet;
	}

	TDBG("Success retrieving all IPv6 Addresses for channel: %d\n", Channel);
	return wRet;
}

uint16 LIBIPMI_HL_Get_AllIPv6Addresses(IPMI20_SESSION_T *pSession, INT8U Channel, INT8U *Address, INT8U *pIPCount, int timeout)
{
    uint16 wRet = 0;
    INT8U tmp[IP6_ADDR_LEN];
    GetLanConfigReq_T getLANReqBuff;
    GetLanConfigRes_T getLANResBuff;

    GetAllIPv6Address_T *pReqAddress = (GetAllIPv6Address_T *)Address;
    *pIPCount = 0;

    getLANReqBuff.ChannelNum = Channel;
    getLANReqBuff.ParameterSelect =LAN_PARAM_IPV6_STATIC_ADDRESS ;
    getLANReqBuff.BlockSelect = 0;
    getLANReqBuff.SetSelect = 0;

    memset(tmp,0x0,sizeof(tmp)); // To identify empty IPv6 Address

    wRet = IPMICMD_GetLANConfig(pSession, &getLANReqBuff, &getLANResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("LIBIPMI_HL_GetAllIPv6Addresses: Error getting LAN configuration for channel %d\n", Channel);
        return wRet;
    }

   if(getLANResBuff.ConfigData.IPv6Addrs.IPv6_AddrSrcType==0x80)
   {

        // If the retrieved address is valid
        if ( memcmp(&(getLANResBuff.ConfigData.IPv6Addrs.IPv6_Address),tmp,IP6_ADDR_LEN) != 0 )
        {
            // Copying the IPv6 Address and subnet mask prefix
            memcpy(&(pReqAddress->Address[*pIPCount]),&(getLANResBuff.ConfigData.IPv6Addrs.IPv6_Address),IP6_ADDR_LEN);
            pReqAddress->Prefix[*pIPCount] = getLANResBuff.ConfigData.IPv6Addrs.IPv6_PrefixLength;
            *pIPCount = 1;
        }
    }
    else
    {
        getLANReqBuff.ChannelNum = Channel;
        getLANReqBuff.ParameterSelect =LAN_PARAM_IPV6_DYNAMIC_ADDRESS ;
        getLANReqBuff.BlockSelect = 0;
        getLANReqBuff.SetSelect = 0;

        wRet = IPMICMD_GetLANConfig(pSession, &getLANReqBuff, &getLANResBuff, timeout);
        if(wRet != 0)
        {
            TCRIT("LIBIPMI_HL_Get_AllIPv6Addresses: Error getting LAN configuration for channel %d\n", Channel);
            return wRet;
        }
        if ( memcmp(&(getLANResBuff.ConfigData.IPv6_DynamicAddrs.IPv6_DynamicAddress),tmp,IP6_ADDR_LEN) != 0 )
        {
            // Copying the IPv6 Address and subnet mask prefix
            memcpy(&(pReqAddress->Address[*pIPCount]),&(getLANResBuff.ConfigData.IPv6_DynamicAddrs.IPv6_DynamicAddress),IP6_ADDR_LEN);
            pReqAddress->Prefix[*pIPCount] = getLANResBuff.ConfigData.IPv6Addrs.IPv6_PrefixLength;
            *pIPCount = 1;
        }
    }

    // Retriving IPv6 Source info for the corresponding channel
    if ( (wRet = LIBIPMI_HL_Get_IPv6Source(pSession,&(pReqAddress->Source),Channel,timeout)) != 0 )
    {
        TCRIT("LIBIPMI_HL_GetAllIPv6Addresses: Error getting LAN configuration for channel %d\n", Channel);
        return wRet;
    }

    TDBG("Success retrieving all IPv6 Addresses for channel: %d\n", Channel);
    return wRet;

}

uint16 LIBIPMI_HL_GetIPv6Address(IPMI20_SESSION_T *pSession, INT8U *Address,INT8U Channel, int timeout)
{
    uint16 wRet = 0;
    GetLanConfigReq_T getLANReqBuff;
    GetLanConfigRes_T getLANResBuff;

    getLANReqBuff.ChannelNum = Channel;
    getLANReqBuff.ParameterSelect = GetLanOEMParamValue (LAN_PARAMS_AMI_OEM_IPV6_IP_ADDR);
    getLANReqBuff.SetSelect = 0;
    getLANReqBuff.BlockSelect = 0;

    wRet = IPMICMD_GetLANConfig(pSession, &getLANReqBuff, &getLANResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("LIBIPMI_HL_GetIPv6Address: Error getting LAN configuration for channel %d\n", Channel);
        return wRet;
    }

    memcpy(Address,&(getLANResBuff.ConfigData.IPv6Addrs.IPv6_Address),IP6_ADDR_LEN);
    TDBG("Success getting IPv6 Address Configuration settings for channel: %d\n", Channel);

    return wRet;
}

uint16 LIBIPMI_HL_GetI_Pv6Address(IPMI20_SESSION_T *pSession, INT8U *Address,INT8U Channel, int timeout,INT8U* v6Index)
{
    uint16 wRet = 0;
    GetLanConfigReq_T getLANReqBuff;
    GetLanConfigRes_T getLANResBuff;
    INT8U tmp[IP6_ADDR_LEN];
	int i = 0;
    FILE *fp =NULL;
    INT8U LinkAddress[INET6_ADDRSTRLEN];
    INT8U buff[INET6_ADDRSTRLEN];

    getLANReqBuff.ChannelNum = Channel;
    getLANReqBuff.ParameterSelect = LAN_PARAM_IPV6_DYNAMIC_ADDRESS ;
    getLANReqBuff.BlockSelect = 0;
    getLANReqBuff.SetSelect = *v6Index;

    memset(tmp,0x0,sizeof(tmp)); // To identify empty IPv6 Address
	memset(&buff,0x0,sizeof(buff));
	fp = fopen("/var/ipv6addr","w+");
	LIBIPMI_HL_GetIPv6LinkAddress(pSession,LinkAddress,Channel,timeout);
	inet_ntop(AF_INET6,LinkAddress,(char *)buff,INET6_ADDRSTRLEN);
	sprintf((char *)buff,"%s ",(char *)buff);

	fputs((char *)buff,fp);
	for(i =0;i<16;i++){
		memset(&buff,0x0,sizeof(buff));
		getLANReqBuff.SetSelect = i;
		memset(&getLANResBuff,0,sizeof(getLANResBuff));
		wRet = IPMICMD_GetLANConfig(pSession, &getLANReqBuff, &getLANResBuff, timeout);
		inet_ntop(AF_INET6, (getLANResBuff.ConfigData.IPv6Addrs.IPv6_Address),(char *)buff , INET6_ADDRSTRLEN);

		sprintf((char *)buff,"%s ",(char *)buff);
		fputs((char *)buff,fp);
	}

	fclose(fp);
	memset(tmp,0x0,sizeof(tmp)); // To identify empty IPv6 Address

    wRet = IPMICMD_GetLANConfig(pSession, &getLANReqBuff, &getLANResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("LIBIPMI_HL_GetI_Pv6Address: Error getting LAN configuration LAN_PARAM_IPV6_STATIC_ADDRESS for channel %d\n", Channel);
        return wRet;
    }

    if(getLANResBuff.ConfigData.IPv6Addrs.IPv6_AddrSrcType==0x80)
    {
         // If the retrieved address is valid
         if ( memcmp(&(getLANResBuff.ConfigData.IPv6Addrs.IPv6_Address),tmp,IP6_ADDR_LEN) != 0 )
         {
             // Copying the IPv6 Address and subnet mask prefix
             memcpy(Address,&(getLANResBuff.ConfigData.IPv6Addrs.IPv6_Address),IP6_ADDR_LEN);
   	      *v6Index=getLANResBuff.ConfigData.IPv6Addrs.SetSelector;
    	  }
    }
    else
    {
        getLANReqBuff.ChannelNum = Channel;
        getLANReqBuff.ParameterSelect =LAN_PARAM_IPV6_DYNAMIC_ADDRESS ;
        getLANReqBuff.BlockSelect = 0;
        getLANReqBuff.SetSelect = 0;

        wRet = IPMICMD_GetLANConfig(pSession, &getLANReqBuff, &getLANResBuff, timeout);
        if(wRet != 0)
        {
            TCRIT("LIBIPMI_HL_GetI_Pv6Address: Error getting LAN configuration LAN_PARAM_IPV6_STATIC_ADDRESS for channel %d\n", Channel);
            return wRet;
        }
        if ( memcmp(&(getLANResBuff.ConfigData.IPv6_DynamicAddrs.IPv6_DynamicAddress),tmp,IP6_ADDR_LEN) != 0 )
        {
            // Copying the IPv6 Address and subnet mask prefix
            memcpy(Address,&(getLANResBuff.ConfigData.IPv6_DynamicAddrs.IPv6_DynamicAddress),IP6_ADDR_LEN);
        }
    }

    TDBG("Success getting IPv6 Address Configuration settings for channel: %d\n", Channel);

    return wRet;
}


uint16 LIBIPMI_HL_Get_IPv6_static_Address(IPMI20_SESSION_T *pSession, INT8U *Address,INT8U Channel, int timeout,INT8U* v6Index,INT8U *Prefix,INT8U *Source)
{
    uint16 wRet = 0;
    GetLanConfigReq_T getLANReqBuff;
    GetLanConfigRes_T getLANResBuff;
    INT8U tmp[IP6_ADDR_LEN];

    getLANReqBuff.ChannelNum = Channel;
    getLANReqBuff.ParameterSelect =LAN_PARAM_IPV6_STATIC_ADDRESS ;
    getLANReqBuff.BlockSelect = 0;
    getLANReqBuff.SetSelect = *v6Index;

    memset(tmp,0x0,sizeof(tmp)); // To identify empty IPv6 Address
    if(0)
    {
        Prefix=Prefix;
	Source=Source;
    }

    wRet = IPMICMD_GetLANConfig(pSession, &getLANReqBuff, &getLANResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("LIBIPMI_HL_GetI_Pv6Address: Error getting LAN configuration LAN_PARAM_IPV6_STATIC_ADDRESS for channel %d\n", Channel);
        return wRet;
    }

    if(getLANResBuff.ConfigData.IPv6Addrs.IPv6_AddrSrcType==0x80)
    {
         // If the retrieved address is valid
         if ( memcmp(&(getLANResBuff.ConfigData.IPv6Addrs.IPv6_Address),tmp,IP6_ADDR_LEN) != 0 )
         {
             // Copying the IPv6 Address and subnet mask prefix
             memcpy(Address,&(getLANResBuff.ConfigData.IPv6Addrs.IPv6_Address),IP6_ADDR_LEN);
	      *v6Index=getLANResBuff.ConfigData.IPv6Addrs.SetSelector;
         }
    }
    else
    {
          //memset(Address,0x0,sizeof(Address));
          memset(Address,0x0,IP6_ADDR_LEN);
    }

    TDBG("Success getting IPv6 Address Configuration settings for channel: %d\n", Channel);

    return wRet;
}

uint16 LIBIPMI_HL_GetIPv6LinkAddress(IPMI20_SESSION_T *pSession, INT8U *Address,INT8U Channel, int timeout)
{
    uint16 wRet = 0;

    GetLanConfigReq_T getLANReqBuff;
    GetLanConfigRes_T getLANResBuff;

    getLANReqBuff.ChannelNum = Channel;
    getLANReqBuff.ParameterSelect = GetLanOEMParamValue (LAN_PARAMS_AMI_OEM_IPV6_LINK_ADDR);
    getLANReqBuff.SetSelect = 0;
    getLANReqBuff.BlockSelect = 0;

    wRet = IPMICMD_GetLANConfig(pSession, &getLANReqBuff, &getLANResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("LIBIPMI_HL_GetIPv6LinkAddr: Error getting LAN configuration for channel %d\n", Channel);
        return wRet;
    }

    memcpy(Address,&(getLANResBuff.ConfigData.IPv6_LinkAddr),IP6_ADDR_LEN);
    TDBG("Success getting IPv6 Link Address Configuration settings for channel: %d\n", Channel);

    return wRet;
}

uint16 LIBIPMI_HL_SetIPv4NetMask(IPMI20_SESSION_T *pSession, INT8U *Mask,INT8U Channel, int timeout)
{
    uint16 wRet = 0;
    SetLanConfigReq_T setLANReqBuff;
    SetLanConfigRes_T setLANResBuff;
    unsigned int ReqDataLen = 0;

    memset(&(setLANReqBuff), 0, sizeof(SetLanConfigReq_T));
    setLANReqBuff.ChannelNum = Channel;
    setLANReqBuff.ParameterSelect = LAN_PARAM_SUBNET_MASK;

    memcpy(setLANReqBuff.ConfigData.SubNetMask,Mask,IP_ADDR_LEN);

    ReqDataLen = 2+(sizeof(setLANReqBuff.ConfigData.SubNetMask));
    wRet = IPMICMD_SetLANConfig(pSession, &setLANReqBuff, ReqDataLen, &setLANResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("LIBIPMI_HL_SetIPv4NetMask: Error setting LAN configuration for channel %d\n", Channel);
        return wRet;
    }

    TDBG("Success setting IPv4 NetMask configuration for channel::%d\n", Channel);
    return wRet;
}

uint16 LIBIPMI_HL_GetIPv4NetMask(IPMI20_SESSION_T *pSession, INT8U *Mask,INT8U Channel, int timeout)
{
    uint16 wRet = 0;
    GetLanConfigReq_T getLANReqBuff;
    GetLanConfigRes_T getLANResBuff;

    getLANReqBuff.ChannelNum = Channel;
    getLANReqBuff.ParameterSelect = LAN_PARAM_SUBNET_MASK;
    getLANReqBuff.SetSelect = 0;
    getLANReqBuff.BlockSelect = 0;

    wRet = IPMICMD_GetLANConfig(pSession, &getLANReqBuff, &getLANResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("LIBIPMI_HL_GetIPv4NetMask: Error getting LAN configuration for channel %d\n", Channel);
        return wRet;
    }

    memcpy(Mask,&(getLANResBuff.ConfigData.SubNetMask),IP_ADDR_LEN);
    TDBG("Success getting IPv4 NetMask Configuration settings for channel: %d\n", Channel);

    return wRet;
}

uint16 LIBIPMI_HL_SetIPv6Prefix(IPMI20_SESSION_T *pSession, INT8U Prefix,INT8U Channel, int timeout)
{
    uint16 wRet = 0;
    SetLanConfigReq_T setLANReqBuff;
    SetLanConfigRes_T setLANResBuff;
    unsigned int ReqDataLen = 0;

    memset(&(setLANReqBuff), 0, sizeof(SetLanConfigReq_T));
    setLANReqBuff.ChannelNum = Channel;
    setLANReqBuff.ParameterSelect = GetLanOEMParamValue (LAN_PARAMS_AMI_OEM_IPV6_PREFIX_LENGTH);

    setLANReqBuff.ConfigData.IPv6Prefix.IPv6_PrefixLen= Prefix;
    setLANReqBuff.ConfigData.IPv6Prefix.IPv6_Prepos = 0;

    ReqDataLen = 2+(sizeof (IPv6Prefix_T));
    wRet = IPMICMD_SetLANConfig(pSession, &setLANReqBuff, ReqDataLen, &setLANResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("LIBIPMI_HL_SetIPv6Prefix: Error setting LAN configuration for channel %d\n", Channel);
        return wRet;
    }

    TDBG("Success setting IPv6 Prefix Length configuration for channel::%d\n", Channel);
    return wRet;
}

uint16 LIBIPMI_HL_GetIPv6Prefix(IPMI20_SESSION_T *pSession, INT8U *Prefix,INT8U Channel, int timeout)
{
    uint16 wRet = 0;
    GetLanConfigReq_T getLANReqBuff;
    GetLanConfigRes_T getLANResBuff;

    getLANReqBuff.ChannelNum = Channel;
    getLANReqBuff.ParameterSelect = GetLanOEMParamValue (LAN_PARAMS_AMI_OEM_IPV6_PREFIX_LENGTH);
    getLANReqBuff.SetSelect = 0;
    getLANReqBuff.BlockSelect = 0;

    wRet = IPMICMD_GetLANConfig(pSession, &getLANReqBuff, &getLANResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("LIBIPMI_HL_GetIPv6Prefix: Error getting LAN configuration for channel %d\n", Channel);
        return wRet;
    }

    *Prefix = getLANResBuff.ConfigData.IPv6Addrs.IPv6_PrefixLength;
    TDBG("Success getting IPv6 Prefix Configuration settings for channel: %d\n", Channel);

    return wRet;
}

uint16 LIBIPMI_HL_Get_IPv6Prefix(IPMI20_SESSION_T *pSession, INT8U *Prefix,INT8U Channel, int timeout)
{
    uint16 wRet = 0;
    GetLanConfigReq_T getLANReqBuff;
    GetLanConfigRes_T getLANResBuff;

    getLANReqBuff.ChannelNum = Channel;
    getLANReqBuff.ParameterSelect =LAN_PARAM_IPV6_STATIC_ADDRESS ;
    getLANReqBuff.BlockSelect = 0;
    getLANReqBuff.SetSelect = 0;

    wRet = IPMICMD_GetLANConfig(pSession, &getLANReqBuff, &getLANResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("LIBIPMI_HL_Get_IPv6Prefix: Error getting LAN configuration LAN_PARAM_IPV6_DYNAMIC_ADDRESS for channel %d\n", Channel);
        return wRet;
    }

    if(getLANResBuff.ConfigData.IPv6Addrs.IPv6_AddrSrcType==0x80)
    {
        *Prefix = getLANResBuff.ConfigData.IPv6Addrs.IPv6_PrefixLength;
    }
    else
    {
        getLANReqBuff.ChannelNum = Channel;
        getLANReqBuff.ParameterSelect =LAN_PARAM_IPV6_DYNAMIC_ADDRESS ;
        getLANReqBuff.BlockSelect = 0;
        getLANReqBuff.SetSelect = 0;

        wRet = IPMICMD_GetLANConfig(pSession, &getLANReqBuff, &getLANResBuff, timeout);
        if(wRet != 0)
        {
            TCRIT("LIBIPMI_HL_Get_IPv6Prefix: Error getting LAN configuration LAN_PARAM_IPV6_DYNAMIC_ADDRESS for channel %d\n", Channel);
            return wRet;
        }
        *Prefix = getLANResBuff.ConfigData.IPv6Addrs.IPv6_PrefixLength;
    }

    TDBG("Success getting IPv6 Prefix Configuration settings for channel: %d\n", Channel);

    return wRet;
}

uint16 LIBIPMI_HL_GetIPv6LinkPrefix (IPMI20_SESSION_T *pSession, INT8U *Prefix,INT8U Channel, int timeout)
{
    uint16 wRet = 0;
    GetLanConfigReq_T getLANReqBuff;
    GetLanConfigRes_T getLANResBuff;

    getLANReqBuff.ChannelNum = Channel;
    getLANReqBuff.ParameterSelect = GetLanOEMParamValue (LAN_PARAMS_AMI_OEM_IPV6_LINK_ADDR_PREFIX);
    getLANReqBuff.SetSelect = 0;
    getLANReqBuff.BlockSelect = 0;

    wRet = IPMICMD_GetLANConfig(pSession, &getLANReqBuff, &getLANResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("LIBIPMI_HL_GetIPv6LinkPrefix: Error getting LAN configuration for channel %d\n", Channel);
        return wRet;
    }

    *Prefix = getLANResBuff.ConfigData.IPv6_LinkAddrPrefix;
    TDBG("Success getting IPv6 LinkPrefix Configuration settings for channel: %d\n", Channel);

    return wRet;
}

uint16 LIBIPMI_HL_SetIPv4Gateway(IPMI20_SESSION_T *pSession, INT8U *Gateway,INT8U Channel, int timeout)
{
    uint16 wRet = 0;
    SetLanConfigReq_T setLANReqBuff;
    SetLanConfigRes_T setLANResBuff;
    unsigned int ReqDataLen = 0;

    memset(&(setLANReqBuff), 0, sizeof(SetLanConfigReq_T));
    setLANReqBuff.ChannelNum = Channel;
    setLANReqBuff.ParameterSelect = LAN_PARAM_DEFAULT_GATEWAY_IP;

    memcpy(setLANReqBuff.ConfigData.DefaultGatewayIPAddr,Gateway,IP_ADDR_LEN);

    ReqDataLen = 2+(sizeof(setLANReqBuff.ConfigData.DefaultGatewayIPAddr));
    wRet = IPMICMD_SetLANConfig(pSession, &setLANReqBuff, ReqDataLen, &setLANResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("LIBIPMI_HL_SetIPv4Gateway: Error setting LAN configuration for channel %d\n", Channel);
        return wRet;
    }

    TDBG("Success setting IPv4 Default Gateway Address configuration for channel::%d\n", Channel);
    return wRet;
}

uint16 LIBIPMI_HL_GetIPv4Gateway(IPMI20_SESSION_T *pSession, INT8U *Gateway,INT8U Channel, int timeout)
{
    uint16 wRet = 0;
    GetLanConfigReq_T getLANReqBuff;
    GetLanConfigRes_T getLANResBuff;

    getLANReqBuff.ChannelNum = Channel;
    getLANReqBuff.ParameterSelect = LAN_PARAM_DEFAULT_GATEWAY_IP;
    getLANReqBuff.SetSelect = 0;
    getLANReqBuff.BlockSelect = 0;

    wRet = IPMICMD_GetLANConfig(pSession, &getLANReqBuff, &getLANResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("LIBIPMI_HL_GetIPv4Gateway: Error getting LAN configuration for channel %d\n", Channel);
        return wRet;
    }

    memcpy(Gateway,&(getLANResBuff.ConfigData.DefaultGatewayIPAddr),IP_ADDR_LEN);
    TDBG("Success getting IPv4 Gateway Configuration settings for channel: %d\n", Channel);

    return wRet;
}

uint16 LIBIPMI_HL_SetIPv6Gateway(IPMI20_SESSION_T *pSession, INT8U *Gateway,INT8U Channel, int timeout)
{
    uint16 wRet = 0;
    SetLanConfigReq_T setLANReqBuff;
    SetLanConfigRes_T setLANResBuff;
    unsigned int ReqDataLen = 0;

    memset(&(setLANReqBuff), 0, sizeof(SetLanConfigReq_T));
    setLANReqBuff.ChannelNum = Channel;
    setLANReqBuff.ParameterSelect = GetLanOEMParamValue (LAN_PARAMS_AMI_OEM_IPV6_GATEWAY_IP);

    memcpy(setLANReqBuff.ConfigData.IPv6_GatewayIPAddr,Gateway,IP6_ADDR_LEN);

    ReqDataLen = 2+(sizeof(setLANReqBuff.ConfigData.IPv6_GatewayIPAddr));
    wRet = IPMICMD_SetLANConfig(pSession, &setLANReqBuff, ReqDataLen, &setLANResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("LIBIPMI_HL_SetIPv6Gateway: Error setting LAN configuration for channel %d\n", Channel);
        return wRet;
    }

    TDBG("Success setting IPv6 Gateway Address configuration for channel::%d\n", Channel);
    return wRet;
}

uint16 LIBIPMI_HL_GetIPv6Gateway(IPMI20_SESSION_T *pSession, INT8U *Gateway,INT8U Channel, int timeout)
{
    uint16 wRet = 0;
    GetLanConfigReq_T getLANReqBuff;
    GetLanConfigRes_T getLANResBuff;

    getLANReqBuff.ChannelNum = Channel;
    getLANReqBuff.ParameterSelect = GetLanOEMParamValue (LAN_PARAMS_AMI_OEM_IPV6_GATEWAY_IP);
    getLANReqBuff.SetSelect = 0;
    getLANReqBuff.BlockSelect = 0;

    wRet = IPMICMD_GetLANConfig(pSession, &getLANReqBuff, &getLANResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("LIBIPMI_HL_GetIPv6Gateway: Error getting LAN configuration for channel %d\n", Channel);
        return wRet;
    }

    memcpy(Gateway,&(getLANResBuff.ConfigData.IPv6_GatewayIPAddr),IP6_ADDR_LEN);
    TDBG("Success getting IPv6 Gateway Configuration settings for channel: %d\n", Channel);

    return wRet;
}

uint16 LIBIPMI_HL_SetIPv6Enable(IPMI20_SESSION_T *pSession, INT8U Enable,INT8U Channel, int timeout)
{
    uint16 wRet = 0;
    SetLanConfigReq_T setLANReqBuff;
    SetLanConfigRes_T setLANResBuff;
    unsigned int ReqDataLen = 0;

    memset(&(setLANReqBuff), 0, sizeof(SetLanConfigReq_T));
    setLANReqBuff.ChannelNum = Channel;
    setLANReqBuff.ParameterSelect = GetLanOEMParamValue (LAN_PARAMS_AMI_OEM_IPV6_ENABLE);

    setLANReqBuff.ConfigData.IPv6_Enable= Enable;

    ReqDataLen = 2+(sizeof(setLANReqBuff.ConfigData.IPv6_Enable));
    wRet = IPMICMD_SetLANConfig(pSession, &setLANReqBuff, ReqDataLen, &setLANResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("LIBIPMI_HL_SetIPv6Source: Error setting LAN configuration for channel %d\n", Channel);
        return wRet;
    }

    TDBG("Success setting IPv6 Address Source configuration for channel::%d\n", Channel);
    return wRet;

}

uint16 LIBIPMI_HL_SetIPv4IPv6Enable(IPMI20_SESSION_T *pSession, INT8U Enable,INT8U Channel, int timeout)
{
    uint16 wRet = 0;
    SetLanConfigReq_T setLANReqBuff;
    SetLanConfigRes_T setLANResBuff;
    unsigned int ReqDataLen = 0;

    memset(&(setLANReqBuff), 0, sizeof(SetLanConfigReq_T));
    setLANReqBuff.ChannelNum = Channel;
    setLANReqBuff.ParameterSelect =(LAN_PARAM_IPV6_IPV4_ADDRESS_ENABLE);

    setLANReqBuff.ConfigData.IPv6IPv4AddrEnable= Enable;
    ReqDataLen = 2+(sizeof(setLANReqBuff.ConfigData.IPv6IPv4AddrEnable));
    wRet = IPMICMD_SetLANConfig(pSession, &setLANReqBuff, ReqDataLen, &setLANResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("LIBIPMI_HL_SetIPv6Source: Error setting LAN configuration for channel %d\n", Channel);
        return wRet;
    }

    TDBG("Success setting IPv6 Address Source configuration for channel::%d\n", Channel);
    return wRet;

}

uint16 LIBIPMI_HL_GetIPv6Enable(IPMI20_SESSION_T *pSession, INT8U *Enable,INT8U Channel, int timeout)
{
    uint16 wRet = 0;
    GetLanConfigReq_T getLANReqBuff;
    GetLanConfigRes_T getLANResBuff;

    getLANReqBuff.ChannelNum = Channel;
    getLANReqBuff.ParameterSelect = GetLanOEMParamValue (LAN_PARAMS_AMI_OEM_IPV6_ENABLE);
    getLANReqBuff.SetSelect = 0;
    getLANReqBuff.BlockSelect = 0;

    wRet = IPMICMD_GetLANConfig(pSession, &getLANReqBuff, &getLANResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("LIBIPMI_HL_GetIPv6Enable: Error getting LAN configuration for channel %d\n", Channel);
        return wRet;
    }

    *Enable = getLANResBuff.ConfigData.IPv6_Enable;
    TDBG("Success getting IPv6 Eanble Configuration settings for channel: %d\n", Channel);

    return wRet;
}

uint16 LIBIPMI_HL_Get_IPv6Enable(IPMI20_SESSION_T *pSession, INT8U *Enable,INT8U Channel, int timeout)
{
    uint16 wRet = 0;
    GetLanConfigReq_T getLANReqBuff;
    GetLanConfigRes_T getLANResBuff;

    getLANReqBuff.ChannelNum = Channel;
    getLANReqBuff.ParameterSelect = LAN_PARAM_IPV6_IPV4_ADDRESS_ENABLE;
    getLANReqBuff.SetSelect = 0;
    getLANReqBuff.BlockSelect = 0;

    wRet = IPMICMD_GetLANConfig(pSession, &getLANReqBuff, &getLANResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("LIBIPMI_HL_GetIPv6Enable: Error getting LAN configuration for channel %d\n", Channel);
        return wRet;
    }

    *Enable = getLANResBuff.ConfigData.IPv6_Enable;
    TDBG("Success getting IPv6 Eanble Configuration settings for channel: %d\n", Channel);

    return wRet;
}

uint16 LIBIPMI_HL_GetIPv4NetworkCfg(IPMI20_SESSION_T *pSession,
					INT8U *Source,
					INT8U *Address,
					INT8U *Mask,
					INT8U *Gateway,
					INT8U Channel,
					int timeout)
{
	uint16 wRet = 0;
	wRet = LIBIPMI_HL_GetIPv4Source(pSession, Source, Channel,timeout);
	if (wRet != 0)
	{
		TCRIT ("LIBIPMI, Error getting IPv4 Address Source::%x\n",wRet);
		return wRet;
	}

	wRet = LIBIPMI_HL_GetIPv4Address(pSession, Address, Channel, timeout);
	if (wRet != 0)
	{
		TCRIT ("LIBIPMI, Error getting IPv4 Address ::%x\n",wRet);
		return wRet;
	}

	wRet = LIBIPMI_HL_GetIPv4NetMask(pSession, Mask, Channel, timeout);
	if (wRet != 0)
	{
		TCRIT ("LIBIPMI, Error getting IPv4 Netmask ::%x\n",wRet);
		return wRet;
	}

	wRet = LIBIPMI_HL_GetIPv4Gateway(pSession, Gateway, Channel, timeout);
	if (wRet != 0)
	{
		TCRIT ("LIBIPMI, Error getting IPv4 Gateway ::%x\n",wRet);
		return wRet;
	}

	return 0;
}

uint16 LIBIPMI_HL_SetIPv4NetworkCfg(IPMI20_SESSION_T *pSession,
                                        INT8U *Address,
                                        INT8U *Mask,
                                        INT8U *Gateway,
                                        INT8U Channel,
                                        int timeout)
{
        uint16 wRet = 0;
        wRet = LIBIPMI_HL_SetIPv4Address(pSession, Address, Channel, timeout);
        if (wRet != 0)
        {
                TCRIT ("LIBIPMI, Error getting IPv4 Address ::%x\n",wRet);
                return wRet;
        }

        wRet = LIBIPMI_HL_SetIPv4NetMask(pSession, Mask, Channel, timeout);
        if (wRet != 0)
        {
                TCRIT ("LIBIPMI, Error getting IPv4 Netmask ::%x\n",wRet);
                return wRet;
        }

        wRet = LIBIPMI_HL_SetIPv4Gateway(pSession, Gateway, Channel, timeout);
        if (wRet != 0)
        {
                TCRIT ("LIBIPMI, Error getting IPv4 Gateway ::%x\n",wRet);
                return wRet;
        }

	return 0;
}

uint16 LIBIPMI_HL_GetIPv6NetworkCfg(IPMI20_SESSION_T *pSession,
					INT8U *Source,
					INT8U *Address,
					INT8U *Prefix,
					INT8U *Gateway,
					INT8U Channel,
					int timeout)
{
	uint16 wRet = 0;

	wRet = LIBIPMI_HL_GetIPv6Source(pSession, Source, Channel, timeout);
	if (wRet != 0)
	{
		TCRIT ("LIBIPMI, Error getting IPv6 Address Source::%x\n",wRet);
		return wRet;
	}

	wRet = LIBIPMI_HL_GetIPv6Address(pSession, Address, Channel, timeout);
	if (wRet != 0)
	{
		TCRIT ("LIBIPMI, Error getting IPv6 Address ::%x\n",wRet);
		return wRet;
	}

	wRet = LIBIPMI_HL_GetIPv6Prefix(pSession, Prefix, Channel, timeout);
	if (wRet != 0)
	{
		TCRIT ("LIBIPMI, Error getting IPv6 Prefix ::%x\n",wRet);
		return wRet;
	}

	wRet = LIBIPMI_HL_GetIPv6Gateway(pSession, Gateway, Channel, timeout);
	if (wRet != 0)
	{
		TCRIT ("LIBIPMI, Error getting IPv6 Gateway ::%x\n",wRet);
		return wRet;
	}

	return 0;
}

uint16 LIBIPMI_HL_Get_IPv6NetworkCfg(IPMI20_SESSION_T *pSession,
					INT8U *Source,
					INT8U *Address,
					INT8U *Prefix,
					INT8U Channel,
					int timeout,INT8U* v6Index)
{
	uint16 wRet = 0;

	wRet = LIBIPMI_HL_GetIPv6Source(pSession, Source, Channel, timeout);		//189575
	if (wRet != 0)
	{
		TCRIT ("LIBIPMI, Error getting IPv6 Address Source::%x\n",wRet);
		return wRet;
	}

	wRet = LIBIPMI_HL_GetI_Pv6Address(pSession, Address, Channel, timeout,v6Index);
	if (wRet != 0)
	{
		TCRIT ("LIBIPMI, Error getting IPv6 Address ::%x\n",wRet);
		return wRet;
	}

	wRet = LIBIPMI_HL_Get_IPv6Prefix(pSession, Prefix, Channel, timeout);
	if (wRet != 0)
	{
		TCRIT ("LIBIPMI, Error getting IPv6 Prefix ::%x\n",wRet);
		return wRet;
	}

	return 0;
}


uint16 LIBIPMI_HL_SetIPv6NetworkCfg(IPMI20_SESSION_T *pSession,
                                        INT8U *Address,
                                        INT8U Prefix,
                                        INT8U *Gateway,
                                        INT8U Channel,
                                        int timeout)
{
        uint16 wRet = 0;
        wRet = LIBIPMI_HL_SetIPv6Address(pSession, Address, Channel, timeout);
        if (wRet != 0)
        {
                TCRIT ("LIBIPMI, Error getting IPv6 Address ::%x\n",wRet);
                return wRet;
        }

        wRet = LIBIPMI_HL_SetIPv6Prefix(pSession, Prefix, Channel, timeout);
        if (wRet != 0)
        {
                TCRIT ("LIBIPMI, Error getting IPv6 Prefix ::%x\n",wRet);
                return wRet;
        }

        wRet = LIBIPMI_HL_SetIPv6Gateway(pSession, Gateway, Channel, timeout);
        if (wRet != 0)
        {
                TCRIT ("LIBIPMI, Error getting IPv6 Gateway ::%x\n",wRet);
                return wRet;
        }

	return 0;
}

uint16 LIBIPMI_HL_SetIPv6_NetworkCfg(IPMI20_SESSION_T *pSession,
                                        INT8U *Address,
                                        INT8U Prefix,
                                        INT8U Channel,
                                        int timeout,INT8U flag,INT8U v6Index)
{
        uint16 wRet = 0;
        wRet = LIBIPMI_HL_Set_IPv6Address(pSession, Address,Prefix,Channel, timeout,flag,v6Index);
        if (wRet != 0)
        {
                TCRIT ("LIBIPMI, Error setting IPv6 Address ::%x\n",wRet);
                return wRet;
        }

	return 0;
}



uint16 LIBIPMI_HL_SetLANDestinationType(IPMI20_SESSION_T *pSession,
					uint8 Channel,
					uint8  index,
					uint8 desttype,
					int timeout)
{
	uint16 wRet = 0;
	SetLanConfigReq_T SetLANReqBuff;
	SetLanConfigRes_T SetLANResBuff;
	unsigned int ReqDataLen = 0;

	memset(&(SetLANReqBuff),0,sizeof(SetLanConfigReq_T));
	SetLANReqBuff.ChannelNum = Channel;
	SetLANReqBuff.ParameterSelect = LAN_PARAM_SELECT_DEST_TYPE;

	SetLANReqBuff.ConfigData.DestType.SetSelect = index;
	SetLANReqBuff.ConfigData.DestType.DestType = desttype;
	SetLANReqBuff.ConfigData.DestType.AlertAckTimeout = 3;	//Default ACK Timeout value is 3seconds
	SetLANReqBuff.ConfigData.DestType.Retries = 3;

	ReqDataLen = 2 + sizeof(LANDestType_T);
	wRet = IPMICMD_SetLANConfig(pSession, &SetLANReqBuff, ReqDataLen,
					&SetLANResBuff, timeout);
	if(wRet != 0)
	{
		TCRIT("LIBIPMI_HL_SetLANDestinationType, Error setting LAN configuration\n");
		return wRet;
	}

	TDBG("Success setting LAN Destination type for index::%d\n",index);
	return wRet;
}

uint16 LIBIPMI_HL_SetLANDestinationAddress(IPMI20_SESSION_T *pSession,
					uint8 Channel,
					uint8  index,
					uint8 addrFormat,
					uint8* destAddr,
					int timeout)
{
	uint16 wRet = 0;
	SetLanConfigReq_T SetLANReqBuff;
	SetLanConfigRes_T SetLANResBuff;
	unsigned int ReqDataLen = 0;

	if(0)
	{
		addrFormat=addrFormat;/*-Wextra: Flag added for strict compilation.*/
	}

	memset(&(SetLANReqBuff),0,sizeof(SetLanConfigReq_T));
	SetLANReqBuff.ChannelNum = Channel;
	SetLANReqBuff.ParameterSelect = LAN_PARAM_SELECT_DEST_ADDR;

	SetLANReqBuff.ConfigData.DestAddr.SetSelect = index;
	//SetLANReqBuff.ConfigData.DestAddr.AddrFormat = addrFormat;
	memcpy(SetLANReqBuff.ConfigData.DestAddr.IPAddr, destAddr, IP_ADDR_LEN);

	ReqDataLen = 2 + sizeof(LANDestAddr_T);
	wRet = IPMICMD_SetLANConfig(pSession, &SetLANReqBuff, ReqDataLen,
					&SetLANResBuff, timeout);
	if(wRet != 0)
	{
		TCRIT("LIBIPMI_HL_SetLANDestinationAddress, Error setting LAN configuration\n");
		return wRet;
	}

	TDBG("Success setting LAN Destination Address for index::%d\n",index);
	return wRet;
}

uint16 LIBIPMI_HL_SetLANDestinationV6Address(IPMI20_SESSION_T *pSession,
					uint8 Channel,
					uint8  index,
					uint8 addrFormat,
					uint8* destAddr,
					int timeout)
{
	uint16 wRet = 0;
	SetLanConfigReq_T SetLANReqBuff;
	SetLanConfigRes_T SetLANResBuff;
	unsigned int ReqDataLen = 0;
	if(0)
	{
		addrFormat=addrFormat;  /*-Wextra: Flag added for strict compilation.*/
	}

	memset(&(SetLANReqBuff),0,sizeof(SetLanConfigReq_T));
	SetLANReqBuff.ChannelNum = Channel;
	SetLANReqBuff.ParameterSelect = GetLanOEMParamValue (LAN_PARAMS_AMI_OEM_SNMPV6_DEST_ADDR);

	SetLANReqBuff.ConfigData.Destv6Addr.SetSelect = index;
	//SetLANReqBuff.ConfigData.Destv6Addr.AddrFormat = addrFormat;
	memcpy(SetLANReqBuff.ConfigData.Destv6Addr.IPAddr, destAddr, IP6_ADDR_LEN);

	ReqDataLen = 2 + sizeof(LANDestv6Addr_T);
	wRet = IPMICMD_SetLANConfig(pSession, &SetLANReqBuff, ReqDataLen,
					&SetLANResBuff, timeout);
	if(wRet != 0)
	{
		TCRIT("LIBIPMI_HL_SetLANDestinationV6Address, Error setting LAN configuration\n");
		return wRet;
	}

	TDBG("Success setting LAN Destination V6 Address for index::%d\n",index);
	return wRet;
}


uint16 LIBIPMI_HL_SetLANAlertEntry(IPMI20_SESSION_T *pSession,
					uint8 DestSel,
					uint8* pDestAddr,uint8 Channel,
					int timeout)
{
	uint16 wRet;
	SetLanConfigReq_T SetLANReqBuff;
	SetLanConfigRes_T SetLANResBuff;
	unsigned int ReqDataLen = 0;

	memset(&(SetLANReqBuff),0,sizeof(SetLanConfigReq_T));

	SetLANReqBuff.ChannelNum = Channel;
	SetLANReqBuff.ParameterSelect = 19;

	SetLANReqBuff.ConfigData.DestAddr.SetSelect = DestSel;

	memcpy(SetLANReqBuff.ConfigData.DestAddr.IPAddr,pDestAddr,IP_ADDR_LEN);

	ReqDataLen = 2 + sizeof(LANDestAddr_T);
	wRet = IPMICMD_SetLANConfig(pSession,
					&SetLANReqBuff,
					ReqDataLen,
					&SetLANResBuff, timeout);
	if(wRet != 0)
	{
		TCRIT("Error setting LAN Alert destinations IP %d\n",wRet);
	}

	TDBG("Success setting LAN Alert destinations IP\n");
	return wRet;
}

uint16 LIBIPMI_HL_SetLANAlertEntry_IPv6(IPMI20_SESSION_T *pSession,
					uint8 DestSel,
					uint8* pDestAddr,uint8 Channel,
					int timeout)
{
	uint16 wRet;
	SetLanConfigReq_T SetLANReqBuff;
	SetLanConfigRes_T SetLANResBuff;
	unsigned int ReqDataLen = 0;

	memset(&(SetLANReqBuff),0,sizeof(SetLanConfigReq_T));

	SetLANReqBuff.ChannelNum = Channel;
	SetLANReqBuff.ParameterSelect = GetLanOEMParamValue (LAN_PARAMS_AMI_OEM_SNMPV6_DEST_ADDR);

	SetLANReqBuff.ConfigData.Destv6Addr.SetSelect = DestSel;

	memcpy(SetLANReqBuff.ConfigData.Destv6Addr.IPAddr,pDestAddr,IP6_ADDR_LEN);

	ReqDataLen = 2 + sizeof(LANDestv6Addr_T);
	wRet = IPMICMD_SetLANConfig(pSession,
					&SetLANReqBuff,
					ReqDataLen,
					&SetLANResBuff, timeout);
	if(wRet != 0)
	{
		TCRIT("Error setting LAN Alert destinations IP %d\n",wRet);
	}

	TDBG("Success setting LAN Alert destinations IP\n");
	return wRet;
}

uint16 IPMICMD_SetLANAlertEntryType(IPMI20_SESSION_T *pSession,
					uint8 DestSel,
					uint8 AlertType,uint8 Channel,
					int timeout)
{
	uint16 wRet;
	SetLanConfigReq_T SetLANReqBuff;
	SetLanConfigRes_T SetLANResBuff;
	unsigned int ReqDataLen = 0;

	memset(&(SetLANReqBuff),0,sizeof(SetLanConfigReq_T));

	SetLANReqBuff.ChannelNum = Channel;
	SetLANReqBuff.ParameterSelect = 18;

	SetLANReqBuff.ConfigData.DestType.AlertAckTimeout = 3;
	SetLANReqBuff.ConfigData.DestType.Retries = 3;
	SetLANReqBuff.ConfigData.DestType.SetSelect = DestSel;

	if(AlertType == 0)
	{
		/* SNMP  is Index 0 is for volatile */
		SetLANReqBuff.ConfigData.DestType.DestType = 0x00;
	}
	else
	{
		SetLANReqBuff.ConfigData.DestType.DestType = 0x06;
	}

	ReqDataLen = 2 + sizeof(LANDestType_T);
	wRet = IPMICMD_SetLANConfig(pSession,
					&SetLANReqBuff,
					ReqDataLen,
					&SetLANResBuff, timeout);
	if(wRet != 0)
	{
		TCRIT("Error setting LAN Alert Type %d\n",wRet);
	}

	TDBG("Success setting LAN Alert Type\n");
	return wRet;
}

uint16 IPMICMD_Set_IFACE_State( IPMI20_SESSION_T *pSession,
                             AMISetIfaceStateReq_T *pGetLANEnableReq,
                             unsigned int ReqDataLen,
                             AMISetIfaceStateRes_T *pGetLANEnableRes,
                             int timeout )
{
    uint16 wRet;
    uint32 dwResLen;
    dwResLen = sizeof(AMISetIfaceStateRes_T);

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                                                                            (NETFN_AMI << 2), CMD_AMI_SET_IFACE_STATE,
                                                                            (uint8*)pGetLANEnableReq, ReqDataLen,
                                                                            (uint8*)pGetLANEnableRes, &dwResLen,
                                                                            timeout);
    if(wRet != 0)
    {
    TCRIT("Error in Setting Interface state in raw IPMI command layer::%d\n", wRet);
    }
    return (wRet);
}

uint16 IPMICMD_Get_IFACE_State( IPMI20_SESSION_T *pSession,
                             AMIGetIfaceStateReq_T *pGetLANEnableReq,
                             AMIGetIfaceStateRes_T *pGetLANEnableRes,
                             int timeout )
{
    uint16 wRet;
    uint32 dwResLen;
    dwResLen = sizeof(AMIGetIfaceStateRes_T);

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                        (NETFN_AMI << 2), CMD_AMI_GET_IFACE_STATE,
                        (uint8*)pGetLANEnableReq, sizeof(AMIGetIfaceStateReq_T),
                        (uint8*)pGetLANEnableRes, &dwResLen,
                        timeout);
    if(wRet != 0)
    {
        TCRIT("Error in getting Interface state in raw IPMI command layer::%d\n", wRet);
    }
    return (wRet);
}

uint16 IPMICMD_Get_LAN_Enable( IPMI20_SESSION_T *pSession,
                             GetLanConfigReq_T *pGetLANEnableReq,
                             GetLanConfigRes_T *pGetLANEnableRes,
                             int timeout )
{
    uint16 wRet;
    uint32 dwResLen;
    uint8 NetFnLunXport = (NETFN_TRANSPORT << 2);
    dwResLen = sizeof(GetLanConfigRes_T);
     TINFO(" in ipmi cmd GetLanReq.ChannelNum=%d\n", pGetLANEnableReq->ChannelNum);
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                        (NetFnLunXport), CMD_GET_LAN_CONFIGURATION_PARAMETERS,
                        (uint8*)pGetLANEnableReq, sizeof(GetLanConfigReq_T),
                        (uint8*)pGetLANEnableRes, &dwResLen,
                        timeout);
    if(wRet != 0)
    {
        TCRIT("Error in getting Interface state in raw IPMI command layer::%d\n", wRet);
    }
    return (wRet);
}

uint16 LIBIPMI_HL_GetLANEnable(IPMI20_SESSION_T *pSession,
                                                                uint8 EthIndex, uint8 *LANEnable, int timeout)
{
    uint16 wRet;
    AMIGetIfaceStateReq_T GetLANEnableReqBuff;
    AMIGetIfaceStateRes_T GetLANEnableResBuff;

    memset(&(GetLANEnableReqBuff), 0, sizeof(GetLANEnableReqBuff));

    GetLANEnableReqBuff.SetSelect= EthIndex;
    GetLANEnableReqBuff.Params = AMI_IFACE_STATE_ETH;

    wRet = IPMICMD_Get_IFACE_State(pSession, &GetLANEnableReqBuff, &GetLANEnableResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("Error in getting LAN Enable in Libipmi layer ::%d\n", wRet);
    }

    *LANEnable = GetLANEnableResBuff.ConfigData.EthIface.EnableState;
    return wRet;
}


uint16 LIBIPMI_HL_Get_LANEnable(IPMI20_SESSION_T *pSession,
                                                                uint8 LanChannel, uint8 *LANEnable, int timeout)
{
    uint16 wRet;
   GetLanConfigReq_T  GetLanReq;
   GetLanConfigRes_T  GetLanRes ;

   memset(&(GetLanReq), 0, sizeof(GetLanReq));

    GetLanReq.SetSelect= 0;
    GetLanReq.ChannelNum=LanChannel;
    GetLanReq.BlockSelect=0;
    GetLanReq.ParameterSelect = LAN_PARAM_IPV6_IPV4_ADDRESS_ENABLE;

    wRet = IPMICMD_Get_LAN_Enable(pSession, &GetLanReq, &GetLanRes, timeout);
    if(wRet != 0)
    {
        TCRIT("Error in getting LAN Enable in Libipmi layer ::%d\n", wRet);
    }

    	switch((GetLanRes.ConfigData.IPv6IPv4AddrEnable ) & (0x3))
		{
			case DISABLE_V6:
				*LANEnable=0x1;
				break;
			case EABLE_V6_DISABLE_V4:
				*LANEnable=0x2;
			break;
			case ENABLE_V6_V4:
				*LANEnable=0x3;
			break;
		}
	return wRet;

}



uint16 LIBIPMI_HL_Get_V6Enable(IPMI20_SESSION_T *pSession,
                                                                uint8 EthIndex, uint8 *v6Enable, int timeout)
{
    uint16 wRet;
   GetLanConfigReq_T  GetLanReq;
   GetLanConfigRes_T  GetLanRes ;


    printf("############## Ethindex=%d\n",EthIndex);
    if((EthIndex==2) || (EthIndex==0))
    GetLanReq.ChannelNum=0x01;
    else if(EthIndex==1)
    GetLanReq.ChannelNum=0x08;
    else{}
    memset(&(GetLanReq), 0, sizeof(GetLanReq));

    GetLanReq.SetSelect= 0;
    GetLanReq.BlockSelect=0;
    GetLanReq.ParameterSelect = LAN_PARAM_IPV6_IPV4_ADDRESS_ENABLE;

    wRet = IPMICMD_Get_LAN_Enable(pSession, &GetLanReq, &GetLanRes, timeout);
    if(wRet != 0)
    {
        TCRIT("Error in getting LAN Enable in Libipmi layer ::%d\n", wRet);
    }

   printf("###################GetLanRes.ConfigData.IPv6IPv4AddrEnable =%d,EthIndex=%d\n",GetLanRes.ConfigData.IPv6IPv4AddrEnable,EthIndex);

     if(GetLanRes.ConfigData.IPv6IPv4AddrEnable == 0x01 || GetLanRes.ConfigData.IPv6IPv4AddrEnable==0x2)
    *v6Enable = 1;
     else
    *v6Enable = 0;
    return wRet;
}

uint16 LIBIPMI_HL_SetLANEnable(IPMI20_SESSION_T *pSession,
                                                                uint8 EthIndex, uint8 LANEnable, int timeout)
{
    uint16 wRet;
    AMISetIfaceStateReq_T GetLANEnableReqBuff;
    AMISetIfaceStateRes_T GetLANEnableResBuff;
    unsigned int ReqLanData=0;

    memset(&(GetLANEnableReqBuff), 0, sizeof(GetLANEnableReqBuff));

    GetLANEnableReqBuff.Params = AMI_IFACE_STATE_ETH;
    GetLANEnableReqBuff.ConfigData.EthIface.EthIndex = EthIndex;
    GetLANEnableReqBuff.ConfigData.EthIface.EnableState = LANEnable;

    ReqLanData = 3;
    wRet = IPMICMD_Set_IFACE_State(pSession, &GetLANEnableReqBuff, ReqLanData,&GetLANEnableResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("Error in Setting LAN Enable in Libipmi layer ::%d\n", wRet);
    }

    return wRet;
}

uint16 LIBIPMI_HL_GetBondEnable(IPMI20_SESSION_T * pSession,
                                                                    uint8 Index, uint8 * BondCfg, int timeout)
{
    uint16 wRet;
    AMIGetIfaceStateReq_T GetLANEnableReqBuff;
    AMIGetIfaceStateRes_T GetLANEnableResBuff;

    memset(&(GetLANEnableReqBuff), 0, sizeof(GetLANEnableReqBuff));

    GetLANEnableReqBuff.SetSelect= Index;
    GetLANEnableReqBuff.Params = AMI_IFACE_STATE_BOND;

    wRet = IPMICMD_Get_IFACE_State(pSession, &GetLANEnableReqBuff, &GetLANEnableResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("Error in getting GetBondEnable Command ::%d\n", wRet);
    }

    memcpy(BondCfg,&GetLANEnableResBuff.ConfigData.BondIface,sizeof(BondIface));
    return wRet;
}

uint16 LIBIPMI_HL_SetBondEnable(IPMI20_SESSION_T * pSession,
                                                               uint8 * BondCfg, int timeout)
{
    uint16 wRet;
    AMISetIfaceStateReq_T GetLANEnableReqBuff;
    AMISetIfaceStateRes_T GetLANEnableResBuff;
    unsigned int ReqLanData=0;

    memset(&(GetLANEnableReqBuff), 0, sizeof(GetLANEnableReqBuff));

    GetLANEnableReqBuff.Params = AMI_IFACE_STATE_BOND;

    memcpy(&GetLANEnableReqBuff.ConfigData.BondIface,BondCfg,sizeof(BondIface));
    ReqLanData = 1 + sizeof(BondIface);
    wRet = IPMICMD_Set_IFACE_State(pSession, &GetLANEnableReqBuff, ReqLanData,&GetLANEnableResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("Error in Setting SetBondEnable Command ::%d\n", wRet);
    }

    return wRet;
}

uint16 LIBIPMI_HL_GetActiveSlave(IPMI20_SESSION_T * pSession,
                                                                    uint8 Index, uint8 * Activeslave, int timeout)
{
    uint16 wRet;
    AMIGetIfaceStateReq_T GetLANEnableReqBuff;
    AMIGetIfaceStateRes_T GetLANEnableResBuff;

    memset(&(GetLANEnableReqBuff), 0, sizeof(GetLANEnableReqBuff));

    GetLANEnableReqBuff.SetSelect= Index;
    GetLANEnableReqBuff.Params = AMI_BOND_ACTIVE_SLAVE;

    wRet = IPMICMD_Get_IFACE_State(pSession, &GetLANEnableReqBuff, &GetLANEnableResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("Error in getting GetActiveSlave Command ::%d\n", wRet);
    }

    *Activeslave = GetLANEnableResBuff.ConfigData.ActiveSlave.ActiveIndex;
    return wRet;
}

uint16 LIBIPMI_HL_SetActiveSlave(IPMI20_SESSION_T * pSession,uint8 Index,
                                                               uint8 ActiveSlave, int timeout)
{
    uint16 wRet;
    AMISetIfaceStateReq_T GetLANEnableReqBuff;
    AMISetIfaceStateRes_T GetLANEnableResBuff;
    unsigned int ReqLanData=0;

    memset(&(GetLANEnableReqBuff), 0, sizeof(GetLANEnableReqBuff));

    GetLANEnableReqBuff.Params = AMI_BOND_ACTIVE_SLAVE;

    GetLANEnableReqBuff.ConfigData.ActiveSlave.BondIndex = Index;
    GetLANEnableReqBuff.ConfigData.ActiveSlave.ActiveIndex = ActiveSlave;
    ReqLanData = 1 + sizeof(ActiveSlave_T);
    wRet = IPMICMD_Set_IFACE_State(pSession, &GetLANEnableReqBuff, ReqLanData,&GetLANEnableResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("Error in Setting SetActiveSalve Command ::%d\n", wRet);
    }

    return wRet;
}

uint16 LIBIPMI_HL_IsBondEnabled(IPMI20_SESSION_T * pSession,
                                                                    uint8 * BondEnabled, int timeout)
{
    uint16 wRet;
    AMIGetIfaceStateReq_T GetLANEnableReqBuff;
    AMIGetIfaceStateRes_T GetLANEnableResBuff;

    memset(&(GetLANEnableReqBuff), 0, sizeof(GetLANEnableReqBuff));

    GetLANEnableReqBuff.SetSelect= 0;
    GetLANEnableReqBuff.Params = AMI_IFACE_BOND_ENABLED;

    wRet = IPMICMD_Get_IFACE_State(pSession, &GetLANEnableReqBuff, &GetLANEnableResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("Error in getting IsBondEnabled Command ::%d\n", wRet);
    }

    memcpy(BondEnabled,&GetLANEnableResBuff.ConfigData.BondEnable,sizeof(BondEnabled_T));
    return wRet;
}

uint16 LIBIPMI_HL_BondVLANEnabled(IPMI20_SESSION_T * pSession,
                                                                    uint8 * BondVLANEnabled, int timeout)
{
    uint16 wRet;
    AMIGetIfaceStateReq_T GetLANEnableReqBuff;
    AMIGetIfaceStateRes_T GetLANEnableResBuff;

    memset(&(GetLANEnableReqBuff), 0, sizeof(GetLANEnableReqBuff));

    GetLANEnableReqBuff.SetSelect= 0;
    GetLANEnableReqBuff.Params = AMI_BOND_VLAN_ENABLED;

    wRet = IPMICMD_Get_IFACE_State(pSession, &GetLANEnableReqBuff, &GetLANEnableResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("Error in getting BondVLANEnabled Command ::%d\n", wRet);
    }

    *BondVLANEnabled = GetLANEnableResBuff.ConfigData.BondVLAN.Enabled;
    return wRet;
}

uint16 LIBIPMI_HL_GetLANCount(IPMI20_SESSION_T * pSession,
                                                         uint8* LANCount,uint8 * LANIndex, int timeout)
{
    uint16 wRet;
    AMIGetIfaceStateReq_T GetLANEnableReqBuff;
    AMIGetIfaceStateRes_T GetLANEnableResBuff;

    memset(&(GetLANEnableReqBuff), 0, sizeof(GetLANEnableReqBuff));

    GetLANEnableReqBuff.SetSelect= 0;
    GetLANEnableReqBuff.Params = AMI_GET_IFACE_COUNT;

    wRet = IPMICMD_Get_IFACE_State(pSession, &GetLANEnableReqBuff, &GetLANEnableResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("Error in getting GetLANCount Command ::%d\n", wRet);
    }

    *LANCount=GetLANEnableResBuff.ConfigData.LANCount.Count;

    memcpy(LANIndex,&GetLANEnableResBuff.ConfigData.LANCount.EthIndex,sizeof(MAX_LAN_CHANNEL));
    return wRet;
}

uint16 LIBIPMI_HL_GetActiveLANCount(IPMI20_SESSION_T * pSession,
                                                         INT8U* LANCount,INT8U * LANIndex, int timeout)
{
    uint16 wRet;
    AMIGetIfaceStateReq_T GetLANEnableReqBuff;
    AMIGetIfaceStateRes_T GetLANEnableResBuff;

    memset(&(GetLANEnableReqBuff), 0, sizeof(GetLANEnableReqBuff));

    GetLANEnableReqBuff.SetSelect= 0;
    GetLANEnableReqBuff.Params = AMI_CURR_ACTIVE_IFACE_COUNT;

    wRet = IPMICMD_Get_IFACE_State(pSession, &GetLANEnableReqBuff, &GetLANEnableResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("Error in getting GetLANCount Command ::%d\n", wRet);
    }

    *LANCount=GetLANEnableResBuff.ConfigData.LANCount.Count;

    memcpy(LANIndex,&GetLANEnableResBuff.ConfigData.LANCount.EthIndex,sizeof(MAX_LAN_CHANNEL));
    return wRet;
}

uint16 LIBIPMI_HL_GetChannelNum(IPMI20_SESSION_T * pSession,
                                                                    uint8 Index, uint8 * Channel, int timeout)
{
    uint16 wRet;
    AMIGetIfaceStateReq_T GetLANEnableReqBuff;
    AMIGetIfaceStateRes_T GetLANEnableResBuff;

    memset(&(GetLANEnableReqBuff), 0, sizeof(GetLANEnableReqBuff));

    GetLANEnableReqBuff.SetSelect= Index;
    GetLANEnableReqBuff.Params = AMI_GET_IFACE_CHANNEL;

    wRet = IPMICMD_Get_IFACE_State(pSession, &GetLANEnableReqBuff, &GetLANEnableResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("Error in getting GetChannelNum Command ::%d\n", wRet);
    }

    memcpy(Channel,&GetLANEnableResBuff.ConfigData.IfcChannel,sizeof(GetIfcChannel_T));
    return wRet;
}

uint16 LIBIPMI_HL_GetLANIndex(IPMI20_SESSION_T * pSession,
                                                                    uint8 Index, uint8 * IfcName, int timeout)
{
    uint16 wRet;
    AMIGetIfaceStateReq_T GetLANEnableReqBuff;
    AMIGetIfaceStateRes_T GetLANEnableResBuff;

    memset(&(GetLANEnableReqBuff), 0, sizeof(GetLANEnableReqBuff));

    GetLANEnableReqBuff.SetSelect= Index;
    GetLANEnableReqBuff.Params = AMI_GET_IFACE_NAME;

    wRet = IPMICMD_Get_IFACE_State(pSession, &GetLANEnableReqBuff, &GetLANEnableResBuff, timeout);
    if(wRet != 0)
    {
        TCRIT("Error in getting GetLANIndex Command ::%d\n", wRet);
    }

    memcpy(IfcName,&GetLANEnableResBuff.ConfigData.IfcName,sizeof(GetIfcName_T));
    return wRet;
}

uint16 LIBIPMI_HL_GetSNMPCommunity(IPMI20_SESSION_T *pSession,
                    uint8* pCommunityStr,uint8 Channel,
					int timeout)
{
	uint16 wRet;
	GetLanConfigReq_T GetLANReqBuff;
	GetLanConfigRes_T GetLANResBuff;

	memset(&(GetLANReqBuff),0,sizeof(GetLanConfigReq_T));

	GetLANReqBuff.ChannelNum =Channel;
	GetLANReqBuff.ParameterSelect = 16;


	wRet = IPMICMD_GetLANConfig(pSession,
					&GetLANReqBuff,
                    &GetLANResBuff, timeout);
	if(wRet != 0)
	{
		TCRIT("Error getting snmp community string %d\n",wRet);
	}

	memcpy(pCommunityStr,GetLANResBuff.ConfigData.CommunityStr,MAX_COMM_STRING_SIZE);

	TDBG("Success getting SNMP community string\n");
	return wRet;
}


uint16 LIBIPMI_HL_SetSNMPCommunity(IPMI20_SESSION_T *pSession,
                    uint8* CommunityStr,uint8 Channel,
					int timeout)
{
	uint16 wRet;
	SetLanConfigReq_T SetLANReqBuff;
	SetLanConfigRes_T SetLANResBuff;
	unsigned int ReqDataLen = 0;

	memset(&(SetLANReqBuff),0,sizeof(SetLanConfigReq_T));

	SetLANReqBuff.ChannelNum = Channel;
	SetLANReqBuff.ParameterSelect = 16;

	memcpy(SetLANReqBuff.ConfigData.CommunityStr,CommunityStr,MAX_COMM_STRING_SIZE);


	ReqDataLen = 2 + MAX_COMM_STRING_SIZE;
	wRet = IPMICMD_SetLANConfig(pSession,
					&SetLANReqBuff,
					ReqDataLen,
					&SetLANResBuff, timeout);
	if(wRet != 0)
	{
		TCRIT("Error setting community string %d\n",wRet);
	}

	TDBG("Success setting community string\n");
	return wRet;
}

/**************************************************************************************************/
/* ****************************** Serial Port Setting Group Starts ********************************/
/**************************************************************************************************/
#ifdef TEMPORARILY_UNUSED
static void DumpBuffer(char *Buffer, int32 size)
{
    int32 i;
    int32 j;

    for (i=0;i<size;i++)
    {
        if ((!(i % 16))&& (i!=0))
        {
            printf("   ");
            for (j=i-16;j<i;j++)
            {
                if (isprint((unsigned char)Buffer[j]))
                    printf("%c",(unsigned char)Buffer[j]);
                else
                    printf(".");
            }
            printf("\n");
        }
	printf("%02X ",(unsigned char)Buffer[i]);
    }
    printf("   ");

    if (size%16)
    {
        for(j=0;j<(16-(size%16));j++)
            printf("   ");
    }

    for(j=(size-1)-((size-1)%16);j<size;j++)
    {
        if (isprint((unsigned char)Buffer[j]))
            printf("%c",(unsigned char)Buffer[j]);
        else
            printf(".");
    }
    printf("\n");
}
#endif


static int Map_ipmibaud_to_regular( INT8U baud, unsigned long *reg_baud )
{

    int retVal = 0;



    if ( baud == 0x06 )
	*reg_baud = 9600;
    else if ( baud == 0x07 )
	*reg_baud = 19200;
    else if ( baud == 0x08 )
	*reg_baud = 38400;
    else if ( baud == 0x09 )
	*reg_baud = 57600;
    else if ( baud == 0x0A )
	*reg_baud = 115200;
    else
	retVal = -1;



#if LIBIPMI_IS_OS_LINUX()
    TDBG("in function %s : baud rate is %ld\n",__FUNCTION__, *reg_baud );
#endif
    return retVal;
}


static int Sol_Map_Ipmi_Spec_To_User( SERIAL_STRUCT *serial, INT8U baud)
{
    unsigned long reg_baud=0;
    int retVal = 0;

    retVal = Map_ipmibaud_to_regular( baud, &reg_baud );

    if ( retVal == 0 ){

	serial->BaudRate = reg_baud;
	serial->DataBits = 8;
	serial->Parity = None_Parity;
	serial->StopBits = 1;
	serial->FlowControl = Hardware_FlowControl;
    }

    return retVal ;

}

static int map_ipmiflowControl_to_regular( uint8 i_flowcontrol, uint8 *flow )
{
    int retVal = 0 ;

    if ( i_flowcontrol == 0x00 )
	*flow = None_FlowControl;
    else if ( i_flowcontrol == 0x01 )
	*flow = Hardware_FlowControl;
    else if ( i_flowcontrol == 0x02 )
	*flow = XonXoff_FlowControl;
    else
	retVal = -1;

    return retVal;

}

static int Map_Ipmi_Spec_To_User( Serial_Port_Settings_T  *i_serial, SERIAL_STRUCT *serial )
{
    int retVal = 0 ;
    uint8 flowcontrol = 0;
    unsigned long baudrate = 0;

    do {

	retVal = map_ipmiflowControl_to_regular(  i_serial->Control.FlowControl, &flowcontrol );

	if ( retVal != 0 )
	    break;

	serial->FlowControl = flowcontrol;

	serial->StopBits = 1;

	serial->DataBits = 8;

	serial->Parity = None_Parity;

	if ( i_serial->Control.DtrHangup == 0 )
	    serial->DtrHangup = 0;
	else
	    serial->DtrHangup = 1;

	retVal =  Map_ipmibaud_to_regular( i_serial->BaudRate.BitRate, &baudrate );

	if ( retVal != 0 )
	    break;

	serial->BaudRate = baudrate;


    }while ( 0 );



    return retVal;
}


static int Map_UserValue_To_Ipmi_Spec( SERIAL_STRUCT *serial, Serial_Port_Settings_T *internal_serial )
{
    int retVal = 0;

    do{
	if ( serial->FlowControl == None_FlowControl )
    {
	    internal_serial->Control.FlowControl = 0x00;
    }
	else if ( serial->FlowControl == Hardware_FlowControl )
    {
	    internal_serial->Control.FlowControl = 0x01;
    }
	else if ( serial->FlowControl == XonXoff_FlowControl )
    {
	    internal_serial->Control.FlowControl = 0x02;
    }
	else{
	    retVal = -1;
	    break;
	}

	if ( serial->DtrHangup == 0x00)
	    internal_serial->Control.DtrHangup =0;
	else
	    internal_serial->Control.DtrHangup =1;

	if ( serial->BaudRate == 9600 )
	    internal_serial->BaudRate.BitRate = 0x06;
	else if ( serial->BaudRate == 19200 )
	    internal_serial->BaudRate.BitRate = 0x07;
	else if ( serial->BaudRate == 38400 )
	    internal_serial->BaudRate.BitRate = 0x08;
	else if ( serial->BaudRate == 57600 )
	    internal_serial->BaudRate.BitRate = 0x09;
	else if ( serial->BaudRate == 115200 )
	    internal_serial->BaudRate.BitRate = 0x0A;
	else
	{
	    retVal = -1;
	    break;
	}

    }while ( 0 );



    return retVal;
}



static uint16	IPMICMD_GetSerialCfg_Messaging( IPMI20_SESSION_T *pSession, Serial_Port_Settings_T *settings,\
				      int timeout )
{
    uint32		dwResLen=1 ;
    uint16		wRet;
    GetSerialModemConfigReq_T serialReq = {0,0,0,0};


    Wrapper_GetSerialModemConfigRes_T serialRes;

    uint8 NetFnLunXport = (NETFN_TRANSPORT << 2);

    serialReq.ParamRevChannelNo = 2;
    serialReq.ParamSel = 7;
    serialReq.SetSel = 0;
    serialReq.BlockSel = 0;

    dwResLen = sizeof( Wrapper_GetSerialModemConfigRes_T);


    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
					   NetFnLunXport,  CMD_GET_SERIAL_MODEM_CONFIG,
					    (uint8 *)&serialReq, sizeof(GetSerialModemConfigReq_T),
					    (uint8*)&serialRes, &dwResLen,
					    timeout);

    memcpy( settings,  & ( serialRes.serial) , sizeof(Serial_Port_Settings_T) );



    return wRet;
}

static uint16	IPMICMD_SetSerialCfg_Messaging( IPMI20_SESSION_T *pSession, Serial_Port_Settings_T *settings,\
				      int timeout )
{

    uint32		dwResLen=1 ;
    uint16		wRet;
    Wrapper_SetSerialModemConfigReq_T serialReq;
    uint8 Res[2] = { 0 } ;

    uint8 NetFnLunXport = (NETFN_TRANSPORT << 2);

    memset(&serialReq,0,sizeof(Wrapper_SetSerialModemConfigReq_T));

    serialReq.Req.ChannelNo = 2;
    serialReq.Req.ParamSel = 7;

    memcpy ( & (serialReq.serial), settings, sizeof(  Serial_Port_Settings_T ) );


	//DumpBuffer( ( char * ) &serialReq, sizeof( Wrapper_SetSerialModemConfigReq_T) );

    TDBG("Calling command \n");


    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
					    NetFnLunXport,  CMD_SET_SERIAL_MODEM_CONFIG,
					    (uint8 *)&serialReq, sizeof(Wrapper_SetSerialModemConfigReq_T),
					    (uint8*)Res, &dwResLen,
					    timeout);



    return wRet;
}




static uint16 IPMICMD_GetSerialCfg_SOL( IPMI20_SESSION_T *pSession, INT8U* baud,INT8U* Enabled,\
				      INT8U* ChannelNum,int timeout )
{
    uint32		dwResLen;
    uint16		wRet;
    GetSOLConfigReq_T SolConfigReq;
    //GetSOLConfigRes_T SolConfigRes;
     INT8U response[32]; //Quanta coverity
	uint8 NetFnLunXport = (NETFN_TRANSPORT << 2);

    SolConfigReq.ChannelNum = *ChannelNum;
    SolConfigReq.ParamSel = 0x05;
    SolConfigReq.SetSel = 0x00;
    SolConfigReq.BlkSEl = 0x00;

    dwResLen = sizeof ( GetSOLConfigRes_T )+1;

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
					    NetFnLunXport, CMD_GET_SOL_CONFIGURATION,
					    (uint8 *)&SolConfigReq,sizeof(GetSOLConfigReq_T),
//					    (uint8*)&SolConfigRes,  &dwResLen, //Quanta coverity
					    (uint8*)response,  &dwResLen, //Quanta coverity
					    timeout);

	//DumpBuffer( (char *) &SolConfigRes, 10 );
    if(wRet != 0)
    {
        TCRIT("Error getting sol config baud rate error is %x\n",wRet);
        return wRet;
    }


 //   *baud = * (((uint8*)(&SolConfigRes))+2); //Quanta coverity
    *baud = * (((uint8*)(response))+2); //Quanta coverity

    //get the enabled disable configuration
    SolConfigReq.ChannelNum = *ChannelNum;
    SolConfigReq.ParamSel = 0x01; //enable disable status
    SolConfigReq.SetSel = 0x00;
    SolConfigReq.BlkSEl = 0x00;

    dwResLen = sizeof ( GetSOLConfigRes_T );

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
					    NetFnLunXport, CMD_GET_SOL_CONFIGURATION,
					    (uint8 *)&SolConfigReq,sizeof(GetSOLConfigReq_T),
	//				    (uint8*)&SolConfigRes,  &dwResLen,  //Quanta coverity
					    (uint8*)response,  &dwResLen,   //Quanta coverity
					    timeout);

	//DumpBuffer( (char *) &SolConfigRes, 10 );

//    *Enabled = * (((uint8*)(&SolConfigRes))+2); //Quanta coverity
    *Enabled = * (((uint8*)(response))+2); //Quanta coverity



    return wRet;

}

uint16 IPMICMD_GetSerialCfg_SOL_Advanced( IPMI20_SESSION_T *pSession, INT8U* char_acc_interval,INT8U* char_send_threshold,int timeout )
{
    uint32		dwResLen;
    uint16		wRet;
    GetSOLConfigReq_T SolConfigReq;
   // GetSOLConfigRes_T SolConfigRes; //Quanta coverity
    INT8U response[32]; //Quanta coverity
    uint8 NetFnLunXport = (NETFN_TRANSPORT << 2);

    SolConfigReq.ChannelNum = 0x02;
    SolConfigReq.ParamSel = 0x03;
    SolConfigReq.SetSel = 0x00;
    SolConfigReq.BlkSEl = 0x00;

    dwResLen = sizeof ( GetSOLConfigRes_T )+1;

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
					    NetFnLunXport, CMD_GET_SOL_CONFIGURATION,
					    (uint8 *)&SolConfigReq,sizeof(GetSOLConfigReq_T),
//					    (uint8*)&SolConfigRes,  &dwResLen,  //Quanta coverity
					    (uint8*)response,  &dwResLen,  //Quanta coverity
					    timeout);


    if(wRet != 0)
    {
        TCRIT("Error getting sol config character acc and rate error is %x\n",wRet);
        return wRet;
    }

//	*char_acc_interval = * (((uint8*)(&SolConfigRes))+2);  //Quanta coverity
//	*char_send_threshold = * (((uint8*)(&SolConfigRes))+3); //Quanta coverity
    	*char_acc_interval = * (((uint8*)(response))+2); //Quanta coverity
    	*char_send_threshold = * (((uint8*)(response))+3); //Quanta coverity

    return wRet;

}


uint16 IPMICMD_SetSerialCfg_SOL_Advanced( IPMI20_SESSION_T *pSession, INT8U char_acc_interval,INT8U char_send_threshold,int timeout )
{
    uint32		dwResLen;
    uint16		wRet;
	uint8 tmp_buffer[16];
    SetSOLConfigReq_T* SolConfigReq = (SetSOLConfigReq_T *)tmp_buffer;
	uint8 Res[2] = { 0 } ;
    uint8 NetFnLunXport = (NETFN_TRANSPORT << 2);

    SolConfigReq->ChannelNum = 0x02;
    SolConfigReq->ParamSel = 0x03;
	*(tmp_buffer+sizeof(SetSOLConfigReq_T)) = char_acc_interval;
	*(tmp_buffer+sizeof(SetSOLConfigReq_T)+1) = char_send_threshold;





    dwResLen = 1;

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
					    NetFnLunXport, CMD_SET_SOL_CONFIGURATION,
					    (uint8 *)SolConfigReq,sizeof(SetSOLConfigReq_T)+2,
					    (uint8*)Res,  &dwResLen,
					    timeout);


    if(wRet != 0)
    {
        TCRIT("Error setting sol config character acc and rate error is %x\n",wRet);
        return wRet;
    }


    return wRet;

}

static uint16	IPMICMD_SetSerialConfig_SOL( IPMI20_SESSION_T *pSession, INT8U* baudrate, INT8U* Enabled,INT8U* ChannelNum,int timeout )
{

    uint32		dwResLen=1 ;
    uint16		wRet;
    char        tmp_buffer[5];
    SetSOLConfigReq_T* SolConfigReq = (SetSOLConfigReq_T*)tmp_buffer;
    uint8 Res[2] = { 0 } ;
    uint8 NetFnLunXport = (NETFN_TRANSPORT << 2);

    SolConfigReq->ChannelNum = *ChannelNum;
    SolConfigReq->ParamSel = 0x05;
    *(tmp_buffer+sizeof(SetSOLConfigReq_T)) = *baudrate;

	//send baud rate.

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
					   NetFnLunXport,  CMD_SET_SOL_CONFIGURATION,
					    (uint8 *)SolConfigReq,sizeof(SetSOLConfigReq_T)+1,
					    (uint8*)Res, &dwResLen,
					    timeout);

    if(wRet != 0)
    {
        TCRIT("Error setting SOL baud rate and error is %x!!\n",wRet);
        return wRet;
    }


    SolConfigReq->ChannelNum = *ChannelNum;
    SolConfigReq->ParamSel = 0x01;
    *(tmp_buffer+sizeof(SetSOLConfigReq_T)) = *Enabled;

    //send baud rate.

    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                       NetFnLunXport,  CMD_SET_SOL_CONFIGURATION,
                        (uint8 *)SolConfigReq,sizeof(SetSOLConfigReq_T)+1,
                        (uint8*)Res, &dwResLen,
                        timeout);

    if(wRet != 0)
    {
        TCRIT("Error setting SOL enalbe disable!!\n");
        return wRet;
    }


    return wRet;
}








uint16 LIBIPMI_HL_GetSerialCfg_Messaging(IPMI20_SESSION_T *pSession, \
					 SERIAL_STRUCT *serial, int timeout )
{
    uint16 wRet = 0;
    Serial_Port_Settings_T internal_serial;



    TDBG( "In regular serial port \n");
    wRet = IPMICMD_GetSerialCfg_Messaging( pSession, &internal_serial, timeout);

    if ( wRet != LIBIPMI_E_SUCCESS )
    {
        TCRIT("Unable to Get Serial Config Messaging.\n");
        return wRet;

    }



    wRet = Map_Ipmi_Spec_To_User( &internal_serial, serial );
    if ( wRet != 0 )
    {
        TINFO( "Error in traslating ipmi spec value to user defined value.\n");
        return wRet;
    }

    return wRet;
}

uint16 LIBIPMI_HL_SetSerialCfg_Messaging(IPMI20_SESSION_T *pSession, SERIAL_STRUCT *serial, int timeout)
{
    uint16	    wRet;
    Serial_Port_Settings_T internal_serial;

    memset(&internal_serial,0,sizeof(Serial_Port_Settings_T));

    TDBG( "In set serial messaging \n");
    wRet = Map_UserValue_To_Ipmi_Spec( serial, &internal_serial );

    if ( wRet != 0 )
    {
        TCRIT("Error in Trslating the user params to ipmi spect params.\n");
        wRet = -1;
        return wRet;

    }

    wRet = IPMICMD_SetSerialCfg_Messaging( pSession, &internal_serial, timeout);

    if ( wRet != 0 )
    {
        TCRIT("Error in Setting serial cfg for messaging.\n");
        return wRet;

    }

    return wRet;

}


uint16 LIBIPMI_HL_GetSerialCfg_SOL(IPMI20_SESSION_T *pSession, \
					 SERIAL_STRUCT *serial, INT8U* Enabled,INT8U* ChannelNum,int timeout )
{
    uint16 wRet = 0;

    INT8U baud = 0;


    wRet = IPMICMD_GetSerialCfg_SOL( pSession, &baud,Enabled,ChannelNum,timeout);

    if ( wRet != LIBIPMI_E_SUCCESS )
    {
        TCRIT("Unable to Get Serial Config Sol\n");
        return wRet;

    }



    wRet  = Sol_Map_Ipmi_Spec_To_User(  serial, baud );
    if ( wRet != 0 )
    {
		TINFO("Error in traslating ipmi spec value to user defined value.\n");
    }




    return wRet;
}


uint16 LIBIPMI_HL_SetSerialCfg_SOL(IPMI20_SESSION_T *pSession, SERIAL_STRUCT *serial, INT8U* Enabled, INT8U* ChannelNum,int timeout)
{
    uint16	    wRet;
    INT8U baud;



    if ( serial->BaudRate == 9600 )
		baud = 0x06;
	    else if ( serial->BaudRate == 19200 )
		baud = 0x07;
	    else if ( serial->BaudRate == 38400 )
		baud = 0x08;
	    else if ( serial->BaudRate == 57600 )
		baud = 0x09;
	    else if ( serial->BaudRate == 115200 )
		baud = 0x0A;
	    else
	    {
            wRet = -1;
            TINFO("BaudRate Not supported.\n");
            return wRet;
		}

	    wRet = IPMICMD_SetSerialConfig_SOL( pSession, &baud, Enabled,ChannelNum,timeout);

        if(wRet != 0)
        {
            TCRIT("Error setting serial config for SOL : %x\n",wRet);
            return wRet;
        }

        return wRet;
}


/**************************************************************************************************/
/* ****************************** Serial Port Setting Group Ends ********************************/
/**************************************************************************************************/
