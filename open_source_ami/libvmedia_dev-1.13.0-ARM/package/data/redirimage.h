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

#ifndef __REDIR_IMAGE_H__
#define __REDIR_IMAGE_H__

#ifdef __GNUC__
#define PACKED __attribute__ ((packed))
#else
#define PACKED
#pragma pack( 1 )
#endif

/* commands */
#define START_LOCAL_IMAGE_REDIRECTION		(0xF0)
#define DEVICE_REDIRECTION_ACK				(0xF1)
#define AUTH_CMD							(0xF2)
#define BYPASS_AUTH_CMD							(0xF3)


/* ACK status */
typedef struct tag_DEV_REDIR_ACK {
	IUSB_SCSI_PACKET	iUsbScsiPkt;
#define CONNECTION_ACCEPTED		(0x01)
#define CONNECTION_DENIED		(0x02)
#define LOGIN_FAILED			(0x03)
#define CONNECTION_IN_USE		(0x04)
#define CONNECTION_PERM_DENIED		(0x05)
#define OTHER_ERROR				(0x06)
#define MEDIA_IN_DETACH_STATE	(0x07)
	uint8			ConnectionStatus;
	char			szClientIP[24];
} PACKED DEV_REDIR_ACK;

typedef struct 
{
	IUSB_SCSI_PACKET	iUsbScsiPkt;
	uint8		flags;
	union {
		struct _user_creds {
			char	username[32];
			char	password[MAX_ADV_PWD_LEN+1];
		} PACKED user_creds;
		struct _token_creds {
			char	session_token[MAX_TOKEN_LEN+1];
		} PACKED token_creds;
	} PACKED authtype;
} PACKED session_token_auth_t;

#ifndef __GNUC__
#pragma pack()
#endif

#endif /* __REDIR_IMAGE_H__ */

