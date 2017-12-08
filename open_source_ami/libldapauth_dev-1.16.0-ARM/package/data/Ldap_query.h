/*****************************************************************
******************************************************************
**																**
**	(C)Copyright 2005-2006, American Megatrends Inc.					**
**																**
**	All Rights Reserved.											**
**																**
**	5555, OakBrook Pkwy, Suite 200,  Norcross,						**
** 																**
**	Georgia-30093, USA. Phone-(770)-246-8600.						**
**									 							**
******************************************************************
******************************************************************
*																**
*Ldap_query.h													**
*																**
*																**
*																**
*																**
******************************************************************/

#ifndef __LDAP_QUERY_H__
 #define __LDAP_QUERY_H__

#include "ldap.h"


//Mcaros for  Max lens

#define	LDAP_BASE_NUM_OF_MSG				20
#define	LDAP_MAX_NUM_OF_RESULTS				32
#define	LDAP_MAX_USER_PRV					9
#define	AD_MAX_ATTRNAME_LEN					256
#define	AD_MAX_DN_LENGTH					520
#define	AD_MAX_QUERY_LENGTH					300
#define	AD_SCHEMA_OBJNAME_MAX				64
#define	LDAP_ATTR_SAMACCOUNT_NAME_MAXSIZE	20



// mcaros for Dell atrributes and common attributes
#define	LDAP_DN_QUERY_HEAD				"distinguishedName="
#define	LDAP_CN_QUERY_HEAD				"cn="
#define	LDAP_AC_QUERY_HEAD				"userPrincipalName="
#define	LDAP_SCHEMA_DELL_PREFIX			"dell"
#define	LDAP_ATTR_MEMBEROF				"memberOf"
#define	LDAP_ATTR_OBJECTCLASS			"objectClass"
#define	LDAP_ATTR_RAC_DEVICE_MEMBERS	"dellProductMembers" /*"dellRacDeviceMembers"*/
#define	LDAP_ATTR_MEMBERS				"member"
#define	LDAP_ATTR_DISTINGUISHED_NAME	"distinguishedName"
#define	LDAP_ATTR_COMMON_NAME			"cn"
#define	LDAP_ATTR_SAMACCOUNT_NAME		"sAMAccountName"
#define	LDAP_ATTR_DELL_PRV     			"dellPrivilegeMember"
#define	LDAP_OBJECTCLASS_ASSOCIATION   	"dellIDRacAssociation"
#define	LDAP_OBJECTCLASS_PRIVILEGES    	"dellprivileges"
#define LDAP_OBJCLASS_GRP_CN_QUERY_HEAD	"(&(objectclass=group)(cn="
#define LDAP_RAC_DEV_QUERY_HEAD		"(dellProductMembers="
#define	LDAP_OBJCLASS_ASSOC_QUERY_HEAD	"(&(objectclass=dellIDRacAssociation)"
#define LDAP_OBJCLASS_PRIV_QUERY_HEAD	"(objectclass=dellprivileges)"


#define OPENLDAP_OBJECTCLASS_GROUP_CN_QUERY_HEAD	"(&(objectClass=groupOfNames)(cn="
#define	OPENLDAP_ATTRIBUTE_GROUP_MEMBERS		"member"


 //AD Sepcific attributes
#define	AD_ATTR_TOKENGROUPS				"tokenGroups"
#define	AD_ATTR_OBJECTSID				"ObjectSid"


//privilege values 
#define	LDAP_ATTR_VALUE_TRUE   			"TRUE"
#define	LDAP_ATTR_VALUE_FALSE  			"FALSE"

//AD Type and privilege mask:
#define	AD_TYPE_EXT			1
#define	AD_TYPE_SSAD		2
#define	AD_PRV_MASK			0x1ff

// result attributes type
#define	RES_TYPE_STRING		0x00
#define	RES_TYPE_BINARY		0x01


//prototypes
//invoke the ldap search 
LDAPMessage**
ldap_search_dir(
		LDAP* ld,
		char* base,
		int scope,
		char* value,
		int* msg_num,
		char** attrs
		);


//get single valued attribute from single object
struct berval*
ldap_get_attribute(
		LDAP* ld,
		char* base,
		int scope,
		char* value,
		char* attr_name,
		int type
		);

//get single valued atribute from single object , its gc search
struct berval*
ldap_get_attr_frmgc(
		LDAP* ld,
		char* base,
		int scope,
		char* value,
		char* attr_name,
		int type
		);


//get multi valued attribute from single object
struct berval*
ldap_get_attr_mul_values(
		LDAP* ld,
		char* base,
		int scope,
		char* value,
		char* attr_name,
		int type,
		int* count
		);

//get multi valued attribute from single object, its gc search


struct berval*
ldap_get_attr_mul_values_from_gc(
		LDAP* ld,
		char* base,
		int scope,
		char* value,
		char* attr_name,
		int type,
		int* count
		);


//one attribute from multiple objects
char**
ldap_get_objects(
		LDAP* ld,
		char* base,
		int scope,
		char* filter,
		char* attr,
		int* count
		);

// query the privilege objects and get the privilege.
unsigned int
QueryPrivilege(LDAP* ld,char* prv_dn);


//check for the special characters and escape it
char* 
ldap_check_special_chars(char *ldstring);


#endif  //__LDAP_QUERY_H__ 
