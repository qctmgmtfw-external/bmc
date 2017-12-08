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
 * Filename : WS_ResourceCatalog.h
 * 
 * File description : The resource file parsing module 
 *
 *  Author: 	Yogeswaran T <yogeswarant@amiindia.co.in> 
 ******************************************************************/
#ifndef _WS_RESOURCECATALOG_H_
#define _WS_RESOURCECATALOG_H_

#include "WS_Structures.h"
#include "WS_LinkedList.h"

#include "tree.h"
#include "include.h"
#include "sparser.h"
#include "serialize.h"


/* Maximum number of Resource files */
#define MAX_RES 20	

/* The directory path of Resource files */
#define CAT_DIR "/etc/webWSMAN/Res/"

/* The directory path for WSDL files */
#define WSDL_DIR "/etc/webWSMAN/"

/* WSMAN Catalog ResourceURI  */ 
#define CATALOG_RES_URI "wsman:system/catalog/2005/06/Catalog"

/* WSDL ResourceURI */
#define WSDL_RES_URI	"wsman:system/catalog/2005/06/WSDL1.1"

void *Catalog_Enumerate(Enumerate_Req *enumreq);
int Catalog_Release(void *e);
int Catalog_Pull(void *e, xmlNode *itemsNode, int *EOS);
WS_INT16 WSGenerateResourceList(void);
WS_INT16 WSCompareResourceURI(void *address, void *data);
WS_INT16 WSCompareWSDLRef(void *address, void *data);	
xmlNode *WSGetWSDLNode(SelectorSet *selectorSet);
WS_INT16 WSFreeResourceList(void);
extern LinkedList *g_resourceList;

#ifdef DEBUG
	void PrintResource(void *resource);
#endif //DEBUG

#endif /* _WS_RESOURCECATALOG_H_ */
