/****************************************************************
 **                                                            **
 **    (C) Copyright 2006-2009, American Megatrends Inc.       **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600          **
 **                                                            **
****************************************************************/

#ifndef __ADCONFIG_H__
#define __ADCONFIG_H__

#ifdef __GNUC__
#define PACKED __attribute__ ((packed))
#else
#define PACKED
#pragma pack( 1 )
#endif

#include <coreTypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "dbgout.h"


#define OPENLDAP_CONF_FILE 	"/etc/openldap23/etc/openldap/ldap.conf"
#define CONF_LOCATION  			"/conf/"
#define CERT_BASE_LOCATION		"/etc/"
#define AD_CONFIG_FILE 			CONF_LOCATION "activedir.conf"

#define ADCERT_FILE				"adcert.pem"
#define CONF_ADCERT_FILE	  	CONF_LOCATION ADCERT_FILE
#define ACTUAL_ADCERT_FILE		CERT_BASE_LOCATION ADCERT_FILE
#define DEFAULT_ADCERT_FILE	"/usr/local/www/certs/" ADCERT_FILE

//Strings in the ini file
#define AD_SECTION_NAME		"activedirectory"
#define KEY_AD_ENABLED			"enabled"
#define KEY_SSL_ENABLED			"sslenable"
#define KEY_AD_TIMEOUT			"timeout"
#define KEY_AD_RACDOMAIN		"racdomain"
#define KEY_AD_TYPE				"adtype"
#define KEY_AD_FILTER_DC1		"adfilterdc1"
#define KEY_AD_FILTER_DC2		"adfilterdc2"
#define KEY_AD_FILTER_DC3		"adfilterdc3"


//strings for role group for ssad
#define SSAD_SECTION_NAME			"ssad"

#define MAX_ROLE_GROUPS 	5

#define MAX_SUPPORTED_DCS   3
//#define MAX_SUPPORTED_GCS    3

#define TEMPLATE_KEY_SSAD_ROLE_GROUP_NAME		"rolegroup%dname"
#define TEMPLATE_KEY_SSAD_ROLE_GROUP_DOMAIN	"rolegroup%ddomain"
#define TEMPLATE_KEY_SSAD_ROLE_GROUP_PRIV		"rolegroup%dpriv"

#define AD_DOMAIN_SECTION_NAME			"domain names"

#define MAX_SUPPORTED_DOMAINS    40

#define TEMPLATE_KEY_AD_DOMAIN_NAME "addomainname%d"

#define MAX_RAC_DOMAIN_LEN  		255
#define MAX_RAC_NAME_LEN			255

#define MAX_ROLE_GROUP_NAME_LEN		255
#define MAX_ROLE_GROUP_DOMAIN_LEN	255

#define MAX_AD_FILTER_DC_LEN		255
#define MAX_AD_FILTER_GC_LEN		255

#define MAX_AD_DOMAIN_NAMES         40

typedef struct __tag_ADCONFIG
{
	uint8 ADEnable;
	uint8 SSLEnable;
	uint32 ADTimeout;
	uint8 ADRACDomainStrlen;
	uint8 ADRACDomainStr[256];
	uint8 ADType; // 1 for extended and 2 for std, we are not using extended, only value is 2.
	uint8 ADFilterEnable;
	uint8 ADDCFilter1Len;
	uint8 ADDCFilter1[256];
	uint8 ADDCFilter2Len;
	uint8 ADDCFilter2[256];
	uint8 ADDCFilter3Len;
	uint8 ADDCFilter3[256];
}PACKED AD_Config_T;


typedef struct __tag_SSADCONFIG
{
	uint8 SSADRoleGroupNameStrlen;
	uint8 SSADRoleGroupNameStr[256];
	uint8 SSADRoleGroupDomainStrlen;
	uint8 SSADRoleGroupDomainStr[256];
	uint32 SSADRoleGroupPrivilege;
}PACKED SSAD_Config_T;

typedef struct
{
uint8 ADUserDomainNameStrlen;
uint8 ADUserDomainNameStr[256];
}PACKED AD_User_Domain_T;


int GetADConfig(AD_Config_T* adcfg);
int SetADConfig(AD_Config_T* adcfg);


//read ldap config for web page sync (enable)
//int GetLDAPGroupConfig(AD_Config_T* adcfg);



//int SetLDAPGroupConfig(AD_Config_T* adcfg);

//get the whole array of role groups
//num role groups has size of array and also returns the number of elements
//third parameter LDAP_OR_AD is used to decide whether LDAP or AD's group conf file is gonna read/write
//if LDAP_OR_AD = 1, it read/write /conf/openLdapGroup.conf    if LDAP_OR_AD=0, it read/write /conf/activedir.conf
int GetSSADConfig(SSAD_Config_T* ssadcfg,int* numrolegroups);
//give rolegorupindex and the configuration to set it to
//third parameter LDAP_OR_AD is used to decide whether LDAP or AD's group conf file is gonna read/write
//if LDAP_OR_AD = 1, it read/write /conf/openLdapGroup.conf    if LDAP_OR_AD=0, it read/write /conf/activedir.conf
int SetSSADConfig(SSAD_Config_T* ssadcfg,int rolegroupindex);


//get and set domain names

//get the whole array of domain names.
int GetDomainNames(AD_User_Domain_T * userdomain, int *numofdomain);
 //give domainindex and the configuration to set it to
int SetDomainNames(AD_User_Domain_T * userdomain, int domainindex);


#ifdef __GNUC__
#else
#define PACKED
#pragma pack(  )
#endif

#endif
