/*****************************************************************
 **                                                             **
 **     (C) Copyright 2006-2009, American Megatrends Inc.       **
 **                                                             **
 **             All Rights Reserved.                            **
 **                                                             **
 **         5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                             **
 **         Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                             **
 ****************************************************************/

/**
 * Intelligent Video Transfer Protocol packet structure
**/

#ifndef __ADVISER_VIDEOPKT_H__
#define __ADVISER_VIDEOPKT_H__
#include "sessioncfg.h"
#include	"vmedia_cfg.h"
/*--------------------------------------------------------------
	List of Commands used between the
	Client & the Adaptive Video Server (ADVISER)
----------------------------------------------------------------*/

//COMMON CODES

#define IVTP_HID_PKT					(0x0001)
#define IVTP_SET_BANDWIDTH				(0x0002)
#define IVTP_SET_FPS					(0x0003)
#define IVTP_PAUSE_REDIRECTION				(0x0004)
#define IVTP_REFRESH_VIDEO_SCREEN			(0x0005)
#define IVTP_RESUME_REDIRECTION				(0x0006)
#define IVTP_SET_COMPRESSION_TYPE			(0x0007)
#define IVTP_STOP_SESSION_IMMEDIATE			(0x0008)
#define IVTP_BLANK_SCREEN				(0x0009)
#define IVTP_GET_USB_MOUSE_MODE				(0x000a)
#define IVTP_GET_FULL_SCREEN				(0x000b)
#define IVTP_ENABLE_ENCRYPTION				(0x000c)
#define IVTP_DISABLE_ENCRYPTION				(0x000d)
#define IVTP_ENCRYPTION_STATUS				(0x000e)
#define IVTP_INITIAL_ENCRYPTION_STATUS			(0x000f)
#define IVTP_BW_DETECT_REQ				(0x0010)
#define IVTP_BW_DETECT_RESP				(0x0011)
#define IVTP_VALIDATE_VIDEO_SESSION			(0x0012)
#define IVTP_VALIDATE_VIDEO_SESSION_RESPONSE		(0x0013)
#define IVTP_GET_KEYBD_LED				(0x0014)
#define IVTP_GET_WEB_TOKEN				(0x0015)
#define IVTP_MAX_SESSION_CLOSING			(0x0016)
#define IVTP_SESSION_ACCEPTED				(0x0017)
#define IVTP_MEDIA_STATE                                (0x0018)
#define IVTP_VIDEO_FRAGMENT				(0x0019)
#ifdef CONFIG_SPX_FEATURE_GLOBAL_WEB_CONSOLE_PREVIEW
#define IVTP_WEB_PREVIEWER_SESSION			(0x001A)
#define IVTP_WEB_PREVIEWER_CAPTURE_STATUS		(0x001B)
#endif
#define IVTP_SET_MOUSE_MODE				(0x001C)
#define IVTP_KVM_SHARING				(0x0020)
#define IVTP_KVM_SOCKET_STATUS                    	(0x0021)  
#define IVTP_POWER_STATUS				(0x0022)
#define IVTP_POWER_CTRL_REQST				(0x0023)
#define IVTP_POWER_CTRL_RESP				(0x0024)
#define IVTP_SERVICE_INFO				(0x0025)
#define IVTP_KVMMOUSE_MEDIA_INFO		(0x0026)
#define IVTP_ACTIVE_CLIENTS					(0x0027)


#define  KEYBD_INFO_CODE                     0x43

/*
 * Status codes for status field in ivtp_hdr_t
**/
#define	STATUS_SUCCESS			(0x00)
#define STATUS_HOST_POWERED_OFF		(0x01)
#define STATUS_SESS_WEB_LOGOUT		(0x07)
/** 
* Status codes for command ADVISER_REQ_KVM_PRIV
*/
#define STATUS_KVM_PRIV_CANCEL		(0x00) // reserved. not used for now.
#define STATUS_KVM_PRIV_REQ_USER1	(0x01) // to user 1
#define STATUS_KVM_PRIV_WAIT_USER2	(0x02) // to user 2
#define STATUS_KVM_PRIV_ACK_USER1	(0x03) // to user 1
#define STATUS_KVM_PRIV_ACK_USER2	(0x04) // to user 2
#define STATUS_KVM_PRIV_RES_USER1		(0x05) // from user 1

