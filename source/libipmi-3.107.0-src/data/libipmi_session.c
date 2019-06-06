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
* Filename: libipmi_session.c
*
* Description: Contains implementation of session establishment 
*   functions to BMC
*
* Author: Rajasekhar
*
******************************************************************/
#include <stdlib.h>
#include "libipmi_session.h"
#include "libipmi_rmcp.h"
#include "libipmi_IPM.h"
#include "libipmi_AMIOEM.h"
#include <string.h>
#include "dbgout.h"
#include "libipmi_sal_session.h"

#if LIBIPMI_IS_OS_LINUX()
#include "UDSIfc.h"
#endif

#define MAX_RETRIES	10
#define MAX_USERS 16
#define USB_TIMEOUT 20000
void *dl_salhandler = NULL;  //This handler will be used by SAL layer.



#if LIBIPMI_IS_OS_LINUX()
#define UDS_SOCKET_PATH "/var/UDSocket"
#define MAX_ENTRIES 23

extern int GetUserNameForUserID(IPMI20_SESSION_T *pSession,INT8U UserID,char *UserName,INT8U AuthEnable,int timeout);

extern int CheckUserPassword(IPMI20_SESSION_T *pSession, INT8U UserID, char *Password, INT8U AuthEnable, int timeout);

extern uint16 Create_IPMI20_UDS_Session(IPMI20_SESSION_T *pSession, char *pszSocketPath,
                     			char *pszUserName, char *pszPassword, uint8 *byPrivLevel, 
                     			char *pUserName, uint8 *ipaddr, int timeout, int AuthFlag,INT8U ChannelNum);

extern uint16 Create_IPMI20_SMBMC_UDS_Session(IPMI20_SESSION_T *pSession, char *pszSocketPath,
                     			char *pszUserName, char *pszPassword, uint8 *byPrivLevel, 
                     			char *pUserName, uint8 *ipaddr, int timeout, int AuthFlag,INT8U ChannelNum);

extern void UDS_Close(UDS_CHANNEL_T *UDSInfo );

extern uint16 Close_IPMI20_UDS_Session( IPMI20_SESSION_T *pSession );

int16 Send_RAW_IPMI2_0_UDS_Command(IPMI20_SESSION_T *pSession,
                                                uint8 byNetFnLUN, uint8 byCommand,
                                                uint8 *pszReqData, uint32 dwReqDataLen,
                                                uint8 *pszResData, uint32 *pdwResDataLen,
                                                INT8U AuthEnable,int timeout);

void *dl_crypto = NULL;
pcryptfunc g_CryptoHandle[MAX_CRYPTO_HANDLES];
#endif

/*** Network Session Establishing Functions ***/
extern uint16 Create_IPMI20_Network_Session(IPMI20_SESSION_T *pSession, char *pszIPAddress, uint16 wPort,
							char *pszUserName, char *pszPassword, uint8 bProtocolType,
							uint8 byMaxRetries,
							uint8 byAuthType,
							uint8 byEncryption, uint8 byIntegrity, uint8 *byPrivLevel,
							uint8 byReqAddr, uint8 byResAddr,
							uint8 *byKG, uint16 wKGLen,
							int timeout);
	/* byMaxRetries is not implemented for now. It will be implemented in future
		versions */

extern uint16 Send_RAW_IPMI2_0_Network_Command(IPMI20_SESSION_T *pSession, uint8 byPayLoadType,
										uint8 byNetFnLUN, uint8 byCommand,
										uint8 *pszReqData, uint32 dwReqDataLen,
										uint8 *pszResData, uint32 *pdwResDataLen,
										int	timeout);
extern uint16 Close_IPMI20_Network_Session( IPMI20_SESSION_T *pSession );

void DeallocateMemory(IPMI20_SESSION_T *pSession);
uint16 SetPrivilegeLevel(IPMI20_SESSION_T *pSession, uint8 *byPrivilegeLevel, int timeout);


/* Serial Medium API */
uint16 Create_IPMI20_Serial_Session(IPMI20_SESSION_T *pSession, char *pszDevice,
							char *pszUserName, char *pszPassword,
							uint8 byAuthType,
							uint8 *byPrivLevel,
							uint8 byReqAddr, uint8 byResAddr,
							int timeout);

uint16 Send_RAW_IPMI2_0_Serial_Command(IPMI20_SESSION_T *pSession,
										uint8 byNetFnLUN, uint8 byCommand,
										uint8 *pbyReqData, uint32 dwReqDataLen,
										uint8 *pbyResData, uint32 *pdwResDataLen,
										int	timeout);
uint16 Close_IPMI20_Serial_Session( IPMI20_SESSION_T *pSession );
uint8 CloseSerialPort();

