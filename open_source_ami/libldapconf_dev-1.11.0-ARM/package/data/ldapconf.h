/*****************************************************************
**                                                              **
**     (C) Copyright 2006-2009, American Megatrends Inc.        **
**                                                              **
**             All Rights Reserved.                             **
**                                                              **
**         5555 Oakbrook Pkwy Suite 200, Norcross,              **
**                                                              **
**         Georgia - 30093, USA. Phone-(770)-246-8600.          **
**                                                              **
******************************************************************
* File : ldapconf.h                                              *
*                                                                *
* Description : LDAP related configuration routines              *
*****************************************************************/
#ifndef __LDAP_H__
#define __LDAP_H__

#include "Types.h"

#define ARRAY_SIZE		128
#define INV_PWD_GIVEN		0x80
#define LDAP_MAX_QUERY_LENGTH 300
#define LDAP_MAX_DN_LENGTH	520
#define	MAX_GROUPS_SUPPORTED	5
#define MAX_ROLE_GROUPS 	5
#define	RMC_RC_LDAP_OK					(0)
#define	RMC_RC_LDAP_CONNECTION_FAIL		(1)
#define	RMC_RC_LDAP_NO_OBJECT			(2)
#define	RMC_RC_LDAP_INVALID_CREDENTIAL	(3)
#define	RMC_RC_LDAP_INTERNAL_ERROR		(4)
#define	RMC_RC_LDAP_INVALID_PARAM		(5)
#define	RMC_RC_LDAP_NOT_READY			(6)
#define	RMC_RC_LDAP_INVALID_DOMAIN		(7)
#define	RMC_RC_LDAP_DNS_RESOLVE_FAIL	(8)
#define	RMC_RC_LDAP_NOT_ENABLED			(9)
#define	RMC_RC_LDAP_PROP_READ_ERROR		(10)
#define	RMC_RC_LDAP_UKNOWN_AD_TYPE		(11)
#define	RMC_RC_LDAP_MALLOC_ERROR			(12)
#define	RMC_RC_LDAP_NO_GROUP_CONFIGURED	(13)

#define	RAC_PRIV_VALID_BIT				(0x80000000)

#define OPENLDAP_OBJECTCLASS_GROUP_CN_QUERY_HEAD	"(&(objectClass=groupOfNames)(cn="
#define OPENLDAP_OBJECTCLASS_GROUP_QUERY_HEAD	"(&(objectClass=groupOfNames)("
#define	OPENLDAP_ATTRIBUTE_GROUP_MEMBERS		"member"
#define GROUP_NOT_CONFIGURED 0
#define GROUP_SID_FOUND      1
#define GROOUP_SID_NOT_FOUND  2 
#define LDAP_SECTION_NAME		"ldap"
#define LDAP_GROUP_CONFIG_FILE "/conf/openLdapGroup.conf"
#define TEMPLATE_KEY_LDAP_ROLE_GROUP_NAME		"rolegroup%dname"
#define TEMPLATE_KEY_LDAP_ROLE_GROUP_DOMAIN	"rolegroup%ddomain"
#define TEMPLATE_KEY_LDAP_ROLE_GROUP_PRIV		"rolegroup%dpriv"
#define MAX_ROLE_GROUP_DOMAIN_LEN	255
#define MAX_ROLE_GROUP_NAME_LEN		255
#define MAX_SUPPORTED_DOMAINS    40
#define LDAP_DOMAIN_SECTION_NAME			"domain names"
#define TEMPLATE_KEY_LDAP_DOMAIN_NAME "ldapdomainname%d"
#define KEY_LDAP_ENABLED			"enabled"


#define		DEFAULT_LDAP_PORT			389
#define		DEFAULT_LDAP_ATTRIBUTE_OF_USERLOGIN			"cn"
/* LDAP configuration file */
#define		LDAP_CONFIG_FILE			"/conf/ldap.conf"
#define		LDAP_CONFIG_FILE_DISABLED		"/conf/ldap.conf.disabled"

/* config file key strings */
#define	LDAP_CFG_STR_IP_ADDRESS		"host"
#define	LDAP_CFG_STR_SEARCH_BASE	"base"
#define	LDAP_CFG_STR_BINDDN		"binddn"
#define	LDAP_CFG_STR_BINDPWD		"bindpw"
#define	LDAP_CFG_STR_BINDPWD		"bindpw"
#define	LDAP_CFG_STR_ATTRIBUTE_OF_USERLOGIN	"attribute_userlogin"
#define LDAP_CFG_PORT_NUM		"port"
#define LDAP_CFG_PAM_PWD		"pam_password"
#define LDAP_CFG_NSS_RECONNECT_TRIES	"nss_reconnect_tries"
#define LDAP_CFG_SSL_ENABLED		"sslenable"

#pragma pack (1)
typedef struct {
	unsigned char Enable;
	unsigned char SSLEnable;
	unsigned short PortNum;
	unsigned char IPAddr[256];
	unsigned char Password[48];
	unsigned char BindDN[64];
	unsigned char SearchBase[128];
	unsigned char AttributeOfUserLogin[8];
	unsigned char DefaultRole;
} LDAPCONFIG;

typedef struct __tag_LDAPCONFIG
{
	uint8 LDAPRoleGroupNameStrlen;
	uint8 LDAPRoleGroupNameStr[256];
	uint8 LDAPRoleGroupDomainStrlen;
	uint8 LDAPRoleGroupDomainStr[256];
	uint32 LDAPRoleGroupPrivilege;
}PACKED LDAP_Config_T;


#pragma pack ()

int setldapconfig(LDAPCONFIG *);
int getldapconfig(LDAPCONFIG *);
int GetLDAPConfig(LDAP_Config_T* LDAPcfg,int* numrolegroups);
int SetLDAPConfig(LDAP_Config_T* ldapcfg,int rolegroupindex_onebased);
int SetLDAPGroupConfig(INT8U ldapcfg);


#endif /* __LDAP_H__ */
