/****************************************************************
****************************************************************
**                                                            **
**    (C)Copyright 2010-2011, American Megatrends Inc.        **
**                                                            **
**            All Rights Reserved.                            **
**                                                            **
**        5555, Oakbrook Pkwy, Norcross,                      **
**                                                            **
**        Georgia - 30093, USA. Phone-(770)-246-8600.         **
**                                                            **
****************************************************************
*****************************************************************
*
* pam_helper.h
* PAM Helper Library File
*
* Author: Jothiram Selvam <jothirams@ami.com>
*
*****************************************************************/
#ifndef LIBPAM_HELPER_
#define LIBPAM_HELPER_

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define WAIT_INFINITE   -1
#define WAIT_NONE       0

#define PAM_HELPER_Q        "/var/PAMHelper"
#define DEL_USER_Q           "/var/DELPAMQ"

/* PAM UIDs */
#define IPMI_FIRST_UID 1000
#define IPMI_LAST_UID  1999
#define LDAP_FIRST_UID 2000
#define LDAP_LAST_UID  2999
#define AD_FIRST_UID   3000
#define AD_LAST_UID    3999
	

/* PAM Helper Actions */
#define ADD_PAM_USER            1
#define GET_USERINFO_BY_NAME    2
#define GET_USERINFO_BY_UID     3
#define DEL_PAM_USER            4
#define PAM_RESPONSE            0x80

/* PAM Helper Return Codes */
#define INVALID_TBL_TYPE                    -1
#define INVALID_ACTION_REQUEST              -2
#define PAM_USER_NOT_FOUND                  -3
#define PAM_USER_ADDED_SUCCESSFULLY         1
#define PAM_USER_RETRIEVED_SUCCESSFULLY     2
#define PAM_USER_DELETED_SUCCESSFULLY       3


#define PAMH_CREATE_Q(QUEUE)                                    \
if (-1 == mkfifo (QUEUE, 0777) && (errno != EEXIST))            \
{                                                               \
    TCRIT (" %s : error creating queue %s\n", __FILE__, QUEUE);  \
}

#define PAMH_OPEN_Q(QUEUE, handle)                              \
handle = open (QUEUE, O_RDWR)                        		\

#define PAMH_GET_FROM_Q(buf, size, handle, perr)            \
{                                                           \
    int readlen = 0, left, len;                             \
    left = size;                                            \
    while( left > 0 )                                       \
    {                                                       \
        len = read (handle, (unsigned char*)buf + readlen, left); \
        if (len < 0 )                                       \
        {                                                   \
            if (errno == EINTR || errno == EAGAIN )         \
            {                                               \
                continue;                                   \
            }                                               \
            else                                            \
            {                                               \
                *perr = -1;                                 \
                break;                                      \
            }                                               \
        }                                                   \
        readlen += len;                                     \
        left -= len;                                        \
    }                                                       \
    *perr = readlen;                                        \
}

#define PAMH_ADD_TO_Q(buf, size, handle, perr)               \
    *perr = write (handle, buf, size)      

/* Adding string to buffer and
 *  * providing the buffer ptr to reference ptr */
#define ADD_STRTOBUF_REF(ref_ptr,str,init_buf,buffer,buflen) \
        if( (buffer - init_buf + strlen(str)) >= buflen )\
                { return -1;}\
        strcpy(buffer,str);\
        ref_ptr = buffer;\
        buffer = buffer + strlen(buffer) + 1;\

#define QUEUE_NAME          64
#define PAM_USERNAME        64

/* PAM Helper Table - Supported PAM types */
#define NUM_OF_TBLTYPES     2
enum pam_usr_tbltype 
{
    PAM_LDAP_TBL = 0,
    PAM_AD_TBL
 };

/* PAM User Details */
typedef struct
{
    char name[PAM_USERNAME];
    unsigned int  priv;
    unsigned int  uid;
} __attribute__ ((packed)) pamusr_t;

/* PAM User Req/Res Info */
typedef struct
{
    unsigned int  action;
    enum pam_usr_tbltype pt;
    char srcq[QUEUE_NAME];
    int  ret;
    pamusr_t usr;
} __attribute__ ((packed)) pamusrpkt_t;

extern int post_pam_userinfo (pamusrpkt_t* pu, char *queue);
extern int get_pam_userinfo (pamusrpkt_t* pu, char *queue, int handle, unsigned int num_ms);
extern void remove_pam_user (char *uname, int table);
#endif /* LIBPAM_HELPER_ */