/* KCS Medium API */
/*uint16 Create_IPMI20_KCS_Session(IPMI20_SESSION_T *pSession, char *pszDevice,
								 uint8	byResAddr);

uint16 Send_RAW_IPMI2_0_KCS_Command(IPMI20_SESSION_T *pSession,
										uint8 byNetFnLUN, uint8 byCommand,
										uint8 *pbyReqData, uint32 dwReqDataLen,
										uint8 *pbyResData, uint32 *pdwResDataLen,
										int	timeout);
uint16 Close_IPMI20_KCS_Session( IPMI20_SESSION_T *pSession );
uint8	CloseKCSDriver(IPMI20_SESSION_T *pSession);*/
uint16 ChangeSerialPortSettings(IPMI20_SESSION_T *pSession);


/***** IPMB Session Establishment***/

uint16 Create_IPMI20_IPMB_Session(IPMI20_SESSION_T *pSession, uint8 bySlaveAddr, uint8 bus);


uint16 Close_IPMI20_IPMB_Session( IPMI20_SESSION_T *pSession );

uint16 Send_RAW_IPMI2_0_IPMB_Command(IPMI20_SESSION_T *pSession,
										uint8 byNetFnLUN, uint8 byCommand,
										uint8 *pbyReqData, uint32 dwReqDataLen,
										uint8 *pbyResData, uint32 *pdwResDataLen,
										int	timeout);



/* USB Medium API */
uint16 Create_IPMI20_USB_Session(IPMI20_SESSION_T *pSession, char *pszDevice,
								 uint8	byResAddr);

uint16 Send_RAW_IPMI2_0_USB_Command(IPMI20_SESSION_T *pSession,
										uint8 byNetFnLUN, uint8 byCommand,
										uint8 *pbyReqData, uint32 dwReqDataLen,
										uint8 *pbyResData, uint32 *pdwResDataLen,
										int	timeout);
uint16 Close_IPMI20_USB_Session( IPMI20_SESSION_T *pSession );


#if LIBIPMI_IS_OS_LINUX()

Crypto_Init g_CryptoInitialize[] = 
{
	{CRYPTO_EVP_CIPHER_CTX_INIT,"EVP_CIPHER_CTX_init"},
	{CRYPTO_EVP_ENCRYPTINIT_EX,"EVP_EncryptInit_ex"},
	{CRYPTO_EVP_CIPHER_CTX_SET_PADDING,"EVP_CIPHER_CTX_set_padding"},
	{CRYPTO_EVP_ENCRYPTUPDATE,"EVP_EncryptUpdate"},
	{CRYPTO_EVP_ENCRYPTFINAL_EX,"EVP_EncryptFinal_ex"},
	{CRYPTO_EVP_CIPHER_CTX_CLEANUP,"EVP_CIPHER_CTX_cleanup"},
	{CRYPTO_EVP_DECRYPTINIT_EX,"EVP_DecryptInit_ex"},
	{CRYPTO_EVP_DECRYPTUPDATE,"EVP_DecryptUpdate"},
	{CRYPTO_EVP_DECRYPTFINAL_EX,"EVP_DecryptFinal_ex"},
	{CRYPTO_EVP_SHA1,"EVP_sha1"},
	{CRYPTO_EVP_AES_128_CBC,"EVP_aes_128_cbc"},
	{CRYPTO_HMAC,"HMAC"},
	{CRYPTO_SEED,"RAND_seed"},
	{CRYPTO_BYTE,"RAND_bytes"}
};


const char *crypto_tbl [MAX_ENTRIES] = 
{
	"/usr/lib/x86_64-linux-gnu/libcrypto.so", /* Debian 7.1, ubuntu 12.04 - 64 */
	"/usr/lib/i386-linux-gnu/libcrypto.so",
	"/usr/lib/libcrypto.so", /*FC 12 -32 */
	"/lib/libcrypto.so.4", /* CentOS 4.8 -32 */
	"/lib/libcrypto.so.6",  /* CentOS 5.5,5.10  -32*/
	"/lib64/libcrypto.so.0.9.7a", /* CentOS 4.8  -64*/
	"/lib64/libcrypto.so.6", /* CentOS 5.5,5.10  -64*/
	"/usr/lib/libcrypto.so.10",  /* CentOS 6.0,6.3,6.4,6.5  -32,Oracle 6.4,6.2*/
	"/usr/lib64/libcrypto.so.10", /* CentOS 6.0,6.3,6.4,6.5  -64*/
	"/usr/lib/libcrypto.so.0.9.8e", /* RHEL 5.4 - 32, Debian 6, SUSE 11.2*/
	"/usr/lib64/libcrypto.so.0.9.8e",  /* RHEL 5.4 - 64, SUSE 11.2  */
	"/lib/libcrypto.so.0.9.8e",  /* RHEL 5.6 - 32 */
	"/lib64/libcrypto.so.0.9.8e",  /* RHEL 5.6 - 64 */
	"/usr/lib/libcrypto.so.1.0.0",  /* RHEL 6.0,6.3 - 32, SUSE 11.4, Ubuntu 10.04 */
	"/usr/lib64/libcrypto.so.1.0.0",  /*RHEL 6.0,6.3 - 64, Oracle 6.4 */
	"/usr/lib64/libcrypto.so.6 ", /*Scientific Linux */
	"/usr/lib/libcrypto.so.1.0.0b", /*Fedora 12 -32 */
	"/lib/libcrypto.so.1.0.0j", /*Fedora 15 - 32 */
	"/usr/lib/libcrypto.so.1.0.0d", /* Fedora 13 - 64*/
	"/usr/lib/libcrypto.so.0.9.8", /* Ubuntu 10.04,11.04 -32*/
	"/usr/lib/libcrypto.so.8", /* Fedora 11-32*/
	"/usr/lib/arm-linux-gnueabi/libcrypto.so", /* Added for BMC */
	"/usr/lib64/libcrypto.so.0.9.8" /* SUSE 11 64-bit  */
};
int InitializeCryptolibrary()
{
	int i=0;
	for(i=0;i<MAX_ENTRIES;i++)
	{
		dl_crypto = dlopen(crypto_tbl[i],RTLD_NOW);
		if(dl_crypto !=NULL)
		{
			break;
		}
		
	}
	if(dl_crypto != NULL)
	{
		for(i=0;i<MAX_CRYPTO_HANDLES;i++)
		{
			g_CryptoHandle[g_CryptoInitialize[i].CryptofunNum] = dlsym(dl_crypto,(char*)g_CryptoInitialize[i].CryptofunName);
			if(g_CryptoHandle[g_CryptoInitialize[i].CryptofunNum] == NULL)
			{
				return LIBIPMI_MEDIUM_E_UNSUPPORTED;
			}
		}
	}
	else
	{
		return LIBIPMI_MEDIUM_E_UNSUPPORTED;
	}
	
	return LIBIPMI_E_SUCCESS;
}
#endif

