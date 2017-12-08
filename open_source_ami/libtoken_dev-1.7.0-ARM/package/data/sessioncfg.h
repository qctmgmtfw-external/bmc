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

#ifndef SESSION_CFG_H
#define SESSION_CFG_H

/*------------------------ Session Configuration -------------------------*/
#define ADVISER_DEFAULT_PORT				(7578)
#define ADVISER_PORT             			ADVISER_DEFAULT_PORT
#define ADVISER_GET_DEFAULT_PORT(portnum)		( (portnum) + 999 )
#define CDSERVER_GET_DEFAULT_PORT(portnum)		( (portnum) + 999 )
#define FDSERVER_GET_DEFAULT_PORT(portnum)		( (portnum) + 999 )
#define HDSERVER_GET_DEFAULT_PORT(portnum)		( (portnum) + 999 )

#define HID_DEFAULT_INTERNAL_PORT			(8765)

#define	GET_CD_PORT(vmedia_portnum)			(vmedia_portnum)
#define	GET_HD_PORT(vmedia_portnum)			( (vmedia_portnum) + 2 )
#define	GET_FD_PORT(vmedia_portnum)			( (vmedia_portnum) + 3 )
#define	GET_VF_PORT(vmedia_portnum)			( (vmedia_portnum) + 8 )

#define	GET_SECURE_CD_PORT(vmedia_secure_portnum)	( vmedia_secure_portnum )
#define	GET_SECURE_HD_PORT(vmedia_secure_portnum)	( (vmedia_secure_portnum) + 2 )
#define	GET_SECURE_FD_PORT(vmedia_secure_portnum)	( (vmedia_secure_portnum) + 3 )
#define	GET_SECURE_VF_PORT(vmedia_secure_portnum)	( (vmedia_secure_portnum) + 8 )

#define MAX_POSSIBLE_KVM_SESSIONS			10
#define DEFAULT_SESSION_BANDWIDTH			(1000 * 1024 * 1024)
#define MAX_SCREEN_RESOLUTION 				((1600) * (1200))
#define MAX_BANDWIDTH					(100 * 1024 * 1024)
#define BANDWIDTH_1MBPS					(128 * 1024)

#ifdef CONFIG_SPX_FEATURE_GLOBAL_WEB_CONSOLE_PREVIEW  
#define WEB_PREVIEWER_HOST_POWER_OFF			-5
#define WEB_PREVIEWER_CAP_IN_PROGRESS			-2
#define WEB_PREVIEWER_CAP_FAILURE			-1
#define WEB_PREIVEW_CAPTURE_INTERVAL			10 	//seconds
#endif

#define MAX_TOKEN_LEN					16
#define MAX_USRNAME_LEN					32 	/* without the terminating NULL */
#define MAX_ADV_PWD_LEN					64
#define TOKENCFG_MAX_USERNAME_LEN			64
#define TOKENCFG_MAX_CLIENTIP_LEN			64

#endif	/* SESSION_CFG_H */
