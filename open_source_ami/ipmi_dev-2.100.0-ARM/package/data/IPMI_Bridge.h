/*****************************************************************
 *****************************************************************
 ***                                                            **
 ***    (C)Copyright 2005-2006, American Megatrends Inc.        **
 ***                                                            **
 ***            All Rights Reserved.                            **
 ***                                                            **
 ***        6145-F, Northbelt Parkway, Norcross,                **
 ***                                                            **
 ***        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 ***                                                            **
 *****************************************************************
 *****************************************************************
 ******************************************************************
 *
 * IPMI_Bridge.h
 * Bridge Command numbers
 *
 *****************************************************************/
#ifndef IPMI_BRIDGE_H
#define IPMI_BRIDGE_H

/*** Bridge Management Commands ***/
#define CMD_GET_BRIDGE_STATE            0x00
#define CMD_SET_BRIDGE_STATE            0x01
#define CMD_GET_ICMB_ADDR               0x02
#define CMD_SET_ICMB_ADDR               0x03
#define CMD_SET_BRIDGE_PROXY_ADDR       0x04
#define CMD_GET_BRIDGE_STATISTICS       0x05
#define CMD_GET_ICMB_CAPABILITIES       0x06
#define CMD_CLEAR_BRIDGE_STATISTICS     0x08
#define CMD_GET_BRIDGE_PROXY_ADDR       0x09
#define CMD_GET_ICMB_CONNECTOR_INFO     0x0A
#define CMD_GET_ICMB_CONNECTION_ID      0x0B
#define CMD_SEND_ICMB_CONNECTION_ID     0x0C

/*** Bridge Discovery Commands ***/
#define CMD_PREPARE_FOR_DISCOVERY       0x10
#define CMD_GET_ADDRESSES               0x11
#define CMD_SET_DISCOVERED              0x12
#define CMD_GET_CHASSIS_DEVICE_ID       0x13
#define CMD_SET_CHASSIS_DEVICE_ID       0x14

/*** Bridging Commands ***/
#define CMD_BRIDGE_REQUEST              0x20
#define CMD_BRIDGE_MESSAGE              0x21

/*** Bridge Event Commands ***/
#define CMD_GET_EVENT_COUNT             0x30
#define CMD_SET_EVENT_DESTINATION       0x31
#define CMD_SET_EVENT_RECEPTION_STATE   0x32
#define CMD_SEND_ICMB_EVENT_MESSAGE     0x33
#define CMD_GET_EVENT_DESTINATION       0x34
#define CMD_GET_EVENT_RECEPTION_STATE   0x35


#endif  /* IPMI_BRIDGE_H*/
