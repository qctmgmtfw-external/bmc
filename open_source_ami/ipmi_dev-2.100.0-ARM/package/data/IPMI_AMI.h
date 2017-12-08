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
 *
 * IPMI_AMI.h
 * AMI specific IPMI Commands
 *
 * Author: Basavaraj Astekar <basavaraja@ami.com>
 *
 *****************************************************************/
#ifndef IPMI_AMI_H_
#define IPMI_AMI_H_

/**
*@struct AMIGetChNumReq_T
*@brief Structure to get respective channel num
*           for non BMC related Sensor Owner IDs
*/
typedef struct
{
    INT8U SensorOwnerID;

}PACKED AMIGetChNumReq_T;

/**
*@sturct AMIGetChNumRes_T
*@brief Structure to get respective channel num
*           for non BMC related Sensor Owner IDs
*/
typedef struct
{
    INT8U CompletionCode;
    INT8U ChannelNum;

}PACKED AMIGetChNumRes_T;

/**
*@struct AMIGetEthIndexReq_T
*@brief Structure to get respective Channel Num
*/
typedef struct
{

    INT8U ChannelNum;

}PACKED AMIGetEthIndexReq_T;

/**
*@struct AMIGetEthIndexRes_T
*@brief Structure to hold the respective EthIndex
*            for the requested Channel Num
*/

typedef struct
{

    INT8U CompletionCode;
    INT8U EthIndex;

}PACKED AMIGetEthIndexRes_T;


/* AMI Net function group command numbers */

/* define your command numbers here */

#define CMD_AMI_YAFU_COMMON_NAK (0X00FF)
#define CMD_AMI_YAFU_GET_FLASH_INFO  (0x0001)
#define CMD_AMI_YAFU_GET_FIRMWARE_INFO (0x0002)
#define CMD_AMI_YAFU_GET_FMH_INFO (0x0003)
#define CMD_AMI_YAFU_GET_STATUS (0x0004)
#define CMD_AMI_YAFU_ACTIVATE_FLASH (0x0010)
#define CMD_AMI_YAFU_ALLOCATE_MEMORY (0x0020)
#define CMD_AMI_YAFU_FREE_MEMORY (0x0021)
#define CMD_AMI_YAFU_READ_FLASH (0x0022)
#define CMD_AMI_YAFU_WRITE_FLASH (0x0023)
#define CMD_AMI_YAFU_ERASE_FLASH (0x0024)
#define CMD_AMI_YAFU_PROTECT_FLASH (0x0025)
#define CMD_AMI_YAFU_ERASE_COPY_FLASH (0x0026)
#define CMD_AMI_YAFU_VERIFY_FLASH (0x0027)
#define CMD_AMI_YAFU_GET_ECF_STATUS (0x0028)
#define CMD_AMI_YAFU_GET_VERIFY_STATUS (0x0029)
#define CMD_AMI_YAFU_READ_MEMORY (0x0030)
#define CMD_AMI_YAFU_WRITE_MEMORY (0x0031)
#define CMD_AMI_YAFU_COPY_MEMORY (0x0032)
#define CMD_AMI_YAFU_COMPARE_MEMORY (0x0033)
#define CMD_AMI_YAFU_CLEAR_MEMORY (0x0034)
#define CMD_AMI_YAFU_GET_BOOT_CONFIG (0x0040)
#define CMD_AMI_YAFU_SET_BOOT_CONFIG (0x0041)
#define CMD_AMI_YAFU_GET_BOOT_VARS (0x0042)
#define CMD_AMI_YAFU_DEACTIVATE_FLASH_MODE (0x0050)
#define CMD_AMI_YAFU_RESET_DEVICE (0x0051)


#define CMD_AMI_GET_CHANNEL_NUM (0x60)
#define CMD_AMI_GET_ETH_INDEX (0x62)