/*** Session Establishment functions ***/
/*---------------------------------------------------------------------
LIBIPMI_Create_IPMI20_Session

Establishes a connection with BMC
----------------------------------------------------------------------*/
uint16 LIBIPMI_Create_IPMI20_Session(IPMI20_SESSION_T *pSession, char *pszIPAddress, uint16 wPort,
							char *pszUserName, char *pszPassword,
							uint8 byAuthType,
							uint8 byEncryption, uint8 byIntegrity, uint8 *byPrivLevel,
							uint8 byMedium,
							uint8 byReqAddr, uint8 byResAddr,
							uint8 *pbyKG, uint16 wKGLen,
							int timeout)
{

	uint16			wRet;
	uint8 			retries = MAX_RETRIES;
	GetDevIDRes_T		GetDevIDRes;
	pSession->g_NetFnOEM = 0xFF ;
	uint8			pKG [] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};  //Quanta

/*	We should not memset the IPMI20_SESSION_T . As we have to  memset while malloc itself */
	//memset(pSession, 0, sizeof(IPMI20_SESSION_T) - sizeof(LIBIPMI_SERIAL_SETTINGS_T) );

CreateIPMISession:

	/* For Network Medium */
	if(byMedium == NETWORK_MEDIUM_TCP || 
		byMedium == NETWORK_MEDIUM_UDP)
	{
//		uint8			pKG [] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};

		/* Use default KG key if nothing is specified */
		if(pbyKG == NULL)
		{
			pbyKG  = &pKG[0];
			wKGLen = sizeof(pKG);
		}

#if LIBIPMI_IS_OS_LINUX()
		wRet = InitializeCryptolibrary();
		if(wRet != 0)
		{
			return wRet;
		}
#endif
		wRet = Create_IPMI20_Network_Session(pSession, pszIPAddress, wPort,
										pszUserName, pszPassword, byMedium, 0,
										byAuthType,
										byEncryption, byIntegrity, byPrivLevel,
										byReqAddr, byResAddr,
										pbyKG, wKGLen,
										timeout);
	}
	else if(byMedium == SERIAL_MEDIUM)
	{
		wRet = Create_IPMI20_Serial_Session(pSession, pszIPAddress,
										pszUserName, pszPassword,
										byAuthType,
										byPrivLevel,
										byReqAddr, byResAddr,
										timeout);
	}
	/*else if(byMedium == KCS_MEDIUM)
	{

		wRet = Create_IPMI20_KCS_Session(pSession, pszIPAddress,
											byResAddr);
	}*/
	else if(byMedium == USB_MEDIUM)
	{
		wRet = Create_IPMI20_USB_Session(pSession, pszIPAddress,
											byResAddr);
	}
#if LIBIPMI_IS_OS_LINUX() && (LIBIPMI_IS_PLATFORM_PPC() || LIBIPMI_IS_PLATFORM_ARM() )
	else if(byMedium == IPMB_MEDIUM)
	{
		uint8 bus_num = byReqAddr; // Bus number is passed using this parameter if it is a IPMB session
		wRet = Create_IPMI20_IPMB_Session(pSession, byResAddr,bus_num);
	}
