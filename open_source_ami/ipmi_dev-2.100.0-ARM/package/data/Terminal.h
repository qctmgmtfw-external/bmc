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
 * terminal.h
 * Serial Terminal Mode Handler
 *
 *  Author: Vinoth Kumar  <vinothkumars@ami.com> 
 * 
 *****************************************************************/
#ifndef TERMINAL_H
#define TERMINAL_H

#include "Message.h"

/*** MACRO Definitions ***/
#define HEALTH_STATUS_OK        0
#define HEALTH_STATUS_NC        1
#define HEALTH_STATUS_CR        2
#define HEALTH_STATUS_NR        3
#define HEALTH_STATUS_UF        4
#define HEALTH_STATUS_UN        5

#define TERM_START_BYTE         '['
#define TERM_END_BYTE           ']'

#pragma pack( 1 )

/*** Type Definitions ***/
/**
 * @struct HealthState_T
 * @brief Terminal mode health status structure
 **/
typedef struct
 {

    INT8U OverallHealth;    /**< Overall Health    */
    INT8U TemperatureSys;   /**< Temperature Sys   */
    INT8U Volatge;          /**< Voltage Sys       */
    INT8U PowerSubSys;      /**< Power Sub System  */
    INT8U CoolingSys;       /**< Cooling System    */
    INT8U HardDrive;        /**< Hard Drive        */
    INT8U PhysicalSecurity; /**< Physical Security */
    INT8U PowerState;       /**< Power State       */

} PACKED  HealthState_T;

#pragma pack( )

/*** Extern Definitions ***/

/*** Functions Prototypes ***/
/**
 * @brief Processes the terminal requests
 * @param pReq Pointer to request message packet
 **/
extern void ProcessTerminalReq (_NEAR_ MsgPkt_T* pReq,int BMCInst);

/**
 * @brief Called by the interrupt after receiving a byte through serial port
 * @param byte a data byte received throught serial port
 **/
extern void OnTerminalByte (INT8U byte,int BMCInst);

/**
 * @brief Processes the ECHO request from serial interface
 * @param pReq Pointer to request message packet
 **/
extern void ProcessEchoReq (_NEAR_ MsgPkt_T* pReq,int BMCInst);

/**
 * @brief This function is called to process Terminal mode command.
 **/
extern int  PDK_TerminalCmd (INT8U* pReq, INT8U  ReqLen,
                             INT8U* pRes, INT8U* ResLen,
                             INT8U  SessionActivated);

#endif /* TERMINAL_H */
