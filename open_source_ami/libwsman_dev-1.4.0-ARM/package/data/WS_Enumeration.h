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
 * Filename : WS_Enumeration.h
 * 
 * File description : Implementation of WS-Enumeration 
 *
 *  Author: Yogeswaran T <yogeswarant@amiindia.co.in> 
 ******************************************************************/
#ifndef _WS_ENUMERATION_H_
#define _WS_ENUMERATION_H_

#include "WS_Structures.h"

#define NULL_CONTEXT		 	0
#define RESOURCE_URI_STR_LEN	128

#define ACTION_ENUMERATE  "Enumerate"
#define ACTION_PULL 	  "Pull"
#define ACTION_RELEASE 	  "Release"
#define SELECTOR_FILTER_DIALECT "http://schemas.dmtf.org/wbem/wsman/1/wsman/SelectorFilter"

struct EnumerationNode;

typedef int (*PullFP)(void *, xmlNode *itemsNode, int*);
typedef int (*ReleaseFP)(void *e);

extern int g_ContextNum;
extern LinkedList *g_enumerationList; 

typedef struct
{
//	char ResURI[URI_LEN];
	int EnumContextNum;
	WSMAN_EPR EPR;
	PullFP pullFP;
	ReleaseFP releaseFP;
	void *resourcePtr;
	void *eHandler;

}Enumeration;


/* Enumeration thread Input */
typedef struct ENUM_THREAD_INPUT
{
	struct SOAPStructure *soapStruct;
	char **soapResponse;
}EnumerationThInput; 


void * WSEnumerationHandler_T(void *Input);


typedef void *(*EnumerateFP)(Enumerate_Req *enumReq);

typedef struct EnumerationNode
{
	Enumeration *pEnum;
	struct EnumerationNode *pNext;
}EnumerationNode;


/* */
WS_INT16 EnumerationManager(struct SOAPStructure *soapStruct,  char **response);

int WSCreateSubsEnumPlugin(char *resURI);

/* */
WS_INT16 CompareContext(void *enumeration,  void *contextNum);

/* */
WS_INT16 WSFreeEnumeration(void *enumeration);

/* Function to free the enumeration related information in the SOAP structure */
WS_INT16 WSFreeEnumerationBody(void *enumerationBody);
#endif


