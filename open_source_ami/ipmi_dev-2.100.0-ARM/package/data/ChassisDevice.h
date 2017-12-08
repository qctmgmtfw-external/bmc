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
 ******************************************************************
 * 
 * ChassisDevice.h
 * ChassisDevice.c extern declarations
 *
 *  Author: Rama Bisa <ramab@ami.com>
 *          
 ******************************************************************/

#ifndef CHASSIS_DEVICE_H
#define CHASSIS_DEVICE_H

#include "Types.h"

/*** Definitions and Macros ***/
#define  CHASSIS_POWER_DOWN                        0x00
#define  CHASSIS_POWER_UP                          0x01
#define  CHASSIS_POWER_CYCLE                       0x02
#define  CHASSIS_HARD_RESET                        0x03
#define  CHASSIS_PULSE_DIAGNOSTIC_INTERRUPT        0x04
#define  CHASSIS_SOFT_SHUTDOWN                     0x05

#define  CHASSIS_PARAMETER_VERSION                 0x01

#define  POH_MINS_PER_COUNT                        60

/******************************************************************************
*
*  Chassis Command Bit Definitions
*
******************************************************************************/

// The chassis status 1st byte definitions:
//    [4] - power control fault
//    [3] - power fault in main power subsystem (NOT SUPPORT)
//    [2] - Interlock (NOT SUPPORT)
//    [1] - power overload (NOT SUPPORT)
//    [0] - system power is on
#define CHASSIS_STATUS_POWER_CTRL_FAULT        (1 << 4)
#define CHASSIS_STATUS_POWER_FAULT             (1 << 3)
#define CHASSIS_STATUS_POWER_INTERLOCK         (1 << 2)
#define CHASSIS_STATUS_POWER_OVERLOAD          (1 << 1)
#define CHASSIS_STATUS_POWER_ON                (1 << 0)

// The chassis status 2nd byte definitions:
//    [4] - last "power is on" state was entered via IPMI command
//    [3] - last power down caused by power fault (NOT SUPPORT)
//    [2] - last power down caused by a power interlock being activated (NOT SUPPORT)
//    [1] - last power down caused by a power overload (NOT SUPPORT)
//    [0] - AC lost
#define CHASSIS_STATUS_EVENT_POWER_IPMI        (1 << 4)
#define CHASSIS_STATUS_EVENT_POWER_FAULT       (1 << 3)
#define CHASSIS_STATUS_EVENT_POWER_INTERLOCK   (1 << 2)
#define CHASSIS_STATUS_EVENT_POWER_OVERLOAD    (1 << 1)
#define CHASSIS_STATUS_EVENT_AC_LOST           (1 << 0)

// The chassis status 3rd byte definitions:
//    [3] - Cooling / fan fault detected
//    [2] - Drive fault (NOT SUPPORT)
//    [1] - Front panel lockout active
//    [0] - Chassis intrusion active
#define CHASSIS_STATUS_MISC_FAN_FAULT          (1 << 3)
#define CHASSIS_STATUS_MISC_DRIVE_FAULT        (1 << 2)
#define CHASSIS_STATUS_MISC_FP_LOCKOUT         (1 << 1)
#define CHASSIS_STATUS_MISC_INTRUSION          (1 << 0)

// front panel enables
#define FP_DISENABLES_STANDBY_BUTTON           (1 << 3)
#define FP_DISENABLES_NMI_BUTTON         	   (1 << 2)
#define FP_DISENABLES_RESET_BUTTON         	   (1 << 1)
#define FP_DISENABLES_POWER_BUTTON         	   (1 << 0)
// system restart cause code defined by IPMI spec
#define RESTART_CAUSE_UNKNOWN             	   0x0
#define RESTART_CAUSE_CHASSIS_CTRL             0x1
#define RESTART_CAUSE_RESET_BUTTON             0x2
#define RESTART_CAUSE_POWER_BUTTON             0x3
#define RESTART_CAUSE_WDT_EXPIRATION		   0x4
#define RESTART_CAUSE_OEM                      0x5
#define RESTART_CAUSE_AUTO_ALWAYS_ON           0x6
#define RESTART_CAUSE_AUTO_PREV_STATE          0x7
#define RESTART_CAUSE_PEF_RESET                0x8
#define RESTART_CAUSE_PEF_POWER_CYCLE          0x9
#define RESTART_CAUSE_SOFT_RESET               0xA
#define RESTART_CAUSE_POWER_DOWN               0xFF         // 0x0C - 0xFF reserved, 0xFF used internally only
// BMC Boot flag valid bit clearing
#define BMC_BOOT_FLAG_POWER_BUTTON             (1 << 0)
#define BMC_BOOT_FLAG_RESET_BUTTON             (1 << 1)
#define BMC_BOOT_FLAG_WDT_EXPIRATION           (1 << 2)
#define BMC_BOOT_FLAG_CHASSIS_CTRL             (1 << 3)
#define BMC_BOOT_FLAG_PEF                      (1 << 4)

// BMC Boot Option "Set In Progress" parameter
#define BMC_BOOT_OPTION_SET_COMPLETE           0x00
#define BMC_BOOT_OPTION_SET_IN_PROGRESS        0x01
#define BMC_BOOT_OPTION_COMMIT_WRITE           0x02

/*** Function Prototypes ***/

/**
 * @defgroup ccf1 Chassis Device Commands
 * @ingroup ccf
 * IPMI Chassis Device Command Handlers. Invoked by the message handler
 * @{
 **/
extern int GetChassisCaps       (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int GetChassisStatus     (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int ChassisControl       (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int SetChassisCaps       (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int SetPowerRestorePolicy(_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int GetChassisIdentify   (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int GetSysRestartCause   (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int GetPOHCounter        (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int SetSysBOOTOptions    (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int GetSysBOOTOptions    (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int SetFPButtonEnables (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int SetPowerCycleInterval (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern void SetACPIState(INT8U State, int BMCInst);


/** @} */

/**
 * @brief   Power On Hours timer tick, invoked every POH_MINS_PER_COUNT minutes by timer task
 **/

#endif /* CHASSIS_DEVICE_H  */