#endif
	else
	{
		wRet = STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_MEDIUM_E_UNSUPPORTED);
	}

	if(wRet != LIBIPMI_E_SUCCESS)
	{
		DeallocateMemory(pSession);
		if((byMedium == NETWORK_MEDIUM_UDP) && (retries))
		{
			TWARN("Retrying over UDP connection...\n");
			retries--;
			goto CreateIPMISession;
		}
		return wRet;
	}

	if((pSession->byMediumType != IPMB_MEDIUM) && (pSession->byMediumType != USB_MEDIUM))
	{
		/* Set the privilige level */
		wRet = SetPrivilegeLevel(pSession, byPrivLevel, timeout);
		if(byMedium == NETWORK_MEDIUM_TCP || 
			byMedium == NETWORK_MEDIUM_UDP)
		{
			//set the role member to current priv level
			pSession->hNetworkSession->byRole = *byPrivLevel;
		}
	}
	else
	{
		//if it is a IPMB medium try doing a getdeviceid to check that the BMC exists and is responding
		wRet = IPMICMD_GetDeviceID( pSession, &GetDevIDRes, 3000);
		if(wRet != 0)
		{
			TCRIT("Could not communicate with BMC via IPMB when creating session error code %d!!\n",wRet);
			DeallocateMemory(pSession);
			return wRet;
			
		}
	}

	return wRet;
}

#if LIBIPMI_IS_OS_LINUX()
uint16 LIBIPMI_Create_IPMI_Local_Session(IPMI20_SESSION_T *pSession,
                                                    char *pszUserName, char *pszPassword,
                                                    uint8 *byPrivLevel,INT8U *ServIPAddr,INT8U AuthFlag,int timeout)
{
    uint16 wRet;
    INT8U ChannelNum,BMCInst = 0;
    char SocketPath[SOCKET_PATH_LEN]={0};
    INT8U LoopBackIP[IP4_ADDR_LEN]={127,0,0,1};

    if((NULL != ServIPAddr) && (0 != memcmp(ServIPAddr,LoopBackIP,IP4_ADDR_LEN)))
    {
        /* Get the Channel Type,Channel Number and BMC Instance to Communicate if Server IP Address is given */
        wRet = LIBIPMI_HL_AMIGetUDSInfo(ServIPAddr,NULL,&ChannelNum,&BMCInst,timeout);
        if(CC_SUCCESS != wRet)
        {
            TCRIT("Cannot get the Channel Number,Channel Type and BMC Instance to Communicate\n");
            return wRet;
        }
    }
    else
    {
        /* BMC Instance is assumed to be one and Channel Number is assumed to be as 0x0D(Reserved IPMI Channel
                * Number).The BMC Instance needs to be a constant value in MBMC Case to get the details of other BMC Channel
                * to communicate*/
        BMCInst = 1;
        ChannelNum = UDS_CHANNEL;
    }

    sprintf(SocketPath,"%s%d",UDS_SOCKET_PATH,BMCInst);

    wRet = Create_IPMI20_UDS_Session(pSession,&SocketPath[0], pszUserName, pszPassword,
                                    	byPrivLevel, NULL, NULL, timeout, AuthFlag,ChannelNum);

    if(wRet != LIBIPMI_E_SUCCESS)
    {
        DeallocateMemory(pSession);
        return wRet;
    }

    return wRet;
}

uint16 LIBIPMI_Create_IPMI_Local_Session_Ext(IPMI20_SESSION_T *pSession, char *pszUserName,
                                               char *pszPassword, uint8 *byPrivLevel,
                                               char *xtUserName, INT8U *ipaddr, INT8U IPVer,INT8U *ServIPAddr, INT8U AuthFlag,int timeout)
{
    uint16 wRet;
    INT8U ChannelNum,BMCInst = 0;
    char SocketPath[SOCKET_PATH_LEN]={0};
    INT8U LoopBackIP[IP4_ADDR_LEN]={127,0,0,1};

    /* Map IP4 Address into IP6 Address */
    
    INT8U IPAddr[IP6_ADDR_LEN] = { 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xFF, 0xFF, 0x7F, 0x00, 0x00, 0x01
    };
    
    if(NULL != ipaddr)
    {
        if(IP4_VERSION == IPVer)
        {
            memcpy(&IPAddr[IP6_ADDR_LEN - IP4_ADDR_LEN], ipaddr, IP4_ADDR_LEN);
        }
        else
        {
            memcpy(IPAddr, ipaddr, IP6_ADDR_LEN);
        }
    }

    if((NULL != ServIPAddr) && (0 != memcmp(ServIPAddr,LoopBackIP,IP4_ADDR_LEN)))
    {
        /* Get the Channel Type,Channel Number and BMC Instance to Communicate if Server IP Address is given */
        wRet = LIBIPMI_HL_AMIGetUDSInfo(ServIPAddr,NULL,&ChannelNum,&BMCInst,timeout);
        if(CC_SUCCESS != wRet)
        {
            TCRIT("Cannot get the channel number,channel Type and BMC Instance to Communicate\n");
            return wRet;
        }
    }
    else
    {
        /* BMC Instance is assumed to be one and Channel Number is assumed to be as 0x0D(Reserved IPMI Channel
                * Number).The BMC Instance needs to be a constant value in MBMC Case to get the details of other BMC Channel
                * to communicate*/
        BMCInst = 1;
        ChannelNum = UDS_CHANNEL;
    }

    sprintf(SocketPath,"%s%d",UDS_SOCKET_PATH,BMCInst);

    wRet = Create_IPMI20_UDS_Session(pSession,&SocketPath[0], pszUserName, pszPassword,
                                    	byPrivLevel, xtUserName, IPAddr, timeout, AuthFlag,ChannelNum);

    if(wRet != LIBIPMI_E_SUCCESS)
    {
        DeallocateMemory(pSession);
        return wRet;
    }

    return wRet;
}

