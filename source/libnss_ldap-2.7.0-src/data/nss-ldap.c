/*****************************************************************
******************************************************************
***                                                            ***
***        (C)Copyright 2010, American Megatrends Inc.         ***
***                                                            ***
***                    All Rights Reserved                     ***
***                                                            ***
***       5555 Oakbrook Parkway, Norcross, GA 30093, USA       ***
***                                                            ***
***                     Phone 770.246.8600                     ***
***                                                            ***
******************************************************************
******************************************************************
******************************************************************
* 
* * Filename: nss-ipmi.c
* *
* * Description: API's of Network Service 
* * Switch with LDAP Authenitcation scheme
* *
* * Author: Siva Prakash R (sivaprakashr@amiindia.co.in)
* *
* ***************************************************************/


#include <stdio.h>
#include <sys/types.h>
#include <pwd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "iniparser.h"
#include "dictionary.h"
#include "dbgout.h"
#include "pam_helper.h"
#include "nss.h"
#include "user_auth.h"
#include "userprivilege.h"
#include "featuredef.h"
#include "ldapconf.h"

#define NSS_LDAP_Q		"/var/NSSLDAPQ" 
#define WAIT_1000_MS                    1000

extern int post_pam_userinfo (pamusrpkt_t* pu, char *queue);
extern int get_pam_userinfo (pamusrpkt_t* pu, char *queue, int handle, unsigned int num_ms);

int ValidateLDAPUser(char * username);


/*
 * @ fn		- nss_ldap_getpwent_r
 * @ brief	- get passwd file entry reentrantly
 * @ return	- always success
 */
enum nss_status
_nss_ldap_getpwent_r (struct passwd *pwd, char *buffer, size_t buflen,
			int *errnop)
{
	TDBG("entering _nss_ipmi_getpwent_r and pwd is %s\n",pwd->pw_name);
	enum nss_status result = NSS_STATUS_SUCCESS; 
	*errnop = 0;
	if(0)
	{
		buffer=buffer;
		buflen=buflen;
	}
	return result;
}


/*
 * @ fn 		- nss_ldap_getpwnam_r
 * @ brief 		- Get User Info using ssh user name.
 * @ name		- SSH logged user name
 * @ pwd		- Passwd structure to fill
 */
