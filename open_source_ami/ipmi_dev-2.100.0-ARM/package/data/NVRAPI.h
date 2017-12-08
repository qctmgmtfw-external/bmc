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
 * apinvr.h
 * NVRAM API
 *
 * Author: Govind Kothandapani <govindk@ami.com> 
 * 
 *****************************************************************/
#ifndef APINVR_H
#define APINVR_H
#include "Types.h"

/*------------------------------------------------------------------*
 * @fn API_ReadNVR
 * @brief Reads the contents of the NVRAM Entry.
 * @param NVRName	NVR to read.
 * @param Offset	Offset to start reading from.
 * @param Size		Size to read. -1 if read till the end.
 * @param pBuf		Pointer to the buffer to store the contents.
 * @return			0 if success.
 *					-1 if the entry could not be read.
 *-------------------------------------------------------------------*/
extern int API_ReadNVR (char *NVRName, INT32U Offset, INT16U Size, _FAR_ INT8U* pBuf);


/*------------------------------------------------------------------*
 * @fn API_WriteNVR
 * @brief Writes the contents of the NVRAM Entry.
 * @param NVRName	NVR to write.
 * @param Offset	Offset to start writing to.
 * @param Size		Size to write. -1 if write till the end.
 * @param pBuf		Pointer to the buffer to write the contents from.
 * @return			0 if success.
 *					-1 if the entry could not be written to.
 *-------------------------------------------------------------------*/
extern int API_WriteNVR (char *NVRName, INT32U Offset, INT16U Size, INT8U* pBuf);

#endif	/* APINVR_H */
