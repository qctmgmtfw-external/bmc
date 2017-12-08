/**
 * @file   libipmi_struct.h
 * @author Rajasekhar (rajasekharj@amiindia.co.in)
 * @date   02-Sep-2004
 *
 * @brief  Contains data structures	required for establishing a session
 *
 */

#ifndef __LIBIPMI_STRUCT_H__
#define __LIBIPMI_STRUCT_H__

#include "platform.h"
#include "coreTypes.h"

#if LIBIPMI_IS_OS_WINDOWS()
#include <winsock.h>
#endif

#if LIBIPMI_IS_OS_LINUX()
#define PACK __attribute__ ((packed))
#else
#define PACK
#pragma pack(1)
#endif/* LIBIPMI_IS_OS_LINUX() */


#define	MAX_REQUEST_SIZE		1024 * 60
#define	MAX_RESPONSE_SIZE		1024 * 60


#if LIBIPMI_IS_OS_LINUX()

#define HANDLE			int
#define SOCKET			int
#define SOCKET_ERROR	-1

#endif /* LIBIPMI_IS_OS_LINUX() */

#define IP4_VERSION     4
#define IP4_ADDR_LEN    4
#define IP6_ADDR_LEN   16

/**
 @def LAN_CHANNEL_T
 @brief holds data required for network medium
*/
typedef struct LAN_CHANNEL_T_tag {
	char			szIPAddress[46];
	uint16			wPort;
	SOCKET			hSocket;
	uint8			byIsConnected;
	uint8			bProtocol;
        uint8                   bFamily;
} PACK LAN_CHANNEL_T;

/**
 @def UDS_CHANNEL_T
 @brief holds data required for network medium
*/
typedef struct UDS_CHANNEL_T_tag {
	uint16			hSocketPath;
	SOCKET			hSocket;
	uint8			byIsConnected;
} PACK UDS_CHANNEL_T;
/**
 @def SERIAL_CHANNEL_T
 @brief holds data required for Serial medium
*/
typedef struct SERIAL_CHANNEL_T_tag {
	char			szdevice[32];
} PACK SERIAL_CHANNEL_T;

/* Authentication Types */
#define	AUTHTYPE_NONE									0x00
#define	AUTHTYPE_MD2									0x01
#define	AUTHTYPE_MD5									0x02
#define	AUTHTYPE_RESERVED								0x03
#define	AUTHTYPE_STRAIGHT_PASSWORD						0x04
#define	AUTHTYPE_OEM_PROPRIETARY						0x05
#define	AUTHTYPE_RMCP_PLUS_FORMAT						0x06

/* Privilege levels */
#define PRIV_LEVEL_NO_ACCESS                            0x0F
#define PRIV_LEVEL_PROPRIETARY							0x05
#define PRIV_LEVEL_ADMIN								0x04
#define PRIV_LEVEL_OPERATOR								0x03
#define PRIV_LEVEL_USER									0x02
#define PRIV_LEVEL_CALLBACK								0x01
#define PRIV_LEVEL_RESERVED								0x00


/* Authentication Algorithms */
#define AUTH_ALG_RAKP_NONE								0x00
#define AUTH_ALG_RAKP_HMAC_SHA1							0x01
#define AUTH_ALG_RAKP_HMAC_MD5							0x02

/* Integrity Algorithms */
#define INTEGRITY_ALG_NONE								0x00
#define INTEGRITY_ALG_HMAC_SHA1_96						0x01
#define INTEGRITY_ALG_HMAC_SHA1_128						0x02
#define INTEGRITY_ALG_MD5_128							0x03

/* Confidentiality Algorithms */
#define CONFIDENTIALITY_ALG_NONE						0x00
#define CONFIDENTIALITY_ALG_AES_CBC_128					0x01
#define CONFIDENTIALITY_ALG_XRC4_128					0x02
#define CONFIDENTIALITY_ALG_XRC4_40						0x03