#define CMD_AMI_GET_EMAIL_USER	(0x63)
#define CMD_AMI_SET_EMAIL_USER	(0x64)
#define CMD_AMI_RESET_PASS	(0x65)
#define CMD_AMI_RESTORE_DEF	(0x66)
#define CMD_AMI_GET_LOG_CONF	(0x67)
#define CMD_AMI_SET_LOG_CONF	(0x68)

/* AMI Specific Extend Commands */
#define CMD_AMI_GET_SERVICE_CONF    (0x69)
#define CMD_AMI_SET_SERVICE_CONF    (0x6a)
#define CMD_AMI_GET_DNS_CONF        (0x6b)
#define CMD_AMI_SET_DNS_CONF        (0x6c)

//Fiwmware update through TFTP, HTTP and FTP
#define CMD_AMI_FW_INET_UPDATE  (0x6d)
#define CMD_AMI_FW_INET_STATUS  (0x6e)
#define CMD_AMI_FW_INET_PROGRAM (0x6f)
#define CMD_AMI_LINK_DOWN_RESILENT (0x70)

#define CMD_AMI_SET_IFACE_STATE (0x71)
#define CMD_AMI_GET_IFACE_STATE (0x72)

#define CMD_AMI_GET_BIOS_CODE       (0x73)

// DNS v6 commands */
#define CMD_AMI_GET_V6DNS_CONF        (0x74)
#define CMD_AMI_SET_V6DNS_CONF        (0x75)

#define CMD_AMI_SET_FIREWALL		(0x76)
#define CMD_AMI_GET_FIREWALL		(0x77)

#define CMD_SET_SMTP_CONFIG_PARAMS              ( 0x78 )
#define CMD_GET_SMTP_CONFIG_PARAMS              ( 0x79 )

//FRU details
#define CMD_AMI_GET_FRU_DETAILS		(0x80)
#define CMD_AMI_GET_EMAILFORMAT_USER (0x81)
#define CMD_AMI_SET_EMAILFORMAT_USER	(0x82)

//Linux Root User Access Commands
#define CMD_AMI_GET_ROOT_USER_ACCESS	(0x90)
#define CMD_AMI_SET_ROOT_PASSWORD	    (0x91)

//Set User Shelltype
#define CMD_AMI_GET_USER_SHELLTYPE	(0x92)
#define CMD_AMI_SET_USER_SHELLTYPE	(0x93)

/* Trigger Event Configuration Command Numbers */
#define CMD_AMI_SET_TRIGGER_EVT    (0x94)
#define CMD_AMI_GET_TRIGGER_EVT    (0x95)

/* SOL Configuration Command Numbers */
#define CMD_AMI_GET_SOL_CONFIG_PARAMS    (0x96)

/* Login Audit Config Command Numbers */
#define CMD_AMI_SET_LOGIN_AUDIT_CFG    (0x97)
#define CMD_AMI_GET_LOGIN_AUDIT_CFG    (0x98)

/* IPMI PAM Reordering Command */
#define CMD_AMI_SET_PAM_ORDER        0x7a
#define CMD_AMI_GET_PAM_ORDER        0x7b

/* AMI-SNMP related Commands*/
#define CMD_AMI_GET_SNMP_CONF 0x7c
#define CMD_AMI_SET_SNMP_CONF 0x7d

/* AMI SEL Commands */
#define CMD_AMI_GET_SEL_POLICY  0x7e
#define CMD_AMI_SET_SEL_POLICY  0x7f

/* AMI-Preserve Conf related Commands*/
#define CMD_AMI_SET_PRESERVE_CONF 0x83
#define CMD_AMI_GET_PRESERVE_CONF 0x84

/* Retrive SEL Entries Command */
#define CMD_AMI_GET_SEL_ENTIRES 0x85

/* Retrive Sensor Info Command*/
#define CMD_AMI_GET_SENSOR_INFO 0x86

#endif /* IPMI_AMI_H */

