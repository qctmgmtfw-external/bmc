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
 * Filename : WS_Executer.h
 * 
 * File description : Executer delegates work to appropriate module
 *					  depending upon the Action URI 
 *
 *  Author: Yogeswaran T <yogeswarant@amiindia.co.in> 
 ******************************************************************/
#ifndef _WS_EXECUTER_H_
#define _WS_EXECUTER_H_

#include "tree.h"
#include "session.h"

#define SERVICE_NAME_LEN	64
#define OPERATION_TIMEOUT 	-2

//#define WINDOWS
//#define DEBUG

/*@
data structure
@*/

typedef struct tokens
{
	char *singletoken;
	struct tokens *next,*prev;
}TOKENS;


/*@
function
@*/

/* Main module for executer */
WS_INT16 WSExecuter(struct SOAPStructure *soapStruct, char **soapResponseBuffer,struct Session *session);


/*This function used to get action string (Create, Delete, Get or Put)*/
/*@
global
@*/

//Global variable for storing Response SOAP Body information
extern xmlNode *root;
long g_Operation_Timeout;



enum { WS_TRANSFER = 1, WS_CIM_TRANSFER, WS_CUSTOM_OPERATION, WS_CIM_CUSTOM_OPERATION }OperationType;

#endif
