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
 * FRUAPI.h
 * FRU access Api's.
 *
 *  Author: Govind Kothandapani <govindk@ami.com>
 *
 ******************************************************************/
#ifndef FRUAPI_H
#define FRUAPI_H
#include "Types.h"

/*------------------------------------------------------------------
 * @fn GetFRUDeviceSize
 * @brief Returns the size of the FRU Device.
 * @param DeviceID	- FRU Device ID to read from.
 * @returns The size of the FRU Device.
 *			-1 if error.
 *------------------------------------------------------------------*/
extern int API_GetFRUDeviceSize (INT8U DeviceID, int BMCInst);

/*------------------------------------------------------------------
 * @fn ReadFRUDevice
 * @brief Reads data from a FRU Device.
 * @param DeviceID	- FRU Device ID to read from.
 * @param Offset	- Offset in the FRU Device to read from.
 * @param Len		- Number of bytes to read.
 * @param pBuf		- Buffer to read the contents to.
 * @return 	The number of bytes actually read.
 *			-1 if error.
 *------------------------------------------------------------------*/
extern int API_ReadFRUDevice (INT8U DeviceID, INT16U Offset, INT16U Len, _FAR_ INT8U* pBuf, int BMCInst);

/*------------------------------------------------------------------
 * @fn WriteFRUDevice
 * @brief Write the data to FRU Device.
 * @param DeviceID	- FRU Device ID to write to.
 * @param Offset	- Offset in the FRU Device to write to.
 * @param Len		- Number of bytes to write.
 * @param pBuf		- Buffer to write the contents from.
 * @return 	The number of bytes actually written.
 *			-1 if error.
 *------------------------------------------------------------------*/
extern int API_WriteFRUDevice (INT8U DeviceID, INT16U Offset, INT16U Len, _FAR_ INT8U* pBuf, int BMCInst);

#endif	/* FRUAPI_H */

