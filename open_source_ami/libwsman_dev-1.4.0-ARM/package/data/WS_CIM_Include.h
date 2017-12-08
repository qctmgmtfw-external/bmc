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
 * Filename : WS_CIM_Include.h
 * 
 * File description : CIM Configuration 
 *
 *  Author: Yogeswaran T <yogeswarant@amiindia.co.in> 
 ******************************************************************/
#ifndef _WS_CIM_INCLUDE_H_
#define _WS_CIM_INCLUDE_H_

#define CIM_RES_URI 		CONFIG_SPX_FEATURE_GLOBAL_WSMAN_URI

/* All classes ResourceURI */
#define ALL_CLS_RES_URI CONFIG_SPX_FEATURE_GLOBAL_WSMAN_URI

#define CIM_INTEROP_NAMESPACE "root/interop"
#define CIM_TIMEOUT 		15000

//------------- FILTER DIALECT -------------//

// Associated instance filter dialect
#define ASSOCINST_FILTER_DIALECT "http://schemas.dmtf.org/wbem/wsman/1/cimbinding/associationFilter"

// Execute query filter dialect
#define CQL_FILTER_DIALECT "http://schemas.dmtf.org/wbem/cql/1/dsp0202.pdf"
#endif

