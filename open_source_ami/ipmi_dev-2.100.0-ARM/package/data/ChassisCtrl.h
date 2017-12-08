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
 * ChassisCtrl.h
 * Chassis control functions.
 *
 *  Author: AMI MegaRAC PM Team
 ******************************************************************/
#ifndef _CHASSIS_CTRL_H_
#define _CHASSIS_CTRL_H_
#include "Types.h"



/**
 * @def Parameters to controle the Chassis actions
 *
**/
#define CHASSIS_POWER_OFF               0x00
#define CHASSIS_POWER_ON                0x01
#define CHASSIS_POWER_CYCLE             0x02
#define CHASSIS_POWER_RESET             0x03
#define CHASSIS_DIAGNOSTIC_INTERRUPT    0x04
#define CHASSIS_SOFT_OFF                0x05
#define CHASSIS_SMI_INTERRUPT           0x06


#define CHASSIS_CTRL_ACTION             0x01
#define ON_SYSTEM_EVENT_DETECTED        0x02
#define ON_POWER_EVENT_DETECTED         0x03
#define ON_SET_RESTART_CAUSE            0x04


/**
 * @brief Initialize Chassis Control module.
 * @return 0 if success, -1 if error.
**/
extern int InitChassisCtrl( void );
extern void OnSetRestartCause (INT8U u8SysRestartCause, INT8U u8MadeChange,int BMCInst);

#endif	/*_CHASSIS_CTRL_H_*/
