/****************************************************************
****************************************************************
**                                                            **
**    (C)Copyright 2010-2011, American Megatrends Inc.        **
**                                                            **
**            All Rights Reserved.                            **
**                                                            **
**        5555, Oakbrook Pkwy, Norcross,                      **
**                                                            **
**        Georgia - 30093, USA. Phone-(770)-246-8600.         **
**                                                            **
****************************************************************
*****************************************************************
*
* userprivilege.h
* libUserPrivilege Library
*
* Author: Revanth A <revantha@amiindia.co.in>
*
*****************************************************************/
#ifndef LIBUSER_PRIVILEGE_
#define LIBUSER_PRIVILEGE_

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <grp.h>
#include "stdgrps.h"

#define PRIV_MAP_TBL_SIZE		6

#define GRP_ADMIN "admin"
#define GRP_OEM "oem"
#define GRP_OPER "operator"
#define GRP_USER "user"
#define GRP_CALLBACK "callback"
#define GRP_NOACCESS "noaccess"

#define PREFIX_LAN1 "lan"
#define PREFIX_LAN2 "lan2"
#define PREFIX_LAN3 "lan3"
#define PREFIX_SERIAL "serial"

#define GRP_IPMI "ipmi"
#define GRP_LDAP "ldap"
#define GRP_AD "ad"

#define LAN1_ADMIN                  "lanadmin"
#define LAN1_OEM                    "lanoem"
#define LAN1_OPERA                  "lanoperator"
#define LAN1_USER                   "lanuser"
#define LAN1_CALLBACK               "lancallback"
#define LAN1_NOACCESS               "lannoaccess"

#define LAN2_ADMIN                  "lan2admin"
#define LAN2_OEM                    "lan2oem"
#define LAN2_OPERA                  "lan2operator"
#define LAN2_USER                   "lan2user"
#define LAN2_CALLBACK               "lan2callback"
#define LAN2_NOACCESS               "lan2noaccess"

#define LAN3_ADMIN                  "lan3admin"
#define LAN3_OEM                    "lan3oem"
#define LAN3_OPERA                  "lan3operator"
#define LAN3_USER                   "lan3user"
#define LAN3_CALLBACK               "lan3callback"
#define LAN3_NOACCESS               "lan3noaccess"

#define SERIAL_ADMIN               "serialadmin"
#define SERIAL_OEM                 "serialoem"
#define SERIAL_OPERA               "serialoperator"
#define SERIAL_USER                "serialuser"
#define SERIAL_CALLBACK    "serialcallback"
#define SERIAL_NOACCESS    "serialnoaccess"

const unsigned int  g_Grpsprv[PRIV_MAP_TBL_SIZE]= { PRIV_LAN_ADMIN, PRIV_LAN_OEM, PRIV_LAN_OPERATOR, PRIV_LAN_USER, PRIV_LAN_CALLBACK, PRIV_LAN_NOACCESS };

const char *Lan1ChGrps[PRIV_MAP_TBL_SIZE] = { LAN1_ADMIN, LAN1_OEM, LAN1_OPERA, LAN1_USER, LAN1_CALLBACK, LAN1_NOACCESS };
const char *Lan2ChGrps[PRIV_MAP_TBL_SIZE] = { LAN2_ADMIN, LAN2_OEM, LAN2_OPERA, LAN2_USER, LAN2_CALLBACK, LAN2_NOACCESS };
const char *Lan3ChGrps[PRIV_MAP_TBL_SIZE] = { LAN3_ADMIN, LAN3_OEM, LAN3_OPERA, LAN3_USER, LAN3_CALLBACK, LAN3_NOACCESS };

const char *SerialChGrps[PRIV_MAP_TBL_SIZE] = {SERIAL_ADMIN, SERIAL_OEM, SERIAL_OPERA, SERIAL_USER, SERIAL_CALLBACK, SERIAL_NOACCESS };

typedef struct
{
    char* grpname;
    int grppriv;
} GroupPriv_t;

gid_t* GetUsrGroups(char *username,int *ngrps);
void GetUsrPriv(char *username,int *priv);
void AddIPMIUsrtoChGrp(char *username,char *oldgrp, char *newgrp);

void AddUsrtoIPMIGrp(char *username);
void AddUsrtoGenericGrp(char *username,char *servicegrp,char** newgrps,int numgrps);

void DeleteUsrFromIPMIGrp(char *username);
int getUsrGrpgid(char * username, gid_t *usr_grpgid);

#endif /*LIBUSER_PRIVILLEGE_*/
