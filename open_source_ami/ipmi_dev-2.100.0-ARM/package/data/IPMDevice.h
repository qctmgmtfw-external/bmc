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
 *****************************************************************
 *
 * IPMDevice.h
 * IPMDevice Commands Handler
 *
 * Author: Govind Kothandapani <govindk@ami.com> 
 *       : Rama Bisa <ramab@ami.com>
 *       : Basavaraj Astekar <basavaraja@ami.com>
 *       : Bakka Ravinder Reddy <bakkar@ami.com>
 * 
 *****************************************************************/
#ifndef IPMDEVICE_H
#define IPMDEVICE_H
#include "Types.h"

/*** Extern Declaration ***/




/**
 * @var _FAR_ INT8U g_ACPISysPwrState
 * @brief Contains System ACPI state.
 * @warning Should not be accessed from task other than Message Handler
 **/

 /*Commented it inorder to access the data across the processes and moved it to the
 shared memory structure BMCSharedMem_T in SharedMem.h file*/

//extern _FAR_ INT8U      g_ACPISysPwrState;

/**
 * @var _FAR_ INT8U g_ACPIDevPwrState
 * @brief Contains Device ACPI state.
 * @warning Should not be accessed from task other than Message Handler
 **/

 /*Commented it inorder to access the data across the processes and moved it to the
 shared memory structure BMCSharedMem_T in SharedMem.h file*/

//	extern _FAR_ INT8U      g_ACPIDevPwrState;

/*** Function Prototypes ***/
/**
 * @defgroup apcf1 IPM Device Commands
 * @ingroup apcf
 * IPMI IPM Device Command Handlers. Invoked by the message handler
 * @{
 **/
extern int GetDevID             (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int ColdReset            (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int WarmReset            (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int GetSelfTestResults   (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int MfgTestOn            (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int SetACPIPwrState      (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int GetACPIPwrState      (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int GetDevGUID           (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
/** @} */

#endif  /* IPMDEVICE_H */
