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
* Filename: libipmi_device_api.c
*
******************************************************************/
#include "libipmi_session.h"
#include "libipmi_errorcodes.h"
#include "libipmi_api.h"

#include <string.h>
uint16 LIBIPMI_GetDeviceID( IPMI20_SESSION_T *pSession, char *pszDeviceID, int timeout )
{
	uint8	Res [20], Req [20];
	uint32	dwResLen;
	uint8	i;
	uint16	wRet;
	char	szTemp[10];

	pszDeviceID[0] = (char)0;
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
											DEFAULT_NET_FN_LUN, 0x01,
											Req, 0,
											Res, &dwResLen,
											timeout);
	if (wRet == LIBIPMI_E_SUCCESS)
	{
		for (i = 0; i < (uint8)dwResLen; i++ )
		{
			sprintf (szTemp, "%x ", Res[i]);
			strcat(pszDeviceID, szTemp);
		}
	}
	
	return wRet;
}


#if 0
int main()
{
	IPMI20_SESSION_T hSession;
	uint16	wRet;
	uint8				m_KG [] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };

	memset((void*)&hSession, 0, sizeof(IPMI20_SESSION_T) );

	printf("*********************** Entering main **********************\n");
	wRet = Create_IPMI20_Network_Session(&hSession, "10.0.0.217", 623,
							"", "", 0,
							0, 0, 0x04,
							0xc2, 0x20,
							m_KG, sizeof(m_KG),
							1000);
	printf("*************** Create_IPMI20_Session returned ***************\n");

	if (wRet == LIBIPMI_E_SUCCESS)
	{
		GetDeviceID( &hSession );
	}
	else
	{
		printf("*********************** Error 1 ***************************");
		printf("Error in creating the session\n");
	}

	return 0;
}
#endif
