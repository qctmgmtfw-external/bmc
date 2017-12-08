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
 * Filename : WS_CIM_XML.h
 * 
 * File description : CIM - SOAP  conversion helper functions
 *
 *  Author: Manish Tomar <manisht@amiindia.co.in> 
 ******************************************************************/
#ifndef _WS_CIM_XML_H_
#define _WS_CIM_XML_H_

#include "CIMC_Include.h"
#include "WS_Structures.h"
#include "WS_CIM_Enum.h"
#include "WS_EventingDaemon.h"
#include "tree.h"

char *getClassName(char *resourceURI);

CIMC_InstanceName *WSMAN_EPR2InstanceName(WSMAN_EPR *epr);
CIMC_InstanceName *WSMAN_SubsSelector2InstanceName(SubsSelectorSet select);
CIMC_LocalInstancePath * WSMAN_EPR2LocalInstancePath(WSMAN_EPR *epr);
WSMAN_EPR *InstanceName2WSMAN_EPR(CIMC_InstanceName *insName);
WSMAN_EPR *InstPath2WSMAN_EPR(CIMC_InstancePath *pInstPath);
WSMAN_EPR *LocalInstPath2WSMAN_EPR(CIMC_LocalInstancePath *pLocalInstPath);
xmlNode *getInstanceXMLNode(CIMC_Instance *instance, char *namespace);
xmlNode *getWSMAN_EPR_XML_Node(WSMAN_EPR *epr, char *eprName, char *nameSpace);
xmlNode *getClassXMLNode(CIMC_Class *pClass);
xmlNode* getXMLFragmentNode(CIMEnum *pCIMEnum);
xmlNode *getXMLPropertyFragment(CIMC_Class *propertyHead);
xmlNode *WS_GetDateTimeNode(char *pstrDateTime);
WS_INT16 CompareProperties(void *ppropval1, void *ppropval2);
WS_INT16 FreeProperty(void *address);
WSMAN_EPR *CloneWSMANEpr(WSMAN_EPR *epr);
xmlNode* WS_CreateCIMErrorInstanceNode(char *pstrErrorCode, char* pstrErrorDescription);
void AppendCIMError(CIMC_Error *pError);
#endif
