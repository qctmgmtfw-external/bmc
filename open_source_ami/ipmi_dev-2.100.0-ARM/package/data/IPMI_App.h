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
 *****************************************************************
 *
 * IPMI_App.h
 * Application Command numbers
 *
 *****************************************************************/
#ifndef IPMI_APP_H
#define IPMI_APP_H

/*** Application Commands ***/
#define CMD_GET_DEV_ID              0x01
#define CMD_BROADCAST_GET_DEV_ID    0x01
#define CMD_COLD_RESET              0x02
#define CMD_WARM_RESET              0x03
#define CMD_GET_SELF_TEST_RESULTS   0x04
#define CMD_MFG_TEST_ON             0x05
#define CMD_SET_ACPI_PWR_STATE      0x06
#define CMD_GET_ACPI_PWR_STATE      0x07
#define CMD_GET_DEV_GUID            0x08
#define CMD_GET_NETFN_SUP           0x09
#define CMD_GET_CMD_SUP             0x0A
#define CMD_GET_SUBFN_SUP           0x0B
#define CMD_GET_CONFIG_CMDS         0x0C
#define CMD_GET_CONFIG_SUB_FNS      0x0D


#define CMD_SET_CMD_ENABLES         0x60
#define CMD_GET_CMD_ENABLES         0x61
#define CMD_SET_SUBFN_ENABLES       0x62
#define CMD_GET_SUBFN_ENABLES       0x63
#define CMD_GET_OEM_NETFN_IANA_SUPPORT   0x64


#define CMD_RESET_WDT               0x22
#define CMD_SET_WDT                 0x24
#define CMD_GET_WDT                 0x25

#define CMD_SET_BMC_GBL_ENABLES     0x2E
#define CMD_GET_BMC_GBL_ENABLES     0x2F
#define CMD_CLR_MSG_FLAGS           0x30
#define CMD_GET_MSG_FLAGS           0x31
#define CMD_ENBL_MSG_CH_RCV         0x32
#define CMD_GET_MSG                 0x33
#define CMD_SEND_MSG                0x34
#define CMD_READ_EVT_MSG_BUFFER     0x35
#define CMD_GET_BTIFC_CAP           0x36
#define CMD_GET_SYSTEM_GUID         0x37
#define CMD_GET_CH_AUTH_CAP         0x38
#define CMD_GET_SESSION_CHALLENGE   0x39
#define CMD_ACTIVATE_SESSION        0x3A
#define CMD_SET_SESSION_PRIV_LEVEL  0x3B
#define CMD_CLOSE_SESSION           0x3C
#define CMD_GET_SESSION_INFO        0x3D
#define CMD_GET_AUTH_CODE           0x3F
#define CMD_SET_CH_ACCESS           0x40
#define CMD_GET_CH_ACCESS           0x41
#define CMD_GET_CH_INFO             0x42
#define CMD_SET_USER_ACCESS         0x43
#define CMD_GET_USER_ACCESS         0x44
#define CMD_SET_USER_NAME           0x45
#define CMD_GET_USER_NAME           0x46
#define CMD_SET_USER_PASSWORD       0x47
#define CMD_MASTER_WRITE_READ       0x52

#define CMD_ACTIVATE_PAYLOAD        0x48
#define CMD_DEACTIVATE_PAYLOAD      0x49
#define CMD_GET_PAYLD_ACT_STATUS    0x4A
#define CMD_GET_PAYLD_INST_INFO     0x4B
#define CMD_SET_USR_PAYLOAD_ACCESS  0x4C
#define CMD_GET_USR_PAYLOAD_ACCESS  0x4D
#define CMD_GET_CH_PAYLOAD_SUPPORT  0x4E
#define CMD_GET_CH_PAYLOAD_VER      0x4F
#define CMD_GET_CH_OEM_PAYLOAD_INFO 0x50
#define CMD_GET_CH_CIPHER_SUITES    0x54
#define CMD_SUS_RES_PAYLOAD_ENCRYPT 0x55
#define CMD_SET_CH_SECURITY_KEYS    0x56
#define CMD_GET_SYS_IFC_CAPS        0x57

#define CMD_SET_SYSTEM_INFO_PARAM   0x58
#define CMD_GET_SYSTEM_INFO_PARAM   0x59
#endif  /* IPMI_APP_H */

