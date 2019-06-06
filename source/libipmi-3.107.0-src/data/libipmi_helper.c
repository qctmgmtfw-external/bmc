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
* Filename: libipmi_helper.c
*
* Descriptions: Contains implementation of helper routines 
*   required by libipmi core
*
* Author: Rajasekhar
*
******************************************************************/
#include "libipmi_helper.h"
#include "IPMIDefs.h"

/*---------------------------------------------------------
CalculateCheckSum
----------------------------------------------------------*/
uint8	LIBIPMI_CalculateCheckSum(uint8 *pbyStart, uint8 *pbyEnd)
{
	uint8 byCheckSum=0;

	while(pbyStart < pbyEnd) 
		byCheckSum += (uint8)*pbyStart++;


	return (uint8) (-byCheckSum);
}

/*---------------------------------------------------------
ValidateCheckSum
----------------------------------------------------------*/
uint8	LIBIPMI_ValidateCheckSum(uint8	*pbyData, uint32 dwDataLen)
{
	IPMIMsgHdr_T	*pbyIPMIHdr;
	//uint8			*pbyIPMIData;
	uint8			ChkSum1;
	uint8			ChkSum2;

	pbyIPMIHdr = (IPMIMsgHdr_T	*)pbyData;
	//pbyIPMIData = &pbyData[sizeof(IPMIMsgHdr_T)];

	ChkSum1 = LIBIPMI_CalculateCheckSum( (uint8 *)pbyIPMIHdr, (uint8 *)&(pbyIPMIHdr->ChkSum) );
	ChkSum2 = LIBIPMI_CalculateCheckSum( (&(pbyIPMIHdr->ChkSum))+1, &pbyData[dwDataLen-1] );

	if( ChkSum1  == pbyIPMIHdr->ChkSum &&
		ChkSum2 == pbyData[dwDataLen-1])
		return 1;

	return 0;
}


