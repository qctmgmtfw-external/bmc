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
 * SDRfunc.h
 *
 *
 *  Author: Rama Bisa <ramab@ami.com>
 *
 ******************************************************************/
#ifndef SDRFUNC_H
#define SDRFUNC_H

#include "Types.h"
#include "SDRRecord.h"
#include "IPMI_SDR.h"


/**
 * @brief Get the first SDR record. 
 * @return First SDR record if success, 0 if error.
**/             
extern  _FAR_ SDRRecHdr_T* SDR_GetFirstSDRRec (int BMCInst);


/**
 * @brief Get the next SDR record. 
 * @param pSDRRec - Current SDR Record.
 * @return Next SDR record if success, 0 if error.
**/             
extern  _FAR_ SDRRecHdr_T* SDR_GetNextSDRRec (_FAR_ SDRRecHdr_T* pSDRRec,int BMCInst);

#endif /* SDRFUNC_H */
