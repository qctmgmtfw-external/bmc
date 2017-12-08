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
 * PEFDevice.h
 * PEF Commands Handler
 *
 * Author: Govind Kothandapani <govindk@ami.com> 
 *       : Rama Bisa <ramab@ami.com>
 *       : Basavaraj Astekar <basavaraja@ami.com>
 *       : Bakka Ravinder Reddy <bakkar@ami.com>
 * 
 *****************************************************************/
#ifndef PEFDEVICE_H
#define PEFDEVICE_H

#include "Types.h"

/*** Macro Definitions ***/
#define PARAMETER_REVISION_FORMAT   0x11

/**
 * @defgroup pefc PEF and Alerting Commands
 * @ingroup senevt
 * IPMI PEF Device and Alerting Command Handlers. Invoked by the message handler
 * @{
 **/
extern int GetPEFCapabilities       (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int ArmPEFPostponeTimer      (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int SetPEFConfigParams       (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int GetPEFConfigParams       (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int SetLastProcessedEventId  (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int GetLastProcessedEventId  (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int AlertImmediate           (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int PETAcknowledge           (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
/** @} */

#define SEND_ALERT_IMM 0x0
#define GET_ALERT_IMM_STATUS  0x1
#define CLEAR_ALERT_IMM_STATUS 0x2


#endif  /* APPDEVICE_H */