/* Payload Types */
#define PAYLOAD_TYPE_IPMI								0
#define PAYLOAD_TYPE_SOL								1
#define PAYLOAD_TYPE_RSSP_OPEN_SES_REQ					0x10
#define PAYLOAD_TYPE_RSSP_OPEN_SES_RES					0x11
#define PAYLOAD_TYPE_RAKP_MSG_1							0x12
#define PAYLOAD_TYPE_RAKP_MSG_2							0x13
#define PAYLOAD_TYPE_RAKP_MSG_3							0x14
#define PAYLOAD_TYPE_RAKP_MSG_4							0x15

#define	MAX_KEY1_SIZE									20
#define	MAX_KEY2_SIZE									20
#define	MAX_GUID_SIZE									16
#define	MAX_USER_NAME_LEN								(16+1) //1 for stroing the null character
#define	MAX_USER_PWD_LEN								(20+1) //1 for storing the null character
#define MAX_RAND_NO_LEN									16


/* (0x6 << 2) == 0x18 */
#define DEFAULT_NET_FN_LUN					0x18
#define NETFNLUN_IPMI_APP					0x18
#define NETFNLUN_IPMI_SENSOR					0x10
#define NETFNLUN_IPMI_STORAGE					0x28
#define NETFNLUN_IPMI_CHASSIS                                   0x00

/**
 @def IPMI20_NETWORK_SESSION_T
 @brief holds data required for maintaining session with network medium
*/
typedef struct IPMI20_NETWORK_SESSION_tag
{
	LAN_CHANNEL_T	hLANInfo;
	char			szUserName [MAX_USER_NAME_LEN];
	char			szPwd [MAX_USER_PWD_LEN];
	uint32			dwSessionID;
	uint8			byIPMBSeqNum;
	uint32			dwSessionSeqNum;
	uint8			byAuthType;
	uint8			byChannelNum;
	uint8			byPerMsgAuthentication;
	uint8			byRole;
	uint8			byAuthAlgthm;
	uint8			byIntegrityAlgthm;
	uint8			byEncryptAlgthm;
	uint8			byPreSession;
	uint8			byResAddr;
	uint8			byReqAddr;
	uint8			byGUID [MAX_GUID_SIZE];
	uint8			byIsEncryptionReq;
	uint8			byKey1 [MAX_KEY1_SIZE];
	uint8			byKey2 [MAX_KEY2_SIZE];
	uint8			byMsgTag;
	uint32			dwRemConSessID;
	uint8			byRemConRandomNo [16];
	uint8			byMgdSysRandomNo [16];
	uint8			byDefTimeout;
} PACK IPMI20_NETWORK_SESSION_T;


/**
 @def IPMI20_SERIAL_SESSION_T
 @brief holds data required for maintaining session with serial medium
*/
typedef struct IPMI20_SERIAL_SESSION_T_tag {
	SERIAL_CHANNEL_T	hSerialInfo;
	char			szUserName [MAX_USER_NAME_LEN];
	char			szPwd [MAX_USER_PWD_LEN];
	HANDLE			hSerialPort;
	uint8			byMaxRetries;
	uint32			dwSessionID;
	uint8			byAuthType;
	uint32			dwInboundSeqNum;
	uint32			dwOutboundSeqNum;
	uint8			byIPMBSeqNum;
	uint8			byPrivLevel;
	uint8			byResAddr;
	uint8			byReqAddr;
	uint8			byDefTimeout;
} PACK IPMI20_SERIAL_SESSION_T;

/**
 @def IPMI20_UDS_SESSION_T
 @brief holds data required for maintaining session with unix domain socket medium
*/
typedef struct IPMI20_UDS_SESSION_T_tag {
  UDS_CHANNEL_T hUDSInfo;
  char szUserName[MAX_USER_NAME_LEN];
  char szPwd[MAX_USER_PWD_LEN];
  char szUName[MAX_USER_NAME_LEN];
  char abyIPAddr[IP6_ADDR_LEN];
  uint32 dwSessionID;
  uint8 byPreSession;
  uint8 byAuthType;
  uint8 byRole;
  uint8 byDefTimeout;
  uint8 byMaxRetries;
}PACK IPMI20_UDS_SESSION_T;

