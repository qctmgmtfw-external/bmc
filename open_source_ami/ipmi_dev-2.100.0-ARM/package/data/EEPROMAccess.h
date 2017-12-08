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
 * nvram.h
 * NVRAM Functions
 *
 *  Author: Govind Kothandapani <govindk@ami.com>
 *			Basavaraj Astekar	<basavaraja@ami.com>
 *			Ravinder Reddy		<bakkar@ami.com>
 ******************************************************************/
#ifndef EEPROM_ACCESS_H
#define EEPROM_ACCESS_H

#include "Types.h"
#include "IPMI_Main.h"
#include "NVRData.h"

typedef int (*fnEEPROMHndlr_T) (INT8U SlaveAddr, INT8U* pData, INT16U EEPROMAddr, INT16U Size, INT8U RWFlag);

#pragma pack (1)

typedef struct
{
    INT8U               DeviceType;
    fnEEPROMHndlr_T     fnEEPROMHndlr;

} PACKED EEPROMHndlr_T;

#pragma pack ()


 /**
 * @fn ReadWriteEEPROM
 * @brief Reads/Writes the Non volatile informations to/from file
 * @param SlaveAddr - SlaveAddress of device to Write or Read from.
 * @param pData    - Pointer to data.
 * @param Offset   - Offset in the file to Write of Read from.
 * @param Size     - Size of data to read/write.
 * @param Flag     - Flag to perform write or read operation.
**/
extern void ReadWriteEEPROM (INT8U DeviceType, INT8U SlaveAddr, INT8U* pData, INT16U Offset, INT16U Size, INT8U Falg);


#endif /* EEPROM_ACCESS_H */

