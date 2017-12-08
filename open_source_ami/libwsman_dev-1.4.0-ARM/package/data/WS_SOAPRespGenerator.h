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
 * Filename : WS_SOAPRespGenerator.h
 * 
 * File description : The response generator module 
 *
 *  Author: 	Yogeswaran T <yogeswarant@amiindia.co.in> 
 ******************************************************************/

#ifndef _WS_SOAPRESPGENERATOR_H_
#define _WS_SOAPRESPGENERATOR_H_

#include "tree.h"

WS_INT16 GenerateTransferCustomResponseXML(struct SOAPStructure *soap, xmlNode *responseNode,char **responseBuffer);
WS_INT16 GenerateEnumerateResponseXML(struct SOAPStructure *soap, int contextNumber, char **response);
WS_INT16 GeneratePullResponseXML(struct SOAPStructure *soap, int contextNumber, xmlNode *itemsNode , int EOS, char **response);
WS_INT16 GenerateReleaseResponseXML(struct SOAPStructure *soap, char **response);
WS_INT16 GenerateIdentifyResponse(char **ppSoapRespXML);
#ifdef EVENTING_SUPPORT
#include "WS_EventingDaemon.h"
WS_INT16 GenerateEventingResponse(struct SOAPStructure *pSoap, Eventing_Resp *pEventingResp, char **responseBuffer);
#endif 

#endif 