uint16 LIBIPMI_Create_IPMI_Local_Session_SMBMC(IPMI20_SESSION_T *pSession, char *pszUserName,
                                               char *pszPassword, uint8 *byPrivLevel,
                                               char *xtUserName, INT8U *ipaddr, INT8U IPVer, INT8U AuthFlag, INT8U BMCInst, int timeout)
{
    uint16 wRet;
    INT8U ChannelNum = 0;
    char SocketPath[SOCKET_PATH_LEN]={0};
    INT8U IPAddr[IP6_ADDR_LEN] = { 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xFF, 0xFF, 0x7F, 0x00, 0x00, 0x01
    };
    
    if(NULL != ipaddr)
    {
        if(IP4_VERSION == IPVer)
        {
            memcpy(&IPAddr[IP6_ADDR_LEN - IP4_ADDR_LEN], ipaddr, IP4_ADDR_LEN);
        }
        else
        {
            memcpy(IPAddr, ipaddr, IP6_ADDR_LEN);
        }
    }

    ChannelNum = UDS_SMBMC_CHANNEL;			/*UDS_SMBMC_CHANNEL*/

    sprintf(SocketPath,"%s%d",UDS_SOCKET_PATH,BMCInst);

    wRet = Create_IPMI20_SMBMC_UDS_Session(pSession,&SocketPath[0], pszUserName, pszPassword,
                                    	byPrivLevel, xtUserName, IPAddr, timeout, AuthFlag,ChannelNum);

    return wRet;
}

#endif

