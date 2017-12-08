/* ***************************-*- andrew-c -*-****************************** */
/* Filename:    ipmi_userifc.h                                               */
/* Author:      Andrew McCallum (andrewm@ami.com)                            */
/* Format:      Indent: 4 spaces, no tabs                                    */
/* Created:     09/01/2004                                                   */
/* Modified:    09/16/2004                                                   */
/* Description: Function prototypes for IPMI/linux user functions            */
/* ************************************************************************* */

#ifndef IPMI_USERIFC_H_
#define IPMI_USERIFC_H_

/** \file ipmi_userifc.h
 *  \brief Public interface for linux interface to IPMI user functions
 *  
 *  These functions are contained within libuserm, and are called by the IPMI
 *  stack to handle linux level details of creating and modifying users and
 *  some minor user permissions.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define MAX_PASSWD_LEN      ( 20 )

/*Variabled used for creating individual user folders in /conf and the ssh related login fr te user login.*/
#define CONF_USERS_DIR "/conf/user_home"
#define SSH_DIR ".ssh"
#define SSH_AUTH_KEY "authorized_keys"
#define DUMMY_SSH_DIR "._ssh"

#define MAX_USRPATH_LEN		64
#define MAX_CMD_LEN			25


#define USER_HOMEDIR_PATH(UserName,PathName)\
	sprintf((char *)PathName,"%s/%s/",CONF_USERS_DIR,UserName)\

int CreateSSHUserDir(char *UserName);
int CreateSSHAuthKeyFile(char *UserName);
int RenameUserSSHDir(char *UserName);
int RestoreUsrSSHDir(char *UserName);
int RenameUserDir(char *UserName,char *NewUser);
int RemoveUsrDir(char *UserName);



/** \brief Set the username for an IPMI UID
 *
 *  This command takes the IPMI SetUserName data packet and creates a new
 *  user with the specified username.  It converts the IPMI UID into a linux
 *  UID and creates the new user with that UID.  The user has no password as
 *  created, and the password field is locked in /etc/shadow to prevent login.
 *  This function also creates the appropriate entries in /etc/clearpasswd and
 *  /etc/webshadow
 *
 *  \param ipmi_data A pointer to the first byte of the IPMI SetUserName
 *  command data
 *
 *  \retval <0 (Negative of an IPMI completion code) on failure
 *  \retval 0 Success
 */
int LINUX_SetUserName( uint8_t *ipmi_data );


/** \brief Get the username associated with an IPMI UID
 *
 *  This command looks up the username currently associated with the IPMI
 *  UID, as specified in the GetUserName IPMI command.  The username is
 *  looked up from /etc/passwd.
 *
 *  \param ipmi_data A pointer to the first byte of the GetUserName IPMI
 *  command data
 *  \param linux_username A pointer to at least 16 bytes of storage that
 *  will be filled with the username.  If the username is less than 16 bytes
 *  long, the string will be NULL terminated.
 *
 *  \retval <0 (Negative of an IPMI completion code) on failure,
 *  contents of linux_username undefined
 *  \retval 0 Success
 */
int LINUX_GetUserName( uint8_t *ipmi_data, char *linux_username );


/** \brief Set a user's password
 *
 *  This command takes the IPMI SetUserPassword command data as an argument and
 *  sets the specified user's password from this data.  Other functions of the
 *  SetUserPassword command are not currently supported.  16 and 20 byte
 *  passwords are supported.
 *
 *  \param ipmi_data A pointer to the first byte of the SetUserPassword IPMI
 *  command data
 *
 *  \retval <0 (Negative of an IPMI completion code) on failure
 *  \retval 0 Success
 */
int LINUX_SetUserPassword( uint8_t *ipmi_data );


/** \brief Set a root user password
 *
 *  This command takes the IPMI SetRootPassword command data as an argument and
 *  sets the linux root user password from this data.  
 *
 *  \param operation specifies action to done.
 *  \param  passwd A pointer to at least 8 bytes of storage that
 *  will be filled with the user password to set.
 *
 *  \retval <0 (Negative of an IPMI completion code) on failure
 *  \retval 0 Success
 */
int LINUX_SetRootPassword( uint8_t operation, uint8_t *passwd );


/** \brief Get a user's Access state
 *
 *  This command takes the IPMI GetUserAccess command data as an argument and
 *  sends the access state
 *
 *  \param ipmi_data A pointer to the first byte of the GetUserAccess IPMI
 *  command data
 *
 *  \retval <0 (Negative of an IPMI completion code) on failure
 *  \retval 0 Success
 */

int LINUX_GetUserAccess( uint8_t ipmi_uid,uint8_t *AccessState);

/** \brief User has no network access
 *
 *  Setting this value as the access_type in the \a LINUX_SetUserAccess
 *  function means the user will not be allowed to use any of the LAN features
 *  of the SPBMC, including ssh, web access, and video redirection
 */
#define NONETACCESS         ( 0x00 )

/** \brief User has network access
 *
 *  Setting this value as the access_type in the \a LINUX_SetUserAccess
 *  function means the user may use all of the LAN features of the SPBMC,
 *  including ssh, web access, and video redirection
 */
#define NETACCESS           ( 0x01 )


/** \brief Set network access permissions for a user
 *
 *  This function takes an IPMI UID and adds or removes the corresponding
 *  user to various networking groups depending on the value of access_type.
 *
 *  \param ipmi_uid The IPMI UID of an existing user
 *  \param access_type Set \c NONETACCESS to make sure the user cannot
 *  access the SPBMC through the network, and set \c NETACCESS to
 *  grant network access
 *
 *  \retval <0 (Negative of an IPMI completion code) on failure
 *  \retval 0 Success
 */
int LINUX_SetUserAccess( uint8_t ipmi_uid, uint8_t access_type );


/** \brief Obtain the linux username given the IPMI UID
 *
 *  This function copies the username string into \i linux_username
 *  that corresponds with the passed IPMI UID.
 *
 *  \param ipmi_uid The IPMI UID of an existing user
 *  \param linux_username Pointer to char string that will be filled
 *  with the username that corresponds with the IPMI UID.
 *
 *  \retval 0 Success
 *  \retval <0 (Negative of an IPMI completion code) on failure
 */
int username_from_ipmi_uid( uint8_t ipmi_uid, char *linux_username );


/** \brief Obtain the cleartext password for a given username
 *
 *  This function reads the cleartext password for a given username
 *  into the \i password field.  Yes, this is terrible for security,
 *  but it's required for IPMI.
 *
 *  \param ipmi_uid The IPMI UID of an existing user
 *  \param password A pointer to a data buffer to hold the password.
 *  If the password is fewer than password_len bytes long, it will be
 *  NULL terminated.
 *  \param password_len The amount of storage pointed to by \i password
 *
 *  \retval -1 Failure
 *  \retval 0 Success
 */
int get_cleartext_password( char *username, char *password,
                            size_t password_len );


#ifdef __cplusplus
}
#endif


#endif
