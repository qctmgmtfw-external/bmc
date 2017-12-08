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
 * Filename : WS_Main.h
 * 
 * File description : Implementation of WS-Man Specification. 
 *
 *  Author: Arun Venkatachalam <arunv@ami.com> 
 ******************************************************************/
/*@
Externs
@*/

#ifndef _WS_MAIN_H_
#define _WS_MAIN_H_

#include "WS_Session.h"

/*@
define
@*/
#define SERVER_IP_LEN	 128
#define WSMAN_FOLDER_PATH_LEN	 256
#define SMASH_VER_LEN	256 
#define SEND_TO_DIFF_IP	-2
#define REPLY_TO 	"http://schemas.xmlsoap.org/ws/2004/08/addressing/role/anonymous"
#ifndef LINUX
#define LINUX
#endif

//#define DEBUG

typedef char* (*getWSMANUri)();
/* Returns TRUE/FALSE after validating the URI.*/
typedef int (*validateWSMANUri)(char *);

typedef enum {HTTP_DIGEST=1,HTTP_TLS_1,HTTP_TLS_2,HTTP_IPSEC} Sec_Profiles;

/*@
Structures  Declarations
@*/

typedef struct wsman_config
{
	getWSMANUri fpGetWSMANUri;
	validateWSMANUri fpValidateWSMANUri;
	 /* Has the maximum envelope size the server supports, if its indefinite set to 0.*/
	int MaxEnvelopeSize; 
	/* Path where the Folders for storing the event subscriptions are created.*/
	char WSMANPath[WSMAN_FOLDER_PATH_LEN];
	/* SMASH Version Supported. */
	char SMASHVersion[SMASH_VER_LEN];
	/* List of Security Profiles Supported. */
	Sec_Profiles e_sec_profiles[4];
	/* CIM Server IP used for creating the cim handle.*/
	char CIM_ServerIP[SERVER_IP_LEN];
	/* CIM Server Port used for creating the cim handle.*/
	int CIM_ServerPort;
	/* CIM Default NameSpace used for creating the cim handle.*/
	char CIM_DefaultNameSpace[128];
	/* Credentials used for communicating with the CIM Server.*/
	char UserName[128];
	char Password[128];
}WSMAN_Config;



typedef enum {
    WSM_OK,
    WSM_VERSION_MISMATCH,
    WSM_SENDER,
    WSM_RECEIVER,
    WSM_MUST_UNDERSTAND,
    WSM_DATA_ENCODING_UNKNOWN
}WSMStatus;



/*@
Function Declarations
@*/
extern void WSInitialize(WSMAN_Config *wsmanConfig);
extern void WSDestroy(void);
extern WSMStatus WSHandleSOAPRequest(char *soapRequest,char **soapResponse, struct Session *session);
#endif



