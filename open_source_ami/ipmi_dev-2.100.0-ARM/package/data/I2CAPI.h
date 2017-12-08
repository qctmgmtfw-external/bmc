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
 * i2capi.h
 * I2C access Api's.
 *
 *  Author: Govind Kothandapani <govindk@ami.com>
 ******************************************************************/
#ifndef I2CAPI_H
#define I2CAPI_H
#include "Types.h"

/*----------------------------------------------------------------
 * @fn API_ReadWriteI2C
 * @brief Call this function to read & write to the I2C bus.
 * @param Bus 		- I2C bus to read/write to.
 * @param SlaveID	- Slave address of the device to read/write to.
 * @param NumWrite	- Number of bytes to write.
 * @param pWriteBuf	- Pointer to the buffer to write from.
 * @param NumRead	- Number of bytes to read.
 * @param pReadBuf	- Pointer to the buffer to read to.
 * @return			0  if success.
 *					-1 if failed.
 *----------------------------------------------------------------*/
extern int API_ReadWriteI2C (INT8U Bus,             INT8U SlaveID,
	   				       	 INT8U NumWrite, _NEAR_ INT8U* WriteBuf,
			    		   	 INT8U NumRead,  _NEAR_ INT8U* ReadBuf);

#endif	/* I2CAPI_H */
