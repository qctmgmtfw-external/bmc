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
 * SDR.h
 * SDR functions.
 *
 *  Author: Rama Bisa <ramab@ami.com>
 *
 ******************************************************************/
#ifndef SDR_H
#define SDR_H

#include "Types.h"
#include "IPMI_SDR.h"
#include "SDRRecord.h"

#pragma pack( 1 )

/**
 * @struct E2ROMHdr_T
 * @brief EEROM Header.
**/
typedef struct
{
    INT8U Valid;
    INT8U Len;

} PACKED  E2ROMHdr_T;

#pragma pack( )

/**
 * @var g_SDRRAM
 * @brief SDR Repository.
**/
extern _FAR_ SDRRepository_T*   _FAR_   g_SDRRAM;


/**
 * @defgroup src SDR Repository Command handlers
 * @ingroup storage
 * IPMI Sensor Data Records Repository interface commands.
 * These commands provide read/write access to BMC's SDR repository.
 * @{
**/
extern int GetSDRRepositoryInfo      (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int GetSDRRepositoryAllocInfo (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int ReserveSDRRepository      (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int GetSDR                    (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int AddSDR                    (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int PartialAddSDR             (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int DeleteSDR                 (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int ClearSDRRepository        (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int GetSDRRepositoryTime      (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int SetSDRRepositoryTime      (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int EnterSDRUpdateMode        (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int ExitSDRUpdateMode         (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int RunInitializationAgent    (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
/** @} */

/**
 * @brief Get the next SDR record ID.
 * @param RecID - Current SDR record ID.
 * @return the next SDR record ID.
**/
extern INT16U   SDR_GetNextSDRId     (INT16U RecID,int BMCInst);

/**
 * @brief Reads SDR Repository contents.
 * @param pSDRRec - Current SDR Record header.
 * @return the next SDR Record header.
**/
extern _FAR_ SDRRecHdr_T*   ReadSDRRepository (_FAR_ SDRRecHdr_T* pSDRRec,int BMCInst);

/**
 * @brief Write into SDR Repository.
 * @param pSDRRec - Record to be written.
 * @param Offset  - Write offset.
 * @param Size    - Size of write.
 * @return the SDR Record header.
**/
extern void WriteSDRRepository (_FAR_ SDRRecHdr_T* pSDRRec, INT8U Offset, INT8U Size,INT8U SdrSize,int BMCInst);

/**
 * @brief Get the SDR Record.
 * @param RecID - SDR Record ID.
 * @return the SDR Record.
**/
extern _FAR_ SDRRecHdr_T* GetSDRRec  (INT16U RecID,int BMCInst);

/**
 * @brief Initialize SDR Repository.
 * @return 0 if success, -1 if error
**/
extern  int  InitSDR (int BMCInst);


#endif /* SDR_H */
