/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2005-2006, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        6145-F, Northbelt Parkway, Norcross,                **
 **                                                            **
 **        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 ****************************************************************
 ****************************************************************
 ******************************************************************
 *
 * ipmi_defs.h
 * IPMI Definitions
 *
 *  Author: Govind Kothandapani <govindk@ami.com>
 *
 ******************************************************************/
#ifndef  IPMI_DEFS_H
#define IPMI_DEFS_H
#include "Types.h"

/*------- Net Functions  -------------------*/
#define NETFN_CHASSIS					0x00
#define NETFN_BRIDGE					0x02
#define NETFN_SENSOR					0x04
#define NETFN_APP					0x06
#define NETFN_FIRMWARE					0x08
#define NETFN_STORAGE					0x0A
#define NETFN_TRANSPORT					0x0C
#define NETFN_PICMG					0x2C
#define NETFN_AMI					0x32
#define NETFN_OEM					0x2E
#define NETFN_OPMA1					0x30
#define NETFN_OPMA2					0x3E
#ifdef CFG_PROJ_SHMC_TEST_SUPPORT_YES
#define NETFN_TEST_OEM					0x34
#endif

#define NETFN_UNKNOWN               0xFF
#define IPMI_CMD_UNKNOWN            0xFF

#define NETFN_GROUP_EXTN                                0x2C
#define IPMI_GROUP_EXTN_CODE_DCMI	                0xDC

#define	MAX_USER_NAME_LEN           (16+1) //+1 for null terminator
#define IP6_ADDR_LEN		    16

/**** Command Completion Codes ****/
#define	CC_NORMAL										0x00
#define	CC_SUCCESS										0x00
#define CC_NODE_BUSY									0xC0
#define CC_INV_CMD										0xC1
#define CC_INV_CMD_FOR_LUN								0xC2
#define CC_TIMEOUT										0xC3
#define CC_OUT_OF_SPACE									0xC4
#define CC_INV_RESERVATION_ID							0xC5
#define CC_REQ_TRUNCATED								0xC6
#define CC_REQ_INV_LEN									0xC7
#define CC_REQ_FIELD_LEN_EXCEEDED						0xC8
#define CC_PARAM_OUT_OF_RANGE							0xC9
#define CC_CANNOT_RETURN_REQ_BYTES						0xCA
#define CC_SEL_REC_NOT_PRESENT							0xCB
#define CC_SDR_REC_NOT_PRESENT							0xCB
#define CC_FRU_REC_NOT_PRESENT							0xCB
#define CC_INV_DATA_FIELD								0xCC
#define CC_ILLEGAL_CMD_FOR_SENSOR_REC					0xCD
#define CC_COULD_NOT_PROVIDE_RESP						0xCE
#define CC_CANNOT_EXEC_DUPL_REQ							0xCF
#define CC_SDR_REP_IN_UPDATE_MODE						0xD0
#define CC_DEV_IN_FIRMWARE_UPDATE_MODE					0xD1
#define CC_INIT_AGENT_IN_PROGRESS						0xD2
#define CC_DEST_UNAVAILABLE								0xD3
#define CC_INSUFFIENT_PRIVILEGE							0xD4
#define CC_PARAM_NOT_SUP_IN_CUR_STATE					0xD5
#define CC_ERR_HANDLING_COMMAND_FAILURE					0xD6
#define CC_UNSPECIFIED_ERR								0xFF
#define CC_GET_MSG_QUEUE_EMPTY							0x80
#define	CC_EVT_MSG_QUEUE_EMPTY							0x80
#define CC_GET_SESSION_INVALID_USER						0x81
#define CC_GET_SESSION_NULL_USER_DISABLED				0x82
#define CC_ACTIVATE_SESS_NO_SESSION_SLOT_AVAILABLE		0x81
#define CC_ACTIVATE_SESS_NO_SLOT_AVAILABLE_USER			0x82
#define CC_ACTIVATE_SESS_REQ_LEVEL_EXCEEDS_LIMIT		0x83
#define CC_ACTIVATE_SESS_SEQ_OUT_OF_RANGE				0x84
#define CC_ACTIVATE_SESS_INVALID_SESSION_ID				0x85
#define CC_ACTIVATE_SESS_MAX_PRIVILEGE_EXCEEDS_LIMIT	0x86
#define	CC_PASSWORD_TEST_FAILED							0x80
#define CC_PASSWORD_TEST_FAILED_WRONG_SIZE				0x81
#define	CC_SETPASSWORD_INVALID_USERID					0x81
#define	CC_SETPASSWORD_CANNOT_DISABLE_USER				0x82
#define	CC_NO_ACK_FROM_SLAVE							0x83
#define	CC_GET_CH_COMMAND_NOT_SUPPORTED					0x82
#define	CC_SET_CH_COMMAND_NOT_SUPPORTED					0x82
#define	CC_SET_CH_ACCES_MODE_NOT_SUPPORTED				0x83
#define	CC_SET_SESS_PREV_REQ_LEVEL_NOT_AVAILABLE		0x80
#define	CC_SET_SESS_PREV_REQ_PRIVILEGE_EXCEEDS_LIMIT	0x81
#define	CC_SET_SESS_PREV_INVALID_SESSION_ID				0x82
#define CC_ACTIVATE_SESS_NO_SLOT_AVAILABLE_USER			0x82
#define	CC_CLOSE_INVALID_SESSION_ID						0x87
#define	CC_CLOSE_INVALID_SESSION_ID_HANDLE              0x88
#define	CC_PEF_PARAM_NOT_SUPPORTED						0x80
#define	CC_PEF_SET_IN_PROGRESS							0x81
#define CC_SESSION_IN_PROGRESS                          0x82
#define CC_PEF_ATTEMPT_TO_SET_READ_ONLY_PARAM			0x82
#define	CC_SET_IN_PROGRESS								0x81
#define CC_ATTEMPT_TO_SET_RO_PARAM						0x82
#define	CC_PARAM_NOT_SUPPORTED							0x80
#define CC_BIOS_NOT_READY								0x82
#define CC_IFC_NOT_SUPPORTED							0x81
#define CC_DISABLE_SM									0x81
#define CC_BIOS_IS_BUSY									0x81

