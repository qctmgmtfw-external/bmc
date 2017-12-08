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
 * IPMI_DeviceConfig.h
 * Device Configuration Commands
 *
 *****************************************************************/
#ifndef IPMI_DEVICE_CONFIG_H
#define IPMI_DEVICE_CONFIG_H

/* LAN Configuration Commands */
#define CMD_SET_LAN_CONFIGURATION_PARAMETERS        0x01
#define CMD_GET_LAN_CONFIGURATION_PARAMETERS        0x02
#define CMD_SUSPEND_BMC_ARPS                        0x03
#define CMD_GET_IP_UDP_RMCP_STATISTICS              0x04

/* Serial Configuration Commands */
#define CMD_SERIAL_MODEM_CONNECTION_ACTIVITY        0x18
#define CMD_SET_SERIAL_MODEM_CONFIG                 0x10
#define CMD_GET_SERIAL_MODEM_CONFIG                 0x11
#define CMD_SET_SERIAL_MODEM_MUX                    0x12
#define CMD_GET_TAP_RES_CODES                       0x13
#define CMD_CALLBACK                                0x19
#define CMD_SET_USER_CALLBACK_OPTION                0x1a
#define CMD_GET_USER_CALLBACK_OPTION                0x1b

/* Serial Over Lan Configuration Commands */
#define CMD_GET_SOL_CONFIGURATION                   0x22
#define CMD_SET_SOL_CONFIGURATION                   0x21


#endif  /* IPMI_DEVICE_CONFIG_H */