#define REQ_KVM_HID_PERMISION		(0x06) // from partial USer

#define STATUS_SOCKET_CLOSE (0x00)

#define KVM_REQ_ALLOWED 0
#define KVM_REQ_DENIED 1
#define KVM_REQ_PARTIAL 2
#define KVM_REQ_TIMEOUT 3 //by default grant full access.
#define POWER_CTRL_OFF_IMMEDIATE		0//Imediate power off
#define POWER_CTRL_ON					1//power on the host
#define POWER_CTRL_CYCLE				2
#define POWER_CTRL_HARD_RESET			3//Hard reset 
#define POWER_CTRL_SOFT_RESET			4 //properly shutdown

/**
 * IVTP packet header
**/
typedef struct
{
	unsigned short		type;				/**< Type of the packet **/
	unsigned long		pkt_size;			/**< Size of the remaining portion of the packet */
	unsigned short		status;				/**< status field. used for sending response	 */

} __attribute__((packed)) ivtp_hdr_t;
typedef struct
{
#define AUTH_SESSION_TOKEN              (0x00) //default
        ivtp_hdr_t      hdr;
        unsigned char   flags;
        union {
                struct _user_creds {

                        char    username[MAX_USRNAME_LEN+1];
                        char    password[MAX_ADV_PWD_LEN+1];
                } __attribute__((packed)) user_creds;
                struct _token_creds {
                        char    session_token[MAX_TOKEN_LEN+1];
                } __attribute__((packed)) token_creds;
        } __attribute__((packed)) authtype;
		char ipaddr_client[TOKENCFG_MAX_CLIENTIP_LEN+1];
} __attribute__((packed)) auth_t;


/**
 * Video fragment
**/
typedef struct
{
	ivtp_hdr_t		hdr;			/**< packet header				*/
	unsigned short		frag_num;		/**< Fragment number, MSB set for the last fragment */

} __attribute__((packed)) video_frag_hdr_t;


/**
 * Sets the compression type
**/
typedef struct
{
	ivtp_hdr_t		hdr;
	unsigned char		compression_type;

} __attribute__((packed)) set_compression_t;

#ifdef CONFIG_SPX_FEATURE_GLOBAL_WEB_CONSOLE_PREVIEW
/**
* Webpreview capture status reply
**/
typedef struct
{
       ivtp_hdr_t              hdr;
       unsigned char           capStatus;
} __attribute__((packed)) web_cap_status_t;
#endif
/**
 * Set USB mouse mode
**/
typedef struct
{
	ivtp_hdr_t		hdr;
	unsigned char		mouse_mode;

} __attribute__((packed)) usb_mouse_mode_t;

/**
 * validate session token response
**/
typedef struct
{
	ivtp_hdr_t		hdr;
	unsigned char		sresp;

} __attribute__((packed)) stoken_resp_t;

typedef struct 
{
        ivtp_hdr_t hdr;
        unsigned char keybd_led;

} __attribute__((packed)) usb_keybd_led_t;

/////////////////////////////////////////added for sending user name/////////////////////

typedef struct
{
	char username_othersession[TOKENCFG_MAX_USERNAME_LEN];
	char  ip_add_othersession[TOKENCFG_MAX_CLIENTIP_LEN+1];
} __attribute__((packed)) ip_usr_datapkt;

typedef struct
{
	ivtp_hdr_t hdr;	
	char username_othersession[TOKENCFG_MAX_USERNAME_LEN];				
	char ip_add_othersession[TOKENCFG_MAX_CLIENTIP_LEN+1];

} __attribute__((packed)) kvm_priv_ctrl_pkt_t;		

/**
 * Set USB mouse mode
**/
typedef struct
{
	unsigned char		mouse_mode;
	VMediaCfg_T 		media_info;

} __attribute__((packed)) mouse_media_t;


#endif	/* __ADVISER_VIDEOPKT_H__ */

