/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2005-2006, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        Suite 200, 5555 oakbrook pkwy, Norcross            **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 *****************************************************************
 *
 * DCMDevice.h
 * DCMDevice Commands Handler
 *
 * Author: Rama Bisa <ramab@ami.com>
 * 
 *****************************************************************/
#ifndef DCMDEVICE_H
#define DCMDEVICE_H
#include "Types.h"

#define FRU_SUB_AREA_NO_DATA  0xC1

extern int GetDCMICapabilityInfo (INT8U* pReq, INT8U ReqLen, INT8U* pRes,_NEAR_ int BMCInst);
extern int GetPowerReading (INT8U* pReq, INT8U ReqLen, INT8U* pRes,_NEAR_ int BMCInst);
extern int GetPowerLimit (INT8U* pReq, INT8U ReqLen, INT8U* pRes,_NEAR_ int BMCInst);
extern int SetPowerLimit (INT8U* pReq, INT8U ReqLen, INT8U* pRes,_NEAR_ int BMCInst);
extern int ActivatePowerLimit (INT8U* pReq, INT8U ReqLen, INT8U* pRes,_NEAR_ int BMCInst);
extern int GetDCMISensorInfo (INT8U* pReq, INT8U ReqLen, INT8U* pRes,_NEAR_ int BMCInst);
extern int GetAssetTag (INT8U* pReq, INT8U ReqLen, INT8U* pRes,_NEAR_ int BMCInst);
extern void DCMIPowerSamplingTask (int BMCInst);
extern int SetAssetTag (INT8U* pReq, INT8U ReqLen, INT8U* pRes,_NEAR_ int BMCInst);
extern int GetManagementControllerIdString (INT8U* pReq, INT8U ReqLen, INT8U* pRes,_NEAR_ int BMCInst);
extern int SetManagementControllerIdString (INT8U* pReq, INT8U ReqLen, INT8U* pRes,_NEAR_ int BMCInst);

#endif  /* IPMDEVICE_H */
