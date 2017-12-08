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
 * PDKSEL.c
 * SEL related Functions.
 *
 *  Author: Winston <winstonv@amiindia.co.in>
 ******************************************************************/
#ifndef _PDKSEL_H
#define _PDKSEL_H

#include "Types.h"
#include "NVRAPI.h"
#include "NVRAccess.h"

//#define SEL_FILE        "/conf/BMC1/SEL.dat"
#define SEL_FILE(Instance,filename) \
sprintf(filename,"%s%d/%s",NV_DIR_PATH,Instance,"SEL.dat")

/**
*@fn PDKInitNVRSEL 
*@brief This function is invoked to load SEL entries from NVRAM to RAM
*@param pData - Pointer to buffer where SEL records have to be initialized
*@param Size - Size of the SEL repository
*/
int PDKInitNVRSEL(INT8U* pData,INT16U Size, int BMCInst);

/**
*@fn WriteSEL 
*@brief This function is invoked to Write SEL entries to NVRAM
*@param pData - Pointer to buffer from where SEL records have to be written in NVRAM 
*@param Size - Size of the SEL entries to be written
*/
extern int PDKWriteSEL(INT8U* pData,INT32U Offset,INT16U Size, int BMCInst);

#endif //_PDKSEL_H
 

