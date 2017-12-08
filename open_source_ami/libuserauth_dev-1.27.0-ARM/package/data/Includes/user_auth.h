/*****************************************************************
 * ******************************************************************
 * ***                                                            ***
 * ***        (C)Copyright 2010, American Megatrends Inc.         ***
 * ***                                                            ***
 * ***                    All Rights Reserved                     ***
 * ***                                                            ***
 * ***       5555 Oakbrook Parkway, Norcross, GA 30093, USA       ***
 * ***                                                            ***
 * ***                     Phone 770.246.8600                     ***
 * ***                                                            ***
 * ******************************************************************
 * ******************************************************************
 * ******************************************************************
 * * 
 * * Filename: user_auth.h
 * *
 * * Description: user authentication related API's                   
 * *
 * * Author: Pavithra S(pavithras@amiindia.co.in)
 * *
 * ******************************************************************/
#ifndef IPMIUSER_AUTH_H
#define IPMIUSER_AUTH_H

#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <pwd.h>//getpwnam
#include "dbgout.h"

#define IPMI_TIMEOUT 30

#define USER_DIR "/home"
#define USER_DEFINED_ID 	1000

#define GID_PROPRIETARY         0x80000005 
#define GID_ADMIN		0x80000004
#define GID_OPERATOR		0x80000003
#define GID_USER		0x80000002
#define GID_CALLBACK		0x80000001
#define GID_NONE		0x80000000

#define MIN_UID_RANGE 		1000
#define MAX_UID_RANGE 		1999

#define GET_USER_INFO 		1
#define DUMMY_UID 			0
#define DUMMY_BUF_LEN 		0
#define MAX_PARAMS_LEN 		256
#define RESERVED_USERS_FILE "/etc/reservedusers"
#define INVALID_USR_CREDEN	0x80
#define IPMI_USER_DISABLED	0x02
#define MAX_USER_NAME_LENGTH 16
#define SERVICE_NAME_LEN	25
#define USER_NAME_LEN		25
#define MAX_IP_LEN		25
#define MODCNT_ENVNAME		"MDCount"
#define SEL_LOGIN_SUCCESS	0
#define SEL_LOGOUT_SUCCESS	1
#define SEL_LOGIN_FAILED	3

/*Enviroment variable to represent the autologout*/
#define PAM_AUTOLOGOUT "AutoLogout"

#if CONFIG_SPX_FEATURE_AD_LOADED && CONFIG_SPX_FEATURE_LDAP_LOADED
#define PAM_MODULE_COUNT    3
#elif CONFIG_SPX_FEATURE_AD_LOADED || CONFIG_SPX_FEATURE_LDAP_LOADED
#define PAM_MODULE_COUNT    2
#else
#define PAM_MODULE_COUNT    1
#endif

#define ROOT_USER_NAME  "sysadmin"
extern int PDK_LoginLogoutAudit (void *hSession, char *service, char*uname, char *ipstr, unsigned char state);
int GetReqUserInfo(void *LSession, char* pReqUser,char* pReqPassword,struct passwd *pwd,char *buffer,size_t buflen,uid_t uid,int getUserInfo);
//void MapPrivilegetoGid(unsigned char ChUserAccess,gid_t *gid);
int GetReservedUserInfo(char *reservedusers_file,const char *name,struct passwd *pwd);
int GetUserPrivilege(char *username,int *userpriv);
#endif
