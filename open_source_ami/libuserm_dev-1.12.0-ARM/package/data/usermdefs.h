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

#ifndef __USERMLIMITS_H__
#define __USERMLIMITS_H__

#include "libipmi_struct.h"

#define UID_MIN_LOCAL_DEFAULT                       (1001)
#define UID_MAX_LOCAL_DEFAULT                       (1032)

#define GID_MIN_LOCAL_DEFAULT                       (501)
#define GID_MAX_LOCAL_DEFAULT                       (601)



#define MAX_ENTRIES_LOCAL_COMPUTE(minid,maxid)      ( ( (maxid)-(minid) ) + 1 )
#define MAX_USERS_LOCAL_DEFAULT                      MAX_ENTRIES_LOCAL_COMPUTE(UID_MIN_LOCAL_DEFAULT,UID_MAX_LOCAL_DEFAULT)
#define MAX_GROUPS_LOCAL_DEFAULT                     MAX_ENTRIES_LOCAL_COMPUTE(GID_MIN_LOCAL_DEFAULT,GID_MAX_LOCAL_DEFAULT)


/**NOTE : All Length definitions below do not include any terminating NULL**/


//It is logical to have this as sizeof(ut.ut_user) but since many clients have to know what the length is we just define it.
//but sizeof ut.ut_user is 32 now on this distro
//#define MAX_USER_NAME_LEN                            (16) //without the terminating NULL
#define MIN_USER_NAME_LEN                            (4)  //without the terminating NULL

//password lengths
#define MAX_PASSWORD_LEN_CLEAR                       (32) //without the terminating NULL
#ifdef CFG_PROJ_USERM_MIN_PASSWORD_LEN
#define MIN_PASSWORD_LEN_CLEAR                       (CFG_PROJ_USERM_MIN_PASSWORD_LEN)  //without the terminating NULL
#else
#define MIN_PASSWORD_LEN_CLEAR                       (8)  //without the terminating NULL
#endif

//there is no min password length crypted..crypted passwd is always 34 characters in unix
#define MAX_PASSWORD_LEN_HASHED                      (64) //without the terminating NULL
#define DEFAULT_SALT                                 "$1$A17c6z5w"

#define UNIX_PASSWORD_HASHED						 (34)

//description filed lengths
//in unix terminology GECOS is any description associated with the user
#define MAX_GECOS_LEN                                (32) //without the terminating NULL
//there is no MIN_GECOS_LENGTH


//arbit limit as there is no system limit
#define MAX_GROUP_NAME_LEN                           (20) //without the terminating NULL
#define MIN_GROUP_NAME_LEN                           (4) //without the terminating NULL




#endif
