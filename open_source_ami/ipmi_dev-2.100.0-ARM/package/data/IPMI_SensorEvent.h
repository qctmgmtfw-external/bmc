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
 * Sensor.h
 * Sensor Command numbers
 *
 *****************************************************************/
#ifndef IPMI_SENSOR_EVENT_H
#define IPMI_SENSOR_EVENT_H

/*** Sensor Event Commands ***/
#define CMD_SET_EVENT_RECEIVER          0x00
#define CMD_GET_EVENT_RECEIVER          0x01
#define CMD_PLATFORM_EVENT              0x02

#define CMD_GET_PEF_CAPABILITIES        0x10
#define CMD_ARM_PEF_POSTPONE_TIMER      0x11
#define CMD_SET_PEF_CONFIG_PARAMS       0x12
#define CMD_GET_PEF_CONFIG_PARAMS       0x13
#define CMD_SET_LAST_PROCESSED_EVENT_ID 0x14
#define CMD_GET_LAST_PROCESSED_EVENT_ID 0x15
#define CMD_ALERT_IMMEDIATE             0x16
#define CMD_PET_ACKNOWLEDGE             0x17

#define CMD_GET_DEV_SDR_INFO            0x20
#define CMD_GET_DEV_SDR                 0x21
#define CMD_RESERVE_DEV_SDR_REPOSITORY  0x22
#define CMD_GET_SENSOR_READING_FACTORS  0x23
#define CMD_SET_SENSOR_HYSTERISIS       0x24
#define CMD_GET_SENSOR_HYSTERISIS       0x25
#define CMD_SET_SENSOR_THRESHOLDS       0x26
#define CMD_GET_SENSOR_THRESHOLDS       0x27
#define CMD_SET_SENSOR_EVENT_ENABLE     0x28
#define CMD_GET_SENSOR_EVENT_ENABLE     0x29
#define CMD_REARM_SENSOR_EVENTS         0x2A
#define CMD_GET_SENSOR_EVENT_STATUS     0x2B
#define CMD_GET_SENSOR_READING          0x2D
#define CMD_SET_SENSOR_TYPE             0x2E
#define CMD_GET_SENSOR_TYPE             0x2F
#define CMD_SET_SENSOR_READING          0x30


#endif  /* IPMI_SENSOREVENT_H */
