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
 * Filename : WS_Struct2Soap.h
 * 
 * File description : The structure to SOAP conversion 
 *
 *  Author: 	Yogeswaran T <yogeswarant@amiindia.co.in> 
 ******************************************************************/
#ifndef STRUCT2SOAP_H_
#define STRUCT2SOAP_H_

WS_INT16 WSStruct2Soap(xmlNode **responseNode, void *buffer, Parameter **parameters, char *operation);

#endif /*STRUCT2SOAP_H_*/
