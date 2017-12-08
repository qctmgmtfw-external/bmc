/****************************************************************
 **                                                            **
 **    (C)Copyright 2006-2009, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
****************************************************************/
#ifndef __ADVISER_TOKEN_H__
#define __ADVISER_TOKEN_H__


#define	SESSION_TOKEN_STORE_LOCK		"/var/session_token_lock"
#define WEBTOKEN_COMM_PIPE              "/var/ActiveWebTokens"
#define WEBTOKEN_COMM_PIPE1              "/var/ActiveWebTokens1"
#define WEBTOKEN_SIZE			(32+3+1) // Last character for null character
#define VIDEO_AUTHENTICATE		1
#define VMEDIA_AUTHENTICATE		2


typedef struct
{
        int             Status;
        char            websession_token[WEBTOKEN_SIZE];
} __attribute__((packed)) websession_info_t;

/**
	@brief Creates a new token and stores it in the session token store
	@returns 0 on success and -1 on failure
*/
int		CreateToken(char *token/*OUT*/, unsigned int user_priv /*IN*/,char* user_name /*IN*/,char* client_ip /*IN*/);

/**
	@brief Validates a given token.
	@returns 0 on success and -1 on failure
*/
int		ValidateToken(char *token/*IN*/, unsigned int *p_user_priv /*OUT*/,unsigned char* user_name /*OUT*/,unsigned char* client_ip /*OUT*/,int type);

/**
	@brief InvValidates a given token.
	@returns 0 on success and -1 on failure
*/
int		InValidateToken(char *token/*IN*/);

/**
	@brief Deletes the token temporarily from the session token store
	@returns 0 on success and returns -1 on failure
*/
int		DeleteToken(char *token/*IN*/);

#endif