#define CC_CALLBACK_REJ_SESSION_ACTIVE					0x82
#define CC_WRITE_ONLY_PARAM								0x83

#define CC_INST_ALREADY_ACTIVE							0x80
#define CC_CANNOT_ACTIVATE_WITH_ENCR      				0x83
#define CC_PAYLOAD_NOT_ENABLED							0x81
#define CC_INST_EXCEEDED								0x82
#define CC_INST_ALREADY_INACTIVE						0x80
#define CC_PAYLOAD_NOT_AVAILABLE						0x80
#define CC_ENCRYPT_NOT_AVAILABLE						0x82
#define CC_INST_NOT_ACTIVE								0x83
#define	CC_KEYS_LOCKED									0x80
#define	CC_INSUF_KEY_BYTES								0x81
#define	CC_TOO_MANY_KEY_BYTES							0x82
#define	CC_KEY_MISMATCH									0x83

#define CC_OP_NOT_SUPPORTED								0x80
#define CC_OP_NOT_ALLOWED								0x81
#define CC_ENC_NOT_AVAILABLE							0x82
#define CC_CANNOT_ACTIVATE_WITH_ENCR					0x83
#define CC_CANNOT_ACTIVATE_WITHOUT_ENCR					0x84

#define CC_ATTEMPT_TO_RESET_UNIN_WATCHDOG 				0x80

#define	CC_ILLEGAL_CONNECTOR_ID							0x81
#define CC_SEL_ERASE_IN_PROGRESS						0x81

#define CC_SAME_PAM_ORDER								0x90
#define CC_PAM_ORDER_SEC_DIFERS							0x91
#define CC_PAM_ORDER_FILE_DIFERS						0x92

/* Completion code for Set system info parameter command */
#define CC_SYS_INFO_PARAM_NOT_SUPPORTED					0x80
#define CC_SYS_INFO_SET_IN_PROGRESS 					0x81
#define CC_SYS_INFO_READ_ONLY_PARAM 					0x82
#define IPMI_EVM_REVISION								0x04
#define IPMI_SENSOR_TEMP_TYPE							0x01
#define IPMI_SENSOR_VOLT_TYPE							0x02
#define IPMI_SENSOR_FAN_TYPE							0x04
#define IPMI_SENSOR_PHYSICAL_SECURITY_TYPE				0x05
#define IPMI_SENSOR_POWER_SUPPLY_TYPE					0x08
#define IPMI_SENSOR_MEMORY_TYPE							0x0C
#define IPMI_SENSOR_DRIVE_TYPE							0x0D
#define IPMI_SENSOR_BUTTON_TYPE							0x14
#define IPMI_SENSOR_ENTITY_PRESENCE_TYPE				0x25
#define IPMI_SENSOR_OEM_TYPE							0xC0

