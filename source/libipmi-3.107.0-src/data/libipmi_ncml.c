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
* Filename: libipmi_ncml.c
*
* Descriptions: Contains implementation of a generic NCML Conf Request/Response
* This will be used by other services who need to talk to NCML for their conf.
*
* Author: Viswanathan S
*
******************************************************************/
#include "libipmi_ncml.h"
#include "libipmi_errorcodes.h"
#include "libipmi_session.h"
#include "libipmi_struct.h"
#include "IPMI_AMIConf.h"
#include "libipmi_AMIEXT.h"

#if LIBIPMI_IS_OS_LINUX()
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/file.h>
#include <fcntl.h>
#include <errno.h>
#include "ncml.h"
#include "stunnel_cfg.h"
#endif

#define TIMEOUT		5

int FindServiceID(char *service_name)
{
	int index = 0;
	int retval = -1;
	char *ServiceNameList[MAX_SERVICES] = {
                                              WEB_SERVICE_NAME,
                                              //KVM_SERVICE_NAME,//Quanat--
                                              //CDMEDIA_SERVICE_NAME,//Quanat--
                                              //FDMEDIA_SERVICE_NAME,//Quanat--
                                              //HDMEDIA_SERVICE_NAME,//Quanat--
                                              SSH_SERVICE_NAME,
                                              //TELNET_SERVICE_NAME, //Quanta--
                                          };

	for (index = 0; index < MAX_SERVICES; index++)
	{
		if (memcmp(service_name, ServiceNameList[index], strlen(ServiceNameList[index])) == 0)
		{
			retval = index;
			break;
		}
	}

	return retval;
}

int Get_Service_Conf_UDS (char *service_name, void *Res)
{
	IPMI20_SESSION_T	hSession;
	AMIGetServiceConfRes_T	hAMIGetServiceConfRes;
	SERVICE_CONF_STRUCT	*ServiceConfResponse 	= (SERVICE_CONF_STRUCT *)(Res);
	int timeout = TIMEOUT;
	int wRet = 0;
	uint32 ServiceID = 0;
	uint8 Privlevel = PRIV_LEVEL_ADMIN;

        wRet = LIBIPMI_Create_IPMI_Local_Session(&hSession, "", "", &Privlevel,NULL,AUTH_BYPASS_FLAG, 5);

        if (wRet != LIBIPMI_E_SUCCESS)
        {
                printf("Error establishing IPMI Session : %x\n",wRet);
                return wRet;
        }

	ServiceID = (0x01 << FindServiceID(service_name));
	if (ServiceID == 0)
	{
		printf("Unable to find the specified Service::%s\n", service_name);
		return ServiceID;
	}

	wRet = (int)LIBIPMI_HL_AMIGetServiceConf(&hSession, ServiceID, &hAMIGetServiceConfRes, timeout);
	if (wRet != 0)
	{
		printf("Unable to fetch the specified Service Conf Info::%s\n", service_name);
		return -1;
	}

	memset((char *)ServiceConfResponse, 0, sizeof(SERVICE_CONF_STRUCT));

	ServiceConfResponse->CurrentState = hAMIGetServiceConfRes.Enable;
	memcpy(ServiceConfResponse->InterfaceName, hAMIGetServiceConfRes.InterfaceName, MAX_SERVICE_IFACE_NAME_SIZE);
	ServiceConfResponse->NonSecureAccessPort = hAMIGetServiceConfRes.NonSecureAccessPort;
	ServiceConfResponse->SecureAccessPort = hAMIGetServiceConfRes.SecureAccessPort;
	ServiceConfResponse->SessionInactivityTimeout = hAMIGetServiceConfRes.SessionInactivityTimeout;
	ServiceConfResponse->MaxAllowSession = hAMIGetServiceConfRes.MaxAllowSession;
	ServiceConfResponse->CurrentActiveSession = hAMIGetServiceConfRes.CurrentActiveSession;

	LIBIPMI_CloseSession(&hSession);

	return 0;
}

int Set_Service_Conf_UDS (char *service_name, void *Req)
{
	IPMI20_SESSION_T	hSession;
	AMISetServiceConfReq_T hAMISetServiceConfReq;
	int timeout = TIMEOUT;
	int wRet = 0;
	uint32 ServiceID = 0;
	uint8 Privlevel = PRIV_LEVEL_ADMIN;
	SERVICE_CONF_STRUCT	*ServiceConfRequest = (SERVICE_CONF_STRUCT *)Req;

        wRet = LIBIPMI_Create_IPMI_Local_Session(&hSession, "", "", &Privlevel,NULL,AUTH_BYPASS_FLAG, 5);

        if (wRet != LIBIPMI_E_SUCCESS)
        {
                printf("Error establishing IPMI Session : %x\n",wRet);
                return wRet;
        }

	ServiceID = (0x01 << FindServiceID(service_name));
	if (ServiceID == 0)
	{
		printf("Unable to find the specified Service::%s\n", service_name);
		return ServiceID;
	}

    memset((char *)&hAMISetServiceConfReq, 0, sizeof(AMISetServiceConfReq_T));

    hAMISetServiceConfReq.ServiceID = ServiceID;
    hAMISetServiceConfReq.Enable = ServiceConfRequest->CurrentState;
	memcpy(hAMISetServiceConfReq.InterfaceName, ServiceConfRequest->InterfaceName, MAX_SERVICE_IFACE_NAME_SIZE);
	hAMISetServiceConfReq.NonSecureAccessPort = ServiceConfRequest->NonSecureAccessPort;
	hAMISetServiceConfReq.SecureAccessPort = ServiceConfRequest->SecureAccessPort;
	hAMISetServiceConfReq.SessionInactivityTimeout = ServiceConfRequest->SessionInactivityTimeout;
	hAMISetServiceConfReq.MaxAllowSession = ServiceConfRequest->MaxAllowSession;
	hAMISetServiceConfReq.CurrentActiveSession = ServiceConfRequest->CurrentActiveSession;

	wRet = (int)LIBIPMI_HL_AMISetServiceConf(&hSession, &hAMISetServiceConfReq, timeout);
	if (wRet != 0)
	{
		printf("Unable to set the specified Service Conf Info::%s\n", service_name);
		return -1;
	}

	LIBIPMI_CloseSession(&hSession);

	return 0;
}
