/*****************************************************************
******************************************************************
***                                                            ***
***        (C)Copyright 2008, American Megatrends Inc.         ***
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
* Filename: pppConf.h
*
* Author: Chennakesava Rao Arnoori
*
******************************************************************/
#ifndef _PPPCONF_H
#define _PPPCONF_H 1

#include <coreTypes.h>
#include "usermapis.h"

/**
 * \def MAX_IP_LEN 
 * \brief Maximum ip string length.
 * 
 */
#define MAX_IP_LEN ( 32 )

/**
 * \def PAP_SECRECT_FILE
 * \brief pap secrect file on the montavista linux.
 * 
 */
#define PAP_SECRECT_FILE "/etc/ppp/pap-secrets"


/**
 * \def CHAP_SECRECT_FILE
 * \brief chap secrect file on the montavista linux.
 * 
 */
#define CHAP_SECRECT_FILE "/etc/ppp/chap-secrets"

/**
 * \def MAX_FILE_READ_WRITE_BUFF_SIZE
 * \brief Maximum buffer being allocated to read the date from the secrets file.
 * 
 */
#define MAX_FILE_READ_WRITE_BUFF_SIZE 255

/**
 * \def DEFAULT_SECRET_STRING
 * \brief For the Pap the default password is " "
 * 
 */
#define DEFAULT_SECRET_STRING  "\" \"" 

/**
 * \def DEFAULT_HEADER_STRING
 * \brief To give the  user some idea of  what field is what.
 * 
 */
#define DEFAULT_HEADER_STRING  "#Client \t\tserver\t\tsecret\t\tacceptable local ip\n"

/**
 * \def DIALIN_CARD_SERVER_NAME
 * \brief Default card server name used while the card is in dialin.This differentiates the
 *	    dialin and dialout , bcuz the dialout will use the peer name as the servername, while
 *	    for the dialin the card is the server.
 * 
 */
#define DIALIN_CARD_SERVER_NAME "g3_card" 	

/**
 * \def DIALOUT_CARD_SERVER_NAME
 * \brief Default card server name used while the card is in dialin.This differentiates the
 *	    dialin and dialout , bcuz the dialout will use the peer name as the servername, while
 *	    for the dialin the card is the server.
 * 
 */
#define DIALOUT_CARD_SERVER_NAME "*"

/**
 * \def ISP_NUMBER_STR
 * \brief Environment varible to be used in the /etc/ppp/ppp-on script.
 * 
 */ 	
#define ISP_NUMBER_STR "ISP_NUMBER"

/**
 * \def MAX_INTERFACE_RETRY_COUNT
 * \brief Max tries to make sure the interface ( ppp0 ) is up or not
 * 
 */ 	
#define MAX_INTERFACE_RETRY_COUNT 5        

/**
 * \def ETC_OPTIONS_FILE
 * \brief The file name where the user name option is set.This is used in the /etc/ppp/ppp-on file
 * 
 */ 	
#define ETC_OPTIONS_FILE "/etc/ppp/dailout-user"    

/**
 * \def DIALOUT_SERVER_STRING
 * \brief Default dialout string for server field.
 * 
 */ 	
#define DIALOUT_SERVER_STRING "*"

/**
 * \def DIALOUT_MAGIC_STRING
 * \brief Default dialout magic string needed before the dialout user.
 * 
 */
#define DIALOUT_MAGIC_STRING "###DialoutUsers\n"

//blocking call (interrupable by signals)
#define readw_lock(fd, offset, whence, len)     \
                    lock_reg (fd, F_SETLKW, F_RDLCK, offset, whence, len)

#define writew_lock(fd, offset, whence, len)    \
                    lock_reg (fd, F_SETLKW, F_WRLCK, offset, whence, len)


//RELEASING locks
#define un_lock(fd, offset, whence, len)        \
                    lock_reg (fd, F_SETLK, F_UNLCK, offset, whence, len)




/**
 * \struct secretFileRecord
 * \brief Represents One line in both chap-secrets and pap-secrets file.
 * 
 */

typedef struct secretFileRecord
{
	/// pppd username
    CHAR userName[ MAX_USER_NAME_LEN + 1 ];
	/// server name to be considered.
    CHAR server [ MAX_IP_LEN +1 ];
	/// password 
    CHAR secret [ MAX_PASSWORD_LEN_CLEAR + 1 ];
	/// acceptable ip
    CHAR acceptableIp [ MAX_IP_LEN +1 ];
	/// marked for deletion
    BOOL  toRemove;
	///marked for dialout
    BOOL  isDialout;
    
    
} PPPD_SECRECT_FILE_RECORD;

char * fgetline ( FILE * fp );
int isAllLineSpaces( CHAR *buffer );


#endif /* _PPPCONF_H */

