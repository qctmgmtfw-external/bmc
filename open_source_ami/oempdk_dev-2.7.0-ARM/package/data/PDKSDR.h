/******************************************************************
 ******************************************************************
 ***                                                                                                           **
 ***    (C)Copyright 2005-2006, American Megatrends Inc.                            **
 ***                                                                                                           **
 ***    All Rights Reserved.                                                                          **
 ***                                                                                                           **
 ***    5555 , Oakbrook Pkwy, Norcross,                                                       **
 ***                                                                                                           **
 ***    Georgia - 30093, USA. Phone-(770)-246-8600.                                  **
 ***                                                                                                           **
 ******************************************************************
 ******************************************************************
 ******************************************************************
 *
 * PDKSDR.c
 * SDR related Functions.
 *
 *  Author: Winston <winstonv@amiindia.co.in>
 ******************************************************************/

 #ifndef _PDKSDR_H
 #define _PDKSDR_H

#include "Types.h"
#include "NVRAccess.h"
//#define SDR_FILE        "/conf/BMC1/SDR.dat"
#define SDR_FILE(Instance,BoardID,filename) \
 sprintf(filename,"%s%d/%s/%s",NV_DIR_PATH,Instance,BoardID,"SDR.dat")


 /**
 *@fn PDKInitNVRSDR     
 *@brief This function is invoked to load SDR records from NVRAM to RAM
 *@param pData - Pointer to buffer where SDR records have to be initialized
 *@param Size - Size of the SDR repository
  */
 extern int PDKInitNVRSDR(INT8U* pData,INT16U Size, int BMCInst);

/**
*@fn WriteSDR 
*@brief This function is invoked to wirte SDR records to NVRAM
*@param pData - Pointer to buffer from where SDR records have to be written in NVRAM
*@param Size - Size of the SDR records to be written
*/
extern int PDKWriteSDR(INT8U* pData,INT32U Offset,INT16U Size, int BMCInst);


 #endif //_PDKSDR_H

