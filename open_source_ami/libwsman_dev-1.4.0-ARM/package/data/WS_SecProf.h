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
 * File description : Implementation of security profiles 
 *
 *  Author: 	Manish Tomar <manisht@amiindia.co.in> 
 ******************************************************************/
#ifndef _WS_SECPROF_H_
#define _WS_SECPROF_H_


#include "WS_Structures.h"
#include "WS_EventingDaemon.h"

extern void Encode64(char *outbuf, char *string, int outlen);

// security profile URIs

#define	SEC_PROF_HTTP_BASIC		"wsman:secprofile/http/basic"
#define	SEC_PROF_HTTP_DIGEST	"wsman:secprofile/http/digest"
#define	SEC_PROF_HTTPS_BASIC	"wsman:secprofile/https/basic"
#define	SEC_PROF_HTTPS_DIGEST	"wsman:secprofile/https/digest"

#define MD5_DIGEST_STR_LEN	(32 + 1)
#define	HTTP_REQ_LEN		(1024 * 2)

extern int HttpBasicSend(AuthInfo* ai, char* URI, char* SoapEnv, int length);

extern int HttpDigestSend(AuthInfo* ai, char* URI, char* SoapEnv, int length);

extern int HttpsBasicSend(AuthInfo* ai, char* URI, char* SoapEnv, int length);

extern int HttpsDigestSend(AuthInfo* ai, char* URI, char* SoapEnv, int length);

#endif