/*---------------------------------------------------------------------
LIBIPMI_Send_RAW_IPMI2_0_Command

Sends RAW IPMI Command to BMC
----------------------------------------------------------------------*/
uint16 LIBIPMI_Send_RAW_IPMI2_0_Command(IPMI20_SESSION_T *pSession,
									uint8 byPayLoadType,
									uint8 byNetFnLUN, uint8 byCommand,
									uint8 *pszReqData, uint32 dwReqDataLen,
									uint8 *pszResData, uint32 *pdwResDataLen,
									int	timeout)
{
	uint16 wRet;
	uint8 retries = MAX_RETRIES;

	static int ( *dl_ProcessSalSession )( IPMI20_SESSION_SAL_T*,
                                          uint8 ,
                                          uint8 ,
                                          uint8 ,
                                          uint8*,
                                          uint32,
                                          uint8*,
                                          uint32*,
                                          int)=NULL;

	if(pSession)
	{
		if(pSession->bySessionStarted == SESSION_NOT_STARTED)
		{
			TCRIT("Attempt to send command on unstarted session.The socket may have possible timed out on the stack end\n");
			return STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_SESSION_E_EXPIRED);
		}

	
		if(pSession->byMediumType == NETWORK_MEDIUM_TCP || 
			pSession->byMediumType == NETWORK_MEDIUM_UDP)
		{
SendNetworkCmd:
                     pSession->hNetworkSession->dwSessionSeqNum++;
			
			wRet =  Send_RAW_IPMI2_0_Network_Command(pSession, byPayLoadType,
												byNetFnLUN, byCommand,
												pszReqData, dwReqDataLen,
												pszResData, pdwResDataLen,
												timeout);
			if(pSession->byMediumType == NETWORK_MEDIUM_TCP)
			{	
													
				if( 
				(wRet == LIBIPMI_MEDIUM_E_INVALID_SOCKET) || 
				(wRet == LIBIPMI_MEDIUM_E_TIMED_OUT_ON_SEND) ||
				(wRet == LIBIPMI_MEDIUM_E_SEND_DATA_FAILURE) ||
				(wRet == LIBIPMI_MEDIUM_E_RECV_DATA_FAILURE)
				)
				{
					TWARN("Encountered what may be socket closure..will close session here\n");
					LIBIPMI_CloseSession(pSession);
					return wRet;
					
				}
			 }
			else if (pSession->byMediumType == NETWORK_MEDIUM_UDP)
			{	
													
				if((wRet != LIBIPMI_E_SUCCESS) && (retries))
				{
					TWARN("Retrying sending the command...\n");
					retries--;
					goto SendNetworkCmd;
				}
			}	
			
			return wRet;

		}
		else if(pSession->byMediumType == SERIAL_MEDIUM)
		{
			return Send_RAW_IPMI2_0_Serial_Command(pSession,
												byNetFnLUN, byCommand,
												pszReqData, dwReqDataLen,
												pszResData, pdwResDataLen,
												timeout);
		}
	/*	else if(pSession->byMediumType == KCS_MEDIUM)
		{
			return Send_RAW_IPMI2_0_KCS_Command(pSession,
												byNetFnLUN, byCommand,
												pszReqData, dwReqDataLen,
												pszResData, pdwResDataLen,
												timeout);
		}*/
		else if(pSession->byMediumType == USB_MEDIUM)
		{
			return Send_RAW_IPMI2_0_USB_Command(pSession,
												byNetFnLUN, byCommand,
												pszReqData, dwReqDataLen,
												pszResData, pdwResDataLen,
												USB_TIMEOUT);
		}
		else if(pSession->byMediumType == UDS_MEDIUM)
		{
#if LIBIPMI_IS_OS_LINUX()
		   wRet =  Send_RAW_IPMI2_0_UDS_Command(pSession,
												byNetFnLUN, byCommand,
												pszReqData, dwReqDataLen,
												pszResData,pdwResDataLen,
												 0,timeout);
		   if(
				(wRet == LIBIPMI_MEDIUM_E_INVALID_SOCKET) ||
				(wRet == LIBIPMI_MEDIUM_E_TIMED_OUT_ON_SEND) ||
				(wRet == LIBIPMI_MEDIUM_E_SEND_DATA_FAILURE) ||
				(wRet == LIBIPMI_MEDIUM_E_RECV_DATA_FAILURE)
				)
				{
					TWARN("Encountered what may be socket closure in UDS..will close session here\n");
					LIBIPMI_CloseSession(pSession);
					return wRet;
		   	      }
		      return wRet;
#endif
		}
#if LIBIPMI_IS_OS_LINUX() && (LIBIPMI_IS_PLATFORM_PPC() || LIBIPMI_IS_PLATFORM_ARM() )
		else if(pSession->byMediumType == IPMB_MEDIUM)
		{
			return Send_RAW_IPMI2_0_IPMB_Command(pSession,
												byNetFnLUN, byCommand,
												pszReqData, dwReqDataLen,
												pszResData, pdwResDataLen,
												timeout);
		}
#endif
		else if((pSession->byMediumType ) == (NETWORK_MEDIUM_UDP_SAL))
		{
#if LIBIPMI_IS_OS_LINUX()
			IPMI20_SESSION_SAL_T *Sal_Session=(IPMI20_SESSION_SAL_T *)pSession;

			uint32	responseLen = *pdwResDataLen;


			if(pSession->bySessionStarted == SAL_SESSION_STARTED)
			{

		retrySALPkt:

				*pdwResDataLen = responseLen;

				if(dl_salhandler==NULL)
				{
					dl_salhandler= dlopen (CMM_SAL_LIB_PATH,RTLD_LAZY);
					if(NULL == dl_salhandler)
					{
						TCRIT("Error in loading libcmmsal.so library %s\n", dlerror() );
						return -1;
					}

					dl_ProcessSalSession = dlsym (dl_salhandler, "Send_RAW_IPMI2_0_SAL_SessionLessPkt_Command");
					if ( NULL == dl_ProcessSalSession)
					{
						TCRIT("Error in getting symbol %s \n", dlerror());
						return -1;
					}
				}
				wRet=dl_ProcessSalSession( (IPMI20_SESSION_SAL_T*)Sal_Session,
											byPayLoadType ,
											byNetFnLUN ,
											byCommand ,
											(uint8*)pszReqData,
											dwReqDataLen,
											(uint8*)pszResData,
											(uint32*)pdwResDataLen,
											timeout);
				if(IS_IPMI_ERROR(wRet) && (CC_NODE_BUSY == GET_ERROR_CODE(wRet	)) && (retries))
				{
					retries--;
					sleep (CMD_RETRY_INTERVAL);
					goto retrySALPkt;
				}
				return wRet;
			}
#endif
		}
		else
		{
			return STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_MEDIUM_E_UNSUPPORTED);
		}
	}

	return STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_SESSION_E_EXPIRED);
}

