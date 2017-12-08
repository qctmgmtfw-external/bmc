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
 * Filename : WS_CIM_InvokeMethod.h
 * 
 * File description : Implementation of WS-Man Specification. 
 *
 *  Author: Yogeswaran T <yogeswarant@amiindia.co.in> 
 ******************************************************************/
#ifndef _WS_CIM_INVOKEMETHOD_H_
#define _WS_CIM_INVOKEMETHOD_H_

#include "tree.h"
#include "CIMC_Include.h"
xmlNode * InvokeMethod(SelectorSet *selectorSet,char *resourceURI,xmlNode *instNode,char *action);

#endif
