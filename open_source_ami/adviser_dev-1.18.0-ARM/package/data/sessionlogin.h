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
 *****************************************************************/

#ifndef __LOGIN_CONF_H__
#define __LOGIN_CONF_H__

#include "coreTypes.h"

#include "sessioncfg.h"
#define DEFAULT_QUALITY_LEVEL			0
#define DEFAULT_FPS				35

//#include "sessiondefs.h"
/* ---> Taken from sessiondefs.h */
#define INVALID_SESSION				0
#define VALID_SESSION				1

#define TOKEN_LEN				(MAX_TOKEN_LEN)
#define HASH_SIZE				16

/* user permission */
#define USER_RO					1
#define USER_RW					2

/* validation types */
#define VALIDATE_VIDEO_SESSION			0
#define VALIDATE_CD_SESSION			1
#define VALIDATE_FLOPPY_SESSION			2

/* <---- Taken from sessiondefs.h */

#define LOGIN_TOKEN_FILE			"/var/tmp/logintoken"
#define LOGIN_TOKEN_LOCK_FILE			"/var/tmp/logintoken.lck"
#define SESSION_TIMEOUT_CONF			"/etc/sessiontimeout.conf"

/* token status */
#define STATUS_CREATED_NOT_USED			1
#define STATUS_VALIDATED			2
#define STATUS_POSSIBLE_RESTART			3

typedef struct
{
	char session_token [TOKEN_LEN + 2]; /* TOKEN_LEN + 1 has permision level */
	unsigned long stoken_uptime; /* session token creation uptime */
	int	stime_valid;			/* session validity time out limit */
	int status;					/* status of the token  */

} stoken_gen_t;

/********************** FUNCTION PROTOTYPES ***************************/
int get_session_token (char* stokenperm, int perm);
int change_stoken_status (char* stoken, int status);
BOOL validate_session_token (char* stoken, int type);
int get_session_validity_time (void);
int set_session_validity_time(int tsecs);
void create_hash_session_token (char* stoken, char *htoken);
int remove_session_token (char* stoken);
int get_num_sentry (int* num_sentry);

/* Macros to open & close the Login Token Lock File */
#define OPEN_LTF_LOCK_FILE( fd ) \
	fd = fopen (LOGIN_TOKEN_LOCK_FILE, "a+"); \
	if(NULL == lockFile) \
	{ \
		TCRIT ("error in creating the file %s.", LOGIN_TOKEN_LOCK_FILE); \
		return ERR_FILE_CREATE; \
	} \

#define CLOSE_LTF_LOCK_FILE( fd )   fclose (fd)

/* Macros for file lock and unlock */
#define LOCK_LTF_FILE_WRITE( fd )   file_lock_write(fd)
#define LOCK_LTF_FILE_READ( fd )    file_lock_read(fd)
#define UNLOCK_LTF_FILE( fd )       file_unlock(fd)

#endif /* __LOGIN_CONF_H__ */



