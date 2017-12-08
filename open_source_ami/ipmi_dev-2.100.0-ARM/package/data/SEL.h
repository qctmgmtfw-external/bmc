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
 * Sel.h
 * Sel command handler
 *
 * Author: Bakka Ravinder Reddy <bakkar@ami.com>
 *
 *****************************************************************/
#ifndef SEL_H
#define SEL_H

#include "Types.h"

/*--------------System Event Record-------------*/
#define SYSTEM_EVT_RECORD                    0x02
#define SYS_EVENT_LOGGING_MASK               0x08

#define ENABLE_SEL_PEF      0xFF
#define DISABLE_SEL_PEF     0x00

#ifdef CONFIG_SPX_FEATURE_CIRCULAR_SEL
#define LINEAR_SEL          0x00
#define CIRCULAR_SEL        0x01
#endif

/**
 * @defgroup sic SEL Command handlers
 * @ingroup storage
 * IPMI System Event Log interface commands.
 * This set of commands provides read/write access to BMC's SEL.
 * @{
**/
extern int GetSELInfo           (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int GetSELAllocationInfo (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int ReserveSEL           (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int ClearSEL             (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int GetSELEntry          (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int AddSELEntry          (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int PartialAddSELEntry   (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int DeleteSELEntry       (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int GetSELTime           (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int SetSELTime           (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int GetAuxiliaryLogStatus(_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int SetAuxiliaryLogStatus(_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int LockedAddSELEntry    (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,INT8U SysIfcFlag,_NEAR_ int BMCInst);
extern int GetSELTimeUTC_Offset  (_NEAR_ INT8U * pReq, INT8U ReqLen, _NEAR_ INT8U * pRes,_NEAR_ int BMCInst);
extern int SetSELTimeUTC_Offset   (_NEAR_ INT8U * pReq, INT8U ReqLen, _NEAR_ INT8U * pRes,_NEAR_ int BMCInst);

/** @} */

/**
 * @brief Get SEL timestamp.
 * @return The timestamp.
**/
extern INT32U GetSelTimeStamp (int BMCInst);

/**
 * @brief Initialize SEL Device.
 * @return 0 if success, -1 if error
**/
extern int InitSEL (int BMCInst);

#ifdef CONFIG_SPX_FEATURE_CIRCULAR_SEL
/*---------------------------------------------------------------------------
 * @fn SetSELPolicy
 *
 * @brief This function is invoked when switch SEL policy, make appropriate
 *        adjustment for environment variables that related to SEL buffer.
 *
 * @param   Policy  - Specify Linear SEL policy or Circular SEL policy.
 * @param   BMCInst - Index of BMC instance.
 *---------------------------------------------------------------------------*/
extern void SetSELPolicy(INT8U Policy, int BMCInst);
#endif

#endif  /* SEL_H */
