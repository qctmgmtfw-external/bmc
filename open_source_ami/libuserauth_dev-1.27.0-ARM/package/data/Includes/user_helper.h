/*****************************************************************
 * ******************************************************************
 * ***                                                            ***
 * ***        (C)Copyright 2011, American Megatrends Inc.         ***
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
 * * Filename: user_helper.h
 * *
 * * Description: PAM Handler Related Function Prototypes               
 * *
 * * Author: Sridharch(sridharch@amiindia.co.in)
 * *
 * ******************************************************************/

#define SEL_AUTO_LOGOUT	    2
/**
*@fn SELLogoutAudit
*@brief Logs the Logout/AutoLogout Audit
*@param pamh - handler for pam authentication
*@param ipadd- ipaddress of the host.
*@param username - Userame for authentication.
*@param service-name of service
**/
extern void SELLogoutAudit (pam_handle_t *pamh, char *ip_addr, char *username, char *service);

/**
 * @fn GetPAMItems
 * @ brief This function is used to get the information from the pam handler.
 *@param pamh - handler for pam authentication
 * @ ipaddr  	ipaddress of the host
 * @ uname	name of the user
 * @ service	service  name
 **/
extern void GetPAMItems ( pam_handle_t *pamh, char *ip_addr, char *username, char *service);

/**
 * @fn GetModuleCountEnv
 * @ brief This function is used to get or set the module count environment variable.
 * @param pamh - handler for pam authentication
 * @param cntr - holds the module traversed count
 **/
extern void GetModuleCountEnv(pam_handle_t *pamh, int *cntr);

/**
 * @fn SELLoginAudit
 * @ brief This function is used logs the audit for login and logfailed case.
 * @param pamh - handler for pam authentication
 * @param hSession - session handle
 * @ ipaddr    -ipaddress of the host
 * @ uname   -name of the user
 * @ service   -service  name
 * @ state    - gives the login or logfailed case.
 **/
extern void SELLoginAudit (pam_handle_t *pamh, void *hSession, char *service, char*uname, char *ipaddr, uint8 state);

/**
*@fn WebAuditLogout
*@brief fn calls the close session for logout and autologout events
*@param pamh - handler for pam authentication
*@param AuditState - represents the logout or autologut
*@returns 0 on success.
*/
extern int WebAuditLogout (pam_handle_t *pamh, INT8U AuditState);

/**
*@fn DoPAMAuthentication
*@brief Check if the user exists in the database listed in /etc/pam.d/webgo
*@param pamh - handler for pam authentication
*@param username - requested username from web
*@param userpriv-userpriv filled relative to GID
*@param service-service name
*@param ipadd- ip address 
  @returns 0 on success.
*/
extern int DoPAMAuthentication(pam_handle_t **pamh, char* username,int* userpriv, char *service, char *ipadd);

extern int _set_auth_tok( pam_handle_t *pamh,int flags, int argc, const char **argv );
extern int converse( pam_handle_t *pamh,int nargs,struct pam_message **message, struct pam_response **response );