/**
 @def IPMI20_KCS_SESSION_T
 @brief holds data required for maintaining session with KCS medium
*/
typedef struct IPMI20_KCS_SESSION_T_tag {
	HANDLE			hKCSDevice;
	uint8			byResAddr;
} PACK IPMI20_KCS_SESSION_T;

/**
 @def IPMI20_IPMB_SESSION_T
 @brief holds data required for maintaining session with IPMB medium
*/
typedef struct IPMI20_IPMB_SESSION_T_tag {
	HANDLE			hIPMBDevice;
	uint8			bySlaveAddr;
} PACK IPMI20_IPMB_SESSION_T;

/**
 @def LIBIPMI_SERIAL_SETTINGS_T
 @brief holds settings for a session
*/
typedef struct {
/* settings state */
	uint8		bySettings;
#define	SETTINGS_NOTSET			0x00
#define	SETTINGS_DEFAULT		0x01
#define	SETTINGS_USER			0x02

/* BaudRate */
	uint32		dwBaudRate;

/* Parity */
	uint8		byParity;
#define			EVEN_PARITY					0x00
#define			ODD_PARITY					0x01
#define			NO_PARITY					0x02
#define			MARK_PARITY					0x03
#define			SPACE_PARITY				0x04

/* Stop Bits */
	uint8		byStopBit;
#define			STOPBIT_ONE					0x00
#define			STOPBIT_ONE5				0x01
#define			STOPBIT_TWO					0x02

	uint8		byByteSize;

/* Byte Size */
} PACK LIBIPMI_SERIAL_SETTINGS_T;

#define DEFAULT_BAUD_RATE					115200
#define DEFAULT_PARITY						NO_PARITY
#define	DEFAULT_STOPBITS					STOPBIT_ONE
#define DEFAULT_BYTESIZE					8

/**
 @def IPMI20_USB_SESSION_T
 @brief holds data required for maintaining session with USB medium
*/
typedef struct IPMI20_USB_SESSION_T_tag {
	union {
		HANDLE			hUSBDevice;
		int				hUSBDesc;
	};
	uint8			byResAddr;
} PACK IPMI20_USB_SESSION_T;


/**
 @def IPMI20_SESSION_T
 @brief holds info for maintaining a session
*/
typedef struct IPMI20_SESSION_T_tag {
/* Medium type (Network, Serial, KCS, IPMB, USB) */
	uint8	byMediumType;

#define NETWORK_MEDIUM_TCP      0x01
#define NETWORK_MEDIUM_UDP      0x04

#define	NETWORK_MEDIUM	        NETWORK_MEDIUM_TCP
#define	SERIAL_MEDIUM	        0x02
#define KCS_MEDIUM		        0x03
#define IPMB_MEDIUM		        0x05
#define USB_MEDIUM		        0x06
#define	UDS_MEDIUM	          0X07

/* tells whether session has started or not. */
	uint8	bySessionStarted;
#define SESSION_NOT_STARTED 0x00
#define SESSION_STARTED	0x01

/* if this value is > 0, session reestablishment will be tried for byMaxRetries times. */
	uint8	byMaxRetries;
/* LAN Eth Index for hold Eth number if multi NIc supported */
	uint8         EthIndex;

	IPMI20_NETWORK_SESSION_T	*hNetworkSession;
	IPMI20_SERIAL_SESSION_T		*hSerialSession;
	IPMI20_KCS_SESSION_T		*hKCSSession;
	IPMI20_IPMB_SESSION_T		*hIPMBSession;
	IPMI20_USB_SESSION_T		*hUSBSession;
	IPMI20_UDS_SESSION_T		*hUDSSession;

	LIBIPMI_SERIAL_SETTINGS_T	Settings;

} PACK IPMI20_SESSION_T;
typedef enum
{
    AUTH_FLAG = 1,
    AUTH_BYPASS_FLAG,
}USER_Auth;



/* Undefine PACK so that it can be redefined in other header files */
#if defined (PACK)
#undef PACK
#endif

#if LIBIPMI_IS_OS_LINUX()
#undef PACK
#else
#pragma pack()
#endif /* LIBIPMI_IS_OS_LINUX() */

#endif /* __LIBIPMI_STRUCT_H__ */
