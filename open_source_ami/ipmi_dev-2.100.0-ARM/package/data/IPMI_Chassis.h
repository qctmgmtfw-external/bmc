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
 * IPMI_Chassis.h
 * Chassis Command numbers
 *
 ******************************************************************/

#ifndef IPMI_CHASSIS_H
#define IPMI_CHASSIS_H

/*** Chassis Commands ***/
#define CMD_GET_CHASSIS_CAPABILITIES           0x00
#define CMD_GET_CHASSIS_STATUS                 0x01
#define CMD_CHASSIS_CONTROL                    0x02
#define CMD_CHASSIS_RESET                      0x03
#define CMD_CHASSIS_IDENTIFY                   0x04
#define CMD_SET_CHASSIS_CAPABILITIES           0x05
#define CMD_SET_POWER_RESTORE_POLICY           0x06
#define CMD_GET_SYSTEM_RESTART_CAUSE           0x07
#define CMD_SET_SYSTEM_BOOT_OPTIONS            0x08
#define CMD_GET_SYSTEM_BOOT_OPTIONS            0x09
#define CMD_SET_FP_BTN_ENABLES                 0x0A
#define CMD_GET_POH_COUNTER                    0x0F
#define CMD_SET_POWER_CYCLE_INTERVAL		   0x0B


#endif /* IPMI_CHASSIS_H	*/
