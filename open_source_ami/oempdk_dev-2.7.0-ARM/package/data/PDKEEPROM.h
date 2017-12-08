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
 *****************************************************************
 * $Header:
 *
 * $Revision:
 *
 * $Date:
 *
 ******************************************************************
 ******************************************************************
 *
 * PDKEEPROM.h
 * EEPROM Access Functions
 *
 *  Author: Govind Kothandapani <govindk@ami.com>
 *			Basavaraj Astekar	<basavaraja@ami.com>
 *			Ravinder Reddy		<bakkar@ami.com>
 ******************************************************************/
#ifndef PDK_EEPROM_H
#define PDK_EEPROM_H

#include "Types.h"
#include "IPMI_Main.h"
#include "NVRData.h"

typedef int (*fnEEPROMHndlr_T) (INT8U BusNuber, INT8U SlaveAddr, INT8U* pData, INT16U EEPROMAddr, INT16U Size, INT8U RWFlag);

#pragma pack (1)

typedef struct
{
    INT8U               DeviceType;
    fnEEPROMHndlr_T     fnEEPROMHndlr;

} PACKED EEPROMHndlr_T;

#pragma pack ()

#define READ_EEPROM(DEVICE_TYPE, BUS_NUMBER, SLAVE_ADDR, PDATA, OFFSET, SIZE)   \
     ReadWriteEEPROM (DEVICE_TYPE, BUS_NUMBER, SLAVE_ADDR, PDATA, OFFSET, SIZE, READ_NVR)


#define WRITE_EEPROM(DEVICE_TYPE, BUS_NUMBER, SLAVE_ADDR, PDATA, OFFSET, SIZE)   \
     ReadWriteEEPROM (DEVICE_TYPE, BUS_NUMBER, SLAVE_ADDR, PDATA, OFFSET, SIZE, WRITE_NVR)


 /**
 * @fn ReadWriteEEPROM
 * @brief Reads/Writes the Non volatile informations to/from file
 * @param SlaveAddr - SlaveAddress of device to Write or Read from.
 * @param pData    - Pointer to data.
 * @param Offset   - Offset in the file to Write of Read from.
 * @param Size     - Size of data to read/write.
 * @param Flag     - Flag to perform write or read operation.
**/
extern int ReadWriteEEPROM (INT8U DeviceType, INT8U BusNumber, INT8U SlaveAddr, INT8U* pData, INT16U Offset, INT16U Size, INT8U Falg);


#endif /* PDK_EEPROM_H */