/*---------------------------------------------------------------------
LIBIPMI_CloseSession

Closes the connection with BMC
----------------------------------------------------------------------*/
uint16 LIBIPMI_CloseSession( IPMI20_SESSION_T *pSession )
{
	if(pSession)
	{
		if(pSession->byMediumType == NETWORK_MEDIUM_TCP || 
			pSession->byMediumType == NETWORK_MEDIUM_UDP)
		{
			uint16	wRet;

			wRet = Close_IPMI20_Network_Session(pSession);
			
			DeallocateMemory(pSession);
#if LIBIPMI_IS_OS_LINUX()
			if(dl_crypto != NULL)
			{
				dlclose(dl_crypto);
			}
#endif
			return wRet;
		}
		else if(pSession->byMediumType == SERIAL_MEDIUM)
		{
			uint16	wRet;

			wRet = Close_IPMI20_Serial_Session(pSession);
			
			DeallocateMemory(pSession);
			return wRet;
		}
	/*	else if(pSession->byMediumType == KCS_MEDIUM)
		{
			uint16	wRet;

			wRet = Close_IPMI20_KCS_Session(pSession);
			
			DeallocateMemory(pSession);
			return wRet;
		}*/
		else if(pSession->byMediumType == USB_MEDIUM)
		{
			uint16	wRet;

			wRet = Close_IPMI20_USB_Session(pSession);
			
			DeallocateMemory(pSession);
			return wRet;
		}
		else if(pSession->byMediumType == UDS_MEDIUM)
		{
#if LIBIPMI_IS_OS_LINUX()
            uint16 wRet;

            wRet = Close_IPMI20_UDS_Session(pSession);

            DeallocateMemory(pSession);
            return wRet;
#endif 
		}	 
#if LIBIPMI_IS_OS_LINUX() && (LIBIPMI_IS_PLATFORM_PPC() || LIBIPMI_IS_PLATFORM_ARM() )
		else if(pSession->byMediumType == IPMB_MEDIUM)
		{
			uint16	wRet;

			wRet = Close_IPMI20_IPMB_Session(pSession);
			
			DeallocateMemory(pSession);
			return wRet;
		}
#endif
		else if(pSession->byMediumType == NETWORK_MEDIUM_UDP_SAL)
		{
#if LIBIPMI_IS_OS_LINUX()
			if(dl_salhandler)
			{
				TDBG("Closing sal session\n");
				dlclose(dl_salhandler);
				dl_salhandler = NULL;
			}
#endif
		}
		else
		{
			return STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_MEDIUM_E_UNSUPPORTED);
		}
	}

	return STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_SESSION_E_EXPIRED);
}

/*---------------------------------------------------------------------
MediumUsed

Returns the medium used to connect to BMC
----------------------------------------------------------------------*/
uint8	MediumUsed(IPMI20_SESSION_T *pSession)
{
	return pSession->byMediumType;
}

/*---------------------------------------------------------------------
IsSessionStarted

Returns if the session has started
----------------------------------------------------------------------*/
uint8	IsSessionStarted(IPMI20_SESSION_T *pSession)
{
	return pSession->bySessionStarted;
}

/*---------------------------------------------------------------------
SetPrivilegeLevel

Sets the session privilege to specified level
----------------------------------------------------------------------*/
uint16 SetPrivilegeLevel(IPMI20_SESSION_T *pSession, uint8 *byPrivLevel, int timeout)
{
	uint8			byReq;
	uint8			pbyRes[2];
	uint32			dwResLen;
	uint16			wRet;


	byReq = *byPrivLevel;
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command (pSession, PAYLOAD_TYPE_IPMI, DEFAULT_NET_FN_LUN,
												CMD_SET_SESSION_PRIV_LEVEL,
												&byReq, sizeof(uint8),
												&pbyRes[0], &dwResLen, timeout);


	if(wRet != 0)
	{
		TDBG("Originally requested priv level failed\n");
		//find the current privilege level allowed
		byReq = 0;
		wRet = LIBIPMI_Send_RAW_IPMI2_0_Command (pSession, PAYLOAD_TYPE_IPMI, DEFAULT_NET_FN_LUN,
												CMD_SET_SESSION_PRIV_LEVEL,
												&byReq, sizeof(uint8),
												&pbyRes[0], &dwResLen, timeout);
		if(wRet != 0)
		{
			TDBG("Amazing..request for current privilege limit returned an error %x2\n",wRet);
		}
		else
		{
			TDBG("New priv level is %x\n",pbyRes[1]);
			*byPrivLevel = pbyRes[1];
		}
	}

	return wRet;
}

/*---------------------------------------------------------------------
DeallocateMemory

Deallocates the memory which was allocated during session establishment
----------------------------------------------------------------------*/
void DeallocateMemory(IPMI20_SESSION_T *pSession)
{
	if(pSession)
	{
	 	pSession->bySessionStarted = SESSION_NOT_STARTED;
		if( pSession->hUDSSession != NULL )
		{
#if LIBIPMI_IS_OS_LINUX()
			/* Close UDS Session */
            if(pSession->hUDSSession->hUDSInfo.byIsConnected)
            {
                UDS_Close(&pSession->hUDSSession->hUDSInfo);
            }
            free(pSession->hUDSSession);
            if(pSession->hSMBMCUDSSession->hUDSInfo.byIsConnected)
            {
                UDS_Close(&pSession->hSMBMCUDSSession->hUDSInfo);
            }
            free(pSession->hSMBMCUDSSession);
#endif
		}
		else if( pSession->hNetworkSession != NULL )
		{
			/* Close RMCP Session */
			if(pSession->hNetworkSession->hLANInfo.byIsConnected)
				RMCP_Close(&pSession->hNetworkSession->hLANInfo);
			free(pSession->hNetworkSession);
		}
		else if( pSession->hSerialSession != NULL )
		{
			CloseSerialPort(pSession);
			free(pSession->hSerialSession);
		}
/*		else if( pSession->hKCSSession!= NULL )
		{
			CloseKCSDriver(pSession);
			free(pSession->hKCSSession);
		}*/
		else if( pSession->hUSBSession!= NULL )
		{
			free(pSession->hUSBSession);
		}
		else if( pSession->hIPMBSession!= NULL )
		{
			free(pSession->hIPMBSession);
		}

		memset(pSession, 0 , sizeof(IPMI20_SESSION_T) );
	}

	return;
}

