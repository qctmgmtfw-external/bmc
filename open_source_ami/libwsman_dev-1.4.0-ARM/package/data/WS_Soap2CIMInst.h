/*****************************************************************
 *****************************************************************
 ***                                                            **
 ***    (C)Copyright 2005-2006, American Megatrends Inc.        **
 ***                                                            **
 ***            All Rights Reserved.                            **
 ***                                                            **
 ***        6145-F, Northbelt Parkway, Norcross,                **
 ***                                                            **
 ***        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 ***                                                            **
 *****************************************************************
 *****************************************************************
 ******************************************************************
 *
 * Filename : WS_Soap2CIMInst.h
 * 
 * File description : Implementation of WS-Man Specification. 
 *
 *  Author: 	Yogeswaran T <yogeswarant@amiindia.co.in> 
 ******************************************************************/
#ifndef WS_SOAP2CIMINST_H_
#define WS_SOAP2CIMINST_H_

#include "CIMC_Include.h"

WS_INT16 WS_CheckForGivenQualifierName(CIMC_Qualifier *pQualifier, char *pstrQualifierName);
WS_INT16 WS_CheckForEmbeddedInstance(CIMC_Qualifier *pQualifier, char *pstrEmbeddedInstClassName , int nLength);
CIMC_Class *WS_GetClass( void *pHandle ,char *pstrResourceURI);
CIMC_Method *WS_GetCIMMethod(CIMC_Class  *pClass , char *pstrMethodName);
CIMC_Instance * WSSoap2Instance(void *cim_handle,char *resURI, xmlNode *instNode);
CIMC_ParamValue *WSSoap2ParamValues(void *cim_handle, CIMC_Method *pMethod, xmlNode *instNode);

#endif /*WS_SOAP2CIMINST_H_*/
