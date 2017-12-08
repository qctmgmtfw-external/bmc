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
 * Filename : WS_Include.h
 *
 * File description : Implementation of WS-Man Specification.
 *
 *  Author: Yogeswaran T <yogeswarant@amiindia.co.in>
 ******************************************************************/
#ifndef _WS_INCLUDE_H_	// Check for Include once
#define _WS_INCLUDE_H_

#include "WS_Main.h"
#include "CIMC_Include.h"
/* for memwatch */
//#include "memwatch.h"

/*@
Definitions
@*/


#ifndef SUCCESS
#define SUCCESS 		0
#endif

#ifndef FAILURE
#define FAILURE			-1
#endif

#define LINUX
#define WS_DEBUG 		printf
#define URI_LEN			256
#define NAME_LEN		128
#define UID_LEN			256
#define TIME_LEN		10
#define ET_LEN 			512
#define PREFIX_LEN 		8
#define LEN			256
#define TRUE			1
#define FALSE			0
#define STR_LEN			256
#define DIALECT_LEN		1024
#define FAULT_DETAIL_LEN	1024
#define BUFF_SIZE		1024
#define CIM_NAME_LEN	128
#define ENUM_MODE_LEN 	256
#define MAX_SERVER_URI	6
#define PROPNAME_LEN 64
#define TIME_OUT	60
#define NAMESPACE_LEN	32

typedef char WS_CHAR;

typedef short int WS_INT16;
typedef unsigned short int WS_UINT16;
typedef unsigned int WS_UINT32;
typedef short int WS_BOOLEAN;


extern char g_pPossibleURIs[MAX_SERVER_URI][URI_LEN];

extern char g_webUserName[LEN];
extern char g_webPassword[LEN];

extern char gClientIP[32] ;
extern int gClientPort ;
extern long g_Operation_Timeout;
extern WSMAN_Config g_WSMAN_Config;
extern CIMC_Class *g_class;
extern void *get_cim_handle(char *ns);
extern int close_cim_handle();

#endif // end _WS_INCLUDE_H_

