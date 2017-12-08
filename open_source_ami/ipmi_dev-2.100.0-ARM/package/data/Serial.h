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
 ****************************************************************/
 /*****************************************************************
 *
 * Serial.h
 * Serial Packet Handler
 *
 * Author: Govind Kothandapani <govindk@ami.com> 
 *       : Bakka Ravinder Reddy <bakkar@ami.com>
 * 
 *****************************************************************/
#ifndef SERIAL_H
#define SERIAL_H

#include "Types.h"
#include "Message.h"

/*** Macro Definitions **/
#define SERIAL_MODE_IDLE        0
#define SERIAL_BASIC_MODE       1
#define SERIAL_PPP_MODE         2
#define SERIAL_TERMINAL_MODE    3
#define MODEM_CONNECT_MODE      4
#define MODEM_HANGUP_MODE       5
#define SERIAL_DIAL_PAGE_MODE   6
#define SERIAL_TAP_PAGE_MODE    7

/*** Extern Declarations ***/

/*** Global Variables ***/
/**
 * @var _FAR_ INT32U g_SerialSessionID
 * @brief Contains the Serial interface session ID
 * @warning Must be used by only serial interface task
 **/
//extern _FAR_ INT32U g_SerialSessionID;

/**
 * @var _FAR_ INT8U g_SerialSessionActive
 * @brief Contains the Serial interface session ID
 * @warning Must be used by only serial interface task
 **/
//extern _FAR_ INT8U g_SerialSessionActive;

/**
 * @brief Processes the IPMI requests received from Serial interface
 * @param pReq Pointer to request message packet
 * @param pRes Pointer to response message packet
 * @return Size of the response data
 **/
extern INT8U ProcessSerialMessage (_NEAR_ MsgPkt_T* pReq, _NEAR_ MsgPkt_T* pRes,int BMCInst);

#endif /* SERIAL_H */
