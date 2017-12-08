/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2002-2003, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        6145-F, Northbelt Parkway, Norcross,                **
 **                                                            **
 **        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 ****************************************************************/
/****************************************************************
  $Header: $

  $Revision: $

  $Date: $
 *****************************************************************/
/*****************************************************************
 * File name    : sslcertificate.h
 * Author       : Anandh Mahalingam
 * Created      : Oct 16, 2003
 * SSL certificate declarations
 *****************************************************************/

#ifndef SSLCERTIFICATE_INCLUDED
#define SSLCERTIFICATE_INCLUDED

#include "Types.h"

/*  Actual SSL certificate location */
/*  GoAhead can use this while starting to look for actual
    certificate files.
    CURI client and GoAhead can use this for uploading the
    certificate and key file to the card.
 */

#define DEFAULT_CERT_FILE   "/usr/local/www/certs/server.pem"
#define DEFAULT_KEY_FILE    "/usr/local/www/certs/privkey.pem"

#define CERT_BASE_LOCATION  "/etc/"
#define CONF_LOCATION  "/conf/"
#define TEMP_LOCATION "/tmp/"

#define CERT_FILE "actualcert.pem"
#define KEY_FILE "actualprivkey.pem"

#define ACTUAL_CERT_FILE    CERT_BASE_LOCATION CERT_FILE
#define ACTUAL_KEY_FILE     CERT_BASE_LOCATION KEY_FILE
#define ACTUAL_CA_FILE      CERT_BASE_LOCATION CERT_FILE
#define ACTUAL_CA_PATH      CERT_BASE_LOCATION
#define ACTUAL_AD_CA_FILE    CERT_BASE_LOCATION "active_dir_ca_file.pem"

#define ACTUAL_CSR_FILE	    CONF_LOCATION "server.csr"

#define TEMP_ASCII_CERT		"/var/certtmp"
#define TEMP_ASCII_CAFILE		"/var/catmp"

#define CONF_CERT_FILE    CONF_LOCATION CERT_FILE
#define CONF_KEY_FILE     CONF_LOCATION KEY_FILE
#define CONF_AD_CA_FILE	  CONF_LOCATION "active_dir_ca_file.pem"

#define TEMP_CERT_FILE TEMP_LOCATION CERT_FILE
#define TEMP_KEY_FILE TEMP_LOCATION KEY_FILE

#define CSR_GEN_CONFIG_FILE	"/var/sslconf"


/* SSL encrypted file identifier  */
#define SSL_ENCRYPTED_IDENTIFIER  "ENCRYPTED"

#define SSL_CERT_EXPIRED     0x80
#define SSL_CERT_UNTRUSTED   0x81

/*  We expect the certificate and key file to be less than 8KB
    in size
 */
#define MAX_PERMITTED_CERT_SIZE     (8*1024)
#define MAX_PERMITTED_PRIVKEY_SIZE  (8*1024)

/** Token Id - 9 SSL Configuration **/ 
#define COMMON_NAME_STR_LEN    64
#define ORG_NAME_STR_LEN 	   64
#define ORG_UNIT_STR_LEN	   64
#define LOCALITY_NAME_STR_LEN  128
#define STATE_NAME_STR_LEN     128
#define COUNTRY_CODE_STR_LEN   2
#define EMAIL_ADDR_LEN         128
#define SSL_MAX_LEN			   64

typedef struct {
	INT32U   CRT_Keylength;
	char		CRT_CommonName [COMMON_NAME_STR_LEN + 1];
	char 	 CRT_Organisation  [ORG_NAME_STR_LEN + 1];  
	char		CRT_OrganisationUnit [ ORG_UNIT_STR_LEN + 1]; 
	char 	 CRT_LocalityName [LOCALITY_NAME_STR_LEN + 1]; 
	char 	 CRT_StateName [STATE_NAME_STR_LEN + 1];
	char 	 CRT_CountryCode [COUNTRY_CODE_STR_LEN + 1];
	char 	 EmailAddress  [EMAIL_ADDR_LEN + 1];
	int CRT_ValidDays;
}PACKED SSL_T;

typedef struct {
	char Version[SSL_MAX_LEN];
	char SerialNo[SSL_MAX_LEN];
	char SignatureAlgorithm[SSL_MAX_LEN];
	char PublicKey[SSL_MAX_LEN];
	SSL_T issue_from;
	SSL_T issue_to;
	char valid_from[SSL_MAX_LEN];
	char valid_to[SSL_MAX_LEN];
} PACKED SSL_Config_T;

extern int genSSL_CRT(SSL_T *sslconfig);
extern int viewSSL_CRT(SSL_Config_T *sslconfig);
extern int verifySSL_CRT();
#endif  /*  SSLCERTIFICATE_INCLUDED */

 /************************* End of File ****************************/


