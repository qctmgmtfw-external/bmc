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
 * Filename : WS_CIMTransfer.h
 * 
 * File description : CIM related transfer operation 
 *
 *  Author: Yogeswaran T <yogeswarant@amiindia.co.in> 
 ******************************************************************/
#ifndef _WS_CIMTRANSFER_H_
#define _WS_CIMTRANSFER_H_
#include "WS_Structures.h"

#include "tree.h"


xmlNode * WSCIMTransferCreate(SelectorSet *selectorSet,char *resourceURI, xmlNode *instNode);
xmlNode * WSCIMTransferGet(SelectorSet *selectorSet,char *resourceURI);
xmlNode * WSCIMTransferDelete(SelectorSet *selectorSet,char *resourceURI);
xmlNode * WSCIMTransferPut(SelectorSet *selectorSet,char *resourceURI,xmlNode *instNode);


#endif
