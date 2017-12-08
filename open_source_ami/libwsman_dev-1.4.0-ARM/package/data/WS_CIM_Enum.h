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
 * Filename : WS_CIM_Enum.h
 * 
 * File description : CIM related enumerations  
 *
 *  Author: Manish Tomar T <manisht@amiindia.co.in> 
 ******************************************************************/
#ifndef _WS_CIM_ENUM_H_
#define _WS_CIM_ENUM_H_

#include "WS_Structures.h"
#include "WS_LinkedList.h"

#include "CIMC_Include.h"

#define CIM_QUERY_LANGUAGE "CQL"
#define WBEM_QUERY_LANGUAGE "WQL"

void *CIM_Enumerate(Enumerate_Req *enumreq);
int CIM_Pull(void *e, xmlNode *itemsNode, int *EOS);
int CIM_Release(void *e);
WS_INT16 WS_GetkeyPropertiesCount(void *pClientHandle,char *pstrClassName);

typedef struct CIMEnum
{
	EnumMode enumMode;
	
	// once finding the kind of enumeration during enumerate request
	// the associated pull function will be stored here.  
	WS_INT16 (*pullFP)(struct CIMEnum *e, xmlNode *itemsNode, int *EOS);	

	// storing eprs while enumerating instance names
	LinkedList *eprs;
	ListNode *currEPRNode;

	CIMC_NamedInstance *namedInstances;
	CIMC_NamedInstance *currNamedInstance;
	
	CIMC_ObjectWithPath *assocInstances;
	CIMC_ObjectWithPath *currAssocObjectPath;
	//Used to store the result of execute query
	CIMC_ObjectWithPath *pObjectWithPath;
	
	WS_Filter *pFilter;
	char resURI[URI_LEN];
	char namespace[NAMESPACE_LEN];
	
}CIMEnum;

WS_INT16 WSFreeCIMEnum(CIMEnum *ce);

#endif