enum nss_status
_nss_ldap_getpwnam_r ( char *name, struct passwd *pwd,
			char *buffer, size_t buflen, int *errnop)
{

	TDBG("\t entering _nss_ldap_getpwnam_r with user name %s \n",name);
	
	enum nss_status result = NSS_STATUS_SUCCESS;
	int ret = 0;
	char *init_buf=NULL;
	pamusrpkt_t nss_req, nss_res;
	int ldap_handle;
	char defshell[MAX_SHELL_NAME_LENGTH]={0};
	
	if(name == NULL || name[0]=='\0' )
	{
		TDBG("null user!!!\n");
		return NSS_STATUS_NOTFOUND;
	}

	/* Retrieve user by name */
	memset (&nss_req, 0, sizeof(pamusrpkt_t));
	memset (&nss_res, 0, sizeof(pamusrpkt_t));

	nss_req.action = GET_USERINFO_BY_NAME;
	nss_req.pt = PAM_LDAP_TBL;
	strncpy (nss_req.usr.name, name, PAM_USERNAME);
	strncpy (nss_req.srcq, NSS_LDAP_Q, QUEUE_NAME-1);

	PAMH_CREATE_Q(NSS_LDAP_Q);
	PAMH_OPEN_Q(NSS_LDAP_Q, ldap_handle);
	ret = post_pam_userinfo(&nss_req, PAM_HELPER_Q);
	if (ret < 0)
	{
		TCRIT (" %s : error openig queue %s \n", __FILE__, PAM_HELPER_Q );
		result = NSS_STATUS_UNAVAIL;
		close(ldap_handle);
		return result;
	}
	ret = get_pam_userinfo (&nss_res, NSS_LDAP_Q, ldap_handle, WAIT_1000_MS);
	close(ldap_handle);

	if (nss_res.action == PAM_RESPONSE)
	{
		if (nss_res.ret == PAM_USER_RETRIEVED_SUCCESSFULLY)
		{
			TDBG ("Name : %s\n", nss_res.usr.name);
			TDBG ("UID: %d\n", nss_res.usr.uid);
			TDBG ("Priv: %lx\n", nss_res.usr.priv);
		}
		else if (nss_res.ret == PAM_USER_NOT_FOUND)
		{
			if(0 == ValidateLDAPUser(nss_req.usr.name))
			{
				PAMH_OPEN_Q(NSS_LDAP_Q, ldap_handle);
				ret = post_pam_userinfo(&nss_req, PAM_HELPER_Q);
				if (ret < 0)
				{
					TCRIT (" %s : error openig queue %s \n", __FILE__, PAM_HELPER_Q );
					result = NSS_STATUS_UNAVAIL;
					close(ldap_handle);
					return result;
				}
				ret = get_pam_userinfo (&nss_res, NSS_LDAP_Q, ldap_handle, WAIT_1000_MS);
				close(ldap_handle);
				if(nss_res.action != PAM_RESPONSE && nss_res.ret != PAM_USER_RETRIEVED_SUCCESSFULLY)
				{
					TDBG ("PAM User: %s : not found\n", nss_req.usr.name);
					result = NSS_STATUS_UNAVAIL;
					return result;
				}
			}
			else
			{
				TDBG ("PAM User: %s : not found\n", nss_req.usr.name);
				result = NSS_STATUS_UNAVAIL;
				return result;
			}
		}
		else
		{
			TDBG ("PAM user return code not valid: %d\n", nss_res.ret);
			result = NSS_STATUS_UNAVAIL;
			return result;
		}
	}
	else
	{
		TDBG ("Error: Not a PAM Respones\n");
		result = NSS_STATUS_UNAVAIL;
		return result;
	}

	init_buf = buffer;
	//strncpy (pwd->pw_name, nss_res.usr.name, strlen(nss_res.usr.name));
	ADD_STRTOBUF_REF(pwd->pw_name, nss_res.usr.name, init_buf, buffer, buflen);
	pwd->pw_uid = nss_res.usr.uid;
	getUsrGrpgid(nss_res.usr.name,&pwd->pw_gid);
	GetMacrodefine_string("CONFIG_SPX_FEATURE_GLOBAL_DEFAULT_SHELL",defshell);
	if(strlen(defshell)!=0)
	{
		strncpy(pwd->pw_shell ,defshell ,MAX_SHELL_NAME_LENGTH);
	}
	*errnop = 0;

	return result;
}


/*
 * @ fn 		- nss_ldap_getpwuid_r
 * @ brief 		- Get User Info using ssh uid.
 * @ uid		- user id, used to get the user info from pam_helper
 * @ pwd		- Passwd structure to fill
 */