/*---------------------------------------------------------------------
LIBIPMI_SetLIBIPMIDefaultSettings

Sets Sessions default settings
----------------------------------------------------------------------*/
void LIBIPMI_SetDefaultSerialPortSettings(IPMI20_SESSION_T *pSession)
{
	pSession->Settings.dwBaudRate	= DEFAULT_BAUD_RATE;
	pSession->Settings.byParity		= DEFAULT_PARITY;
	pSession->Settings.byStopBit	= DEFAULT_STOPBITS;
	pSession->Settings.byByteSize	= DEFAULT_BYTESIZE;

	pSession->Settings.bySettings	= SETTINGS_DEFAULT;

	if( pSession->bySessionStarted	== SESSION_STARTED &&
		pSession->hSerialSession->hSerialPort )
		ChangeSerialPortSettings(pSession);
}

/*---------------------------------------------------------------------
LIBIPMI_SetLIBIPMISettings

Sets Sessions settings to specified values
----------------------------------------------------------------------*/
void LIBIPMI_SetSerialPortSettings(IPMI20_SESSION_T *pSession, LIBIPMI_SERIAL_SETTINGS_T *Settings)
{
	pSession->Settings.dwBaudRate	= Settings->dwBaudRate;
	pSession->Settings.byParity		= Settings->byParity;
	pSession->Settings.byStopBit	= Settings->byStopBit;
	pSession->Settings.byByteSize	= Settings->byByteSize;
	

	pSession->Settings.bySettings	= SETTINGS_USER;
	
	if( pSession->bySessionStarted	== SESSION_STARTED &&
		pSession->hSerialSession->hSerialPort )
		ChangeSerialPortSettings(pSession);
}

/*---------------------------------------------------------------------
LIBIPMI_GetLIBIPMISettings

Returns the Session's settings which are already set.
----------------------------------------------------------------------*/
void LIBIPMI_GetSerialPortSettings(IPMI20_SESSION_T *pSession, LIBIPMI_SERIAL_SETTINGS_T *Settings)
{
	Settings->dwBaudRate	=	pSession->Settings.dwBaudRate;
	Settings->byParity		=	pSession->Settings.byParity;
	Settings->byStopBit		=	pSession->Settings.byStopBit;
	Settings->byByteSize	=	pSession->Settings.byByteSize;
}

/*---------------------------------------------------------------------
 * LIBIPMI_SetRetryCfg - Not implemented
 *
 * Set the Retry Count and Interval 
 * ----------------------------------------------------------------------*/
uint16 LIBIPMI_SetRetryCfg( IPMI20_SESSION_T *pSession,
                                    int nCmdRetryCount,
                                    int nCmdRetryval)

{
		if(0)
		{
			pSession=pSession;
			nCmdRetryCount=nCmdRetryCount;
			nCmdRetryval=nCmdRetryval;
		}
	        return LIBIPMI_E_SUCCESS;
}

/**
This function is used to get the UserID for given Username and Password.
It compares all IPMI username with the IN argument Username.
If it matches, then it checks for the password.
If password also matches, it will return SUCCESS and corresponding User ID.
Since duplicate username is supported in IPMI specification,
It will executes unless it gets SUCCESS or reaches MAX_USERS_COUNT.
Username and Password are IN arguments, UserID is OUT argument.
**/
#if LIBIPMI_IS_OS_LINUX()
int LIBIPMI_GetUserIDForUserName(IPMI20_SESSION_T *pSession, char *UserName, char *Password, INT8U *UserID, INT8U AuthEnable,int timeout)
{
	INT8U Index = 0;
	char IPMI_Username[MAX_USERNAME_LEN];
	int retval = 0;
	*UserID = 0;

	for(Index = 1; Index <= MAX_USERS; Index++)
	{
		memset(IPMI_Username, 0, MAX_USERNAME_LEN);
		retval = GetUserNameForUserID(pSession, Index, IPMI_Username, AuthEnable, timeout);
		if(retval == LIBIPMI_E_SUCCESS)
		{
			if(strncmp(UserName, IPMI_Username, MAX_USERNAME_LEN) == 0)
			{
				*UserID = Index;
				break;
			}
		}
	}
   
	if(*UserID != 0)
	{
		retval = CheckUserPassword(pSession, Index, Password, AuthEnable, timeout);
		if(retval == LIBIPMI_E_SUCCESS)
		{
			*UserID = Index;
			return LIBIPMI_E_SUCCESS;
		}
		else
		{
			*UserID = 0;
			return LIBIPMI_E_INVALID_PASSWORD;
		}
	}
  
	return LIBIPMI_E_INVALID_USER_NAME;
}
#endif
