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
 *
 * SerialModem.c
 * Serial/Modem configuration , Callback &  MUX
 *
 *  Author: Govindarajan <govindarajann@amiindia.co.in>
 *          Vinoth Kumar <vinothkumars@amiindia.co.in>
 ****************************************************************/
#ifndef SERIAL_MODEM_H
#define SERIAL_MODEM_H

#include "Types.h"
#include "IPMI_SerialModem.h"
#include "IPMI_LANConfig.h"

/*** External Definitions ***/
#define CHANNEL_ALWAYS_AVAILABLE                2
#define MAX_MODEM_INIT_STR_BLOCKS               4
#define MAX_MODEM_INIT_STR_BLOCK_SIZE           16
#define MAX_MODEM_ESC_SEQ_SIZE                  5
#define MAX_MODEM_HANG_UP_SEQ_SIZE              8
#define MAX_MODEM_DIAL_CMD_SIZE                 8
#define MAX_SERIAL_ALERT_DESTINATIONS           5
#define MAX_MODEM_DIAL_STRS                     5
#define MAX_MODEM_DIAL_STR_BLOCKS               3
#define MAX_MODEM_DIAL_STR_BLOCK_SIZE           16
#define MAX_MODEM_ALERT_DEST_IP_ADDRS           5
#define MAX_MODEM_TAP_ACCOUNTS                  3
#define TAP_SERVICE_TYPE_FIELD_SIZE             3
#define TAP_PASSWORD_SIZE                       7
#define TAP_PAGER_ID_STRING_SIZE                13
#define MAX_MODEM_CHAP_NAME_SIZE                16
#define MAX_MODEM_PPP_ACCOUNTS                  3
#define PPP_ACC_USER_NAME_DOMAIN_PASSWD_SIZE    16
#define MAX_COMM_STRING_SIZE                    18
#define MODEM_CFG_DEST_INFO_DEST_TYPE_MASK      0x0f

/* Bit field constants for configuration Data1 & Data2*/
#define TC_SET_DATA_MASK                    0xC0
#define TC_LINE_EDIT_ENABLE_MASK            0x20
#define TC_CHECKSUM_REQ_MASK                0x10
#define TC_DELETE_CTRL_MASK                 0x0C
#define TC_ECHO_MASK                        0x02
#define TC_HAND_SHAKE_MASK                  0x1
#define TC_OUT_TERMINATION_SEQ_MASK         0xF0
#define TC_IN_TERMINATION_SEQ_MASK          0x0F

/**
 * @defgroup smc Serial/Modem Command handlers
 * @ingroup devcfg
 * IPMI Serial/Modem configuration interface command handlers.
 * Set/Get configuration commands allow retrieval and updation of various
 * Serial/Modem, Terminal and PPP parameters.
 * @{
**/
extern int SetSerialModemConfig   (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int GetSerialModemConfig   (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int CallBack               (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int SetUserCallBackOptions (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int GetUserCallBackOptions (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int SetSerialModemMUX      (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int SerialModemConnectActive (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int GetTAPResponseCodes    (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
/** @} */

/**
 * @brief Task function to generate Serial/Modem Ping message.
**/
extern void SerialModemPingTask ( int BMCInst );

#endif  /* SERIAL_MODEM_H */