enum nss_status
_nss_ldap_getpwuid_r (uid_t uid, struct passwd *pwd,
			char *buffer, size_t buflen, int *errnop)
{
	TDBG("entering _nss_ipmi_getpwuid_r uid - %d \n",uid);
	enum nss_status result = NSS_STATUS_SUCCESS;
	int ret = 0;
	char *init_buf=NULL;
	pamusrpkt_t nss_req, nss_res;
	int ldap_handle;
	char defshell[MAX_SHELL_NAME_LENGTH]={0};

	memset (&nss_req, 0, sizeof(pamusrpkt_t));
	memset (&nss_res, 0, sizeof(pamusrpkt_t));
	
	nss_req.action = GET_USERINFO_BY_UID;
	nss_req.pt = PAM_LDAP_TBL;
	nss_req.usr.uid = uid;

	strncpy (nss_req.srcq, NSS_LDAP_Q, QUEUE_NAME-1);

	PAMH_CREATE_Q(NSS_LDAP_Q);
	PAMH_OPEN_Q(NSS_LDAP_Q, ldap_handle);
	ret = post_pam_userinfo(&nss_req, PAM_HELPER_Q);
	if (ret < 0)
	{
		TCRIT (" %s : error openig queue %s \n", __FILE__, PAM_HELPER_Q );
		result = NSS_STATUS_UNAVAIL;
		close(ldap_handle);
		return result;
	}
	ret = get_pam_userinfo (&nss_res, NSS_LDAP_Q, ldap_handle, WAIT_1000_MS);
	close(ldap_handle);

	if (nss_res.action == PAM_RESPONSE)
	{
		if (nss_res.ret == PAM_USER_RETRIEVED_SUCCESSFULLY)
		{
			TDBG ("Name : %s\n", nss_res.usr.name);
			TDBG ("UID: %d\n", nss_res.usr.uid);
			TDBG ("Priv: %lx\n", nss_res.usr.priv);
		}
		else if (nss_res.ret == PAM_USER_NOT_FOUND)
		{
			TDBG ("PAM User: %s : not found\n", nss_req.usr.name);
			result = NSS_STATUS_UNAVAIL; 
			return result;
		}
		else
		{
			TDBG ("PAM user return code not valid: %d\n", nss_res.ret);
			return result;
		}
	}
	else
	{
		TDBG ("Error: Not a PAM Respones\n");
		result = NSS_STATUS_UNAVAIL; 
		return result;
	}    
			
	//strncpy (pwd->pw_name, nss_res.usr.name, strlen(nss_res.usr.name));
	init_buf = buffer;
	ADD_STRTOBUF_REF(pwd->pw_name, nss_res.usr.name, init_buf, buffer, buflen);
	pwd->pw_uid = nss_res.usr.uid;
	getUsrGrpgid(nss_res.usr.name,&pwd->pw_gid);
	GetMacrodefine_string("CONFIG_SPX_FEATURE_GLOBAL_DEFAULT_SHELL",defshell);
	if(strlen(defshell)!=0)
	{
		strncpy(pwd->pw_shell,defshell,MAX_SHELL_NAME_LENGTH);
	}
	*errnop = 0;	
	return result;
}

/*
 * @ fn 		- ValidateLdapUser
 * @ brief 		- Get User Info from ldap server.
 * @ username	- User name to get information 
 * @ ret		- Returns 0 on success and -1 on error 
 */

int ValidateLDAPUser(char * username)
{
	pamusrpkt_t upkt_req, upkt_res;
	int ret =0;
	int open_handle;
	
	if(0 != IsLDAPUser(username) )
	{
		TDBG("Not a ldap user\n");
		return -1;
	}
	
	memset(&upkt_req,0,sizeof(pamusrpkt_t));
	memset(&upkt_res,0,sizeof(pamusrpkt_t));
	
	upkt_req.action = ADD_PAM_USER;
	upkt_req.pt = PAM_LDAP_TBL;
	
	strncpy (upkt_req.usr.name, username, PAM_USERNAME);
	strncpy (upkt_req.srcq, NSS_LDAP_Q, QUEUE_NAME-1);
	
	PAMH_CREATE_Q(NSS_LDAP_Q);
	PAMH_OPEN_Q(NSS_LDAP_Q, open_handle);
	
	ret = post_pam_userinfo(&upkt_req, PAM_HELPER_Q);
	if (ret < 0)
	{
		TCRIT (" %s : error openig queue %s \n", __FILE__, PAM_HELPER_Q );
		close(open_handle);
		return -1;
	}
	ret = get_pam_userinfo (&upkt_res, NSS_LDAP_Q, open_handle, WAIT_1000_MS);
	close(open_handle);
	if(upkt_res.action != PAM_RESPONSE && upkt_res.ret != PAM_USER_ADDED_SUCCESSFULLY)
	{
		return -1;
	}
	
	return ret;
}
