/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2008, American Megatrends Inc.             **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555, Oakbrook Parkway, Suite 200,  Norcross,       **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 *****************************************************************
 *****************************************************************
 *
 * IPMI_PICMG.h
 * PICMG Command numbers
 *
 *****************************************************************/
#ifndef IPMI_PICMG_H
#define IPMI_PICMG_H

#define     CMD_GET_PICMG_PROPERTIES           0x00
#define     CMD_GET_ADDR_INFO                  0x01
#define     CMD_GET_SHELF_ADDR_INFO            0x02
#define     CMD_SET_SHELF_ADDR_INFO            0x03
#define     CMD_FRU_CONTROL                    0x04
#define     CMD_GET_FRU_LED_PROPERTIES         0x05
#define     CMD_GET_LED_COLOR_CAPS             0x06
#define     CMD_SET_FRU_LED_STATE              0x07
#define     CMD_GET_FRU_LED_STATE              0x08
#define     CMD_SET_IPMB_STATE                 0x09
#define     CMD_SET_FRU_ACTVN_POLICY           0x0A
#define     CMD_GET_FRU_ACTVN_POLICY           0x0B
#define     CMD_SET_FRU_ACTIVATION             0x0C
#define     CMD_GET_DEV_LOCATOR_REC_ID         0x0D
#define     CMD_SET_PORT_STATE                 0x0E
#define     CMD_GET_PORT_STATE                 0x0F
#define     CMD_COMPUTE_POWER_PROPERTIES       0x10
#define     CMD_SET_POWER_LEVEL                0x11
#define     CMD_GET_POWER_LEVEL                0x12
#define     CMD_RENEGOTIATE_PWR                0x13
#define     CMD_GET_FAN_SPEED_PROPERTIES       0x14
#define     CMD_SET_FAN_LEVEL                  0x15
#define     CMD_GET_FAN_LEVEL                  0x16
#define     CMD_BUSED_RESOURCE                 0x17
#define     CMD_GET_IPMB_LINK_INFO             0x18
#define     CMD_GET_SHELF_IPMB_ADDR            0x1B
#define     CMD_SET_AMC_PORT_STATE             0x19
#define     CMD_GET_AMC_PORT_STATE             0x1A
#define     CMD_SET_FAN_POLICY                 0x1C
#define     CMD_GET_FAN_POLICY                 0x1D
#define     CMD_FRU_CONTROL_CAPS               0x1E
#define     CMD_FRU_INVY_DEVICE_LOCK_CTRL      0x1F
#define     CMD_FRU_INVY_DEVICE_WRITE          0x20
#define     CMD_GET_SHELF_MANAGER_IPADDR       0x21
#define     CMD_GET_SHELF_POWER_ALLOC          0x22
#define     CMD_GET_TELCO_ALARM_CAPS           0x29
#define     CMD_GET_TELCO_ALARM_STATE          0x2A
#define     CMD_SET_TELCO_ALARM_STATE          0x2B
#define     CMD_GET_TELCO_ALARM_LOCATION       0x39
#define     CMD_SET_FRU_EXTRACTED              0x3A

#endif //IPMI_PICMG_H

