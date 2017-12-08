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
 * Filename : WS_Soap2Struct.h
 * 
 * File description : The SOAP to structure convertion module 
 *
 *  Author: 	Yogeswaran T <yogeswarant@amiindia.co.in> 
 ******************************************************************/
#ifndef SOAP2STRUCT_H_
#define SOAP2STRUCT_H_

WS_INT16 WSSoap2Struct(xmlNode *operationNode, void *buffer, Parameter **parameters);
WS_UINT32 GetSizeOfType(Type *type);
#endif /*SOAP2STRUCT_H_*/