/******************OEM Completion codes*********************/
#define OEMCC_INVALID_USERNAME							0x01
#define OEMCC_PASSWD_MISMATCH							0x02
#define OEMCC_INVALID_PASSWD							0x03
#define OEMCC_DUPLICATE_USERNAME						0x04
#define OEMCC_USER_EXISTS_AT_SLOT						0x05
#define OEMCC_NOMEM										0x06
#define OEMCC_FILE_ERR									0x07
#define OEMCC_SSHKEY_UPDATE_FAILURE						0x08
#define OEMCC_SENSOR_DISABLED							0x09
#define OEMCC_INVALID_SDR_ENTRY							0x0a
#define OEMCC_CORRUPT_FLASH_DATA						0x0b
#define OEMCC_CORRUPT_DATA_CHKSUM						0x0c
#define OEMCC_FLASH_UPGRADE_FAILURE						0x0d
#define OEMCC_VERSION_MISMATCH							0x0e
#define OEMCC_USER_NOT_EXISTS                           0x0f

#define OEMCC_EMAIL_NOT_CONFIGURED						0x10
#define OEMCC_SMTP_DISABLED								0x11
#define OEMCC_UNSUPPORTED_AUTH_TYPE						0x12
#define OEMCC_SEND_EMAIL_AUTH_FAILED					0x13
#define OEMCC_UNABLE_TO_CONNECT_SMTPSERVER				0x14
#define OEMCC_SEND_EMAIL_FAILED							0x15
#define OEMCC_USER_DISABLED 							0x16
#define OEMCC_RESERVED_USER_NAME						0x17
#define OEMCC_INSUFFIENT_LANIFC_COUNT                   0x80
#define OEMCC_UNSUPPORTED_BOND_MODE                     0x81
#define OEMCC_VLAN_ENABLED_ON_SLAVE                     0x82
#define OEMCC_INV_PARAM_ONLY_FOR_NON_BONDING   			0x81
#define OEMCC_INV_IP4_NOT_ENABLED  						0x82
#define OEMCC_INV_MIN_IFC_COUNT_DISABLED				0x83
#define OEMCC_BOND_NOT_ENABLED  						0x80
#define OEMCC_ACTIVE_SLAVE_LINK_DOWN                    0x81
#define OEMCC_SEL_EMPTY_REPOSITORY                         0x86
#define OEMCC_SEL_CLEARED                                           0x85
#define OEMCC_SENSOR_INFO_EMPTY                                 0x87

#define OEMCC_ATTEMPT_TO_GET_WO_PARAM              0x82

#define	CC_POWER_LIMIT_OUT_OF_RANGE						0x84
#define	CC_CORRECTION_TIME_OUT_OF_RANGE				    0x85
#define	CC_STAT_REPORTING_OUT_OF_RANGE					0x89

#define OEMCC_INV_DATE_TIME                             0x93
/*--------------------------------------------
 * Macro to extract the net function.
 *--------------------------------------------*/
#define NET_FN(NetFnLUN)	((INT8U)((NetFnLUN & 0xFC) >> 2))

#pragma pack( 1 )

/*-------------------------
 * Disable Message Filter Table
 *-------------------------*/
typedef struct
{
    INT8U                   NetFn;
    INT8U                   Command;
}  DisableMsgFilterTbl_T;

/* IPMI Message Header */
typedef struct
{
    INT8U	ResAddr;
    INT8U	NetFnLUN;
    INT8U	ChkSum;
    INT8U	ReqAddr;
    INT8U	RqSeqLUN;
    INT8U	Cmd;
} PACKED IPMIMsgHdr_T;

typedef struct
{
    uint32 SessionID;
    INT8U Privilege;
    INT16U IPMIMsgLen;
    INT8U NetFnLUN;
    INT8U Cmd;
    INT8U ChannelNum;
    INT8U AuthFlag;
    INT8U UserName[MAX_USER_NAME_LEN];
    INT8U IPAddr[IP6_ADDR_LEN];
}PACKED IPMIUDSMsg_T;
#pragma pack( )

#endif	/* IPMI_DEFS_H */
