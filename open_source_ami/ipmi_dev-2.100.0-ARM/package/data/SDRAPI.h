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
 * SDRAPI.h
 * SDR Access API.
 *
 *  Author: Govind Kothandapani <govindk@ami.com>
 *
 ******************************************************************/
#ifndef SDRAPI_H
#define SDRAPI_H
#include "Types.h"
#include "IPMI_SDR.h"
#include "IPMI_Sensor.h"
#include "IPMI_SDRRecord.h"

/*-----------------------------------------------------------------
 * @fn API_GetSDRRepositoryInfo
 * @brief Gets the information regarding the SDR.
 * @param pRepositoryInfo - Pointer to the repository information.
 * @return	0	if Success
 *			-1  if no repository, or repository bad.
 *-----------------------------------------------------------------*/
extern int API_GetSDRRepositoryInfo (SDRRepositoryInfo_T* pRepositoryInfo, int BMCInst);

/*-----------------------------------------------------------------
 * @fn API_GetSDRAllocInfo
 * @brief Gets the SDR Allocation information.
 * @param pAllocInfo - Pointer to the allocation information.
 * @return 	0	if Success
 *			-1	if no repository, or repository bad.
 *-----------------------------------------------------------------*/
extern int API_GetSDRAllocInfo (SDRRepositoryAllocInfo_T* pAllocInfo, int BMCInst);

/*-----------------------------------------------------------------
 * @fn API_GetFirstSDR
 * @brief Gets the first SDR record.
 * @param pSDRRecord - Pointer to the buffer to hold the first SDR Record.
 * @return 	The next SDR Record ID
 *			0	if no SDR Record is found.
 *-----------------------------------------------------------------*/
extern INT16U API_GetFirstSDR (_FAR_ INT8U* pSDRRecord, int BMCInst);

/*-----------------------------------------------------------------
 * @fn API_GetSDRRecord
 * @brief Gets the SDR record corresponding to the record ID.
 * @param pRecordID  - Pointer to ID of the record to fetch and set to Next ID.
 * @param pSDRRecord - Pointer to the buffer to hold the first SDR Record.
 * @return 	        1       the Next ID is stored in pNextID
 *			0	if no next SDR Record is found.
 *                     -1       if error
 *-----------------------------------------------------------------*/
extern int API_GetNextSDR (INT16U* pRecordID, _FAR_ INT8U* pSDRRecord, int BMCInst);

/*-----------------------------------------------------------------
 * @fn API_GetSensorSDR
 * @brief Finds SDR record corresponding to sensor number.
 * @param SensorNum - Sensor number to look for.
 * @return 	Pointer to sensor record
 *			NULL	if no SDR Record is found.
 *-----------------------------------------------------------------*/
extern SDRRecHdr_T* API_GetSensorSDR (INT8U SensorNum, int BMCInst);

/*-----------------------------------------------------------------
 * @fn API_AddSDRRecord
 * @brief Adds a new SDR Record.
 * @param pSDRRecord	- Pointer to the buffer holding the SDR Record.
 * @param RecordLen     - SDR Record length.
 * @return 	Record ID of the record added.
 *			0	if failed.
 *-----------------------------------------------------------------*/
extern INT16U API_AddSDRRecord (_FAR_ INT8U* pSDRRecord, INT8U RecordLen, int BMCInst);


/*-----------------------------------------------------------------
 * @fn API_ClearSDRRepository
 * @brief Clears the SDR repository.
 * @return 	0   if success
 *			-1	if failed.
 *-----------------------------------------------------------------*/
extern int API_ClearSDRRepository (int BMCInst);

/*-----------------------------------------------------------------
 * @fn API_GetSensorThresholds
 * @brief Retrieves the sensor thresholds for a particular sensor number
 * @param SensorNum - Sensor Number to look up
 * @param SensorThresh - Structure to store sensor thresholds in
 * @return 	0   if success
 *			-1	if failed.
 *-----------------------------------------------------------------*/
extern int API_GetSensorThresholds (INT8U SensorNum, GetSensorThresholdRes_T* SensorThresh, int BMCInst);

/*-----------------------------------------------------------------
 * @fn API_GetSensorReading
 * @brief Retrieves the sensor reading for a particular sensor number
 * @param SensorNum - Sensor Number to look up
 * @param SensorRead - Sensor Reading response
 * @return 	reading   if success
 *			 0	if no reading.
 *			-1	if failed.
 *-----------------------------------------------------------------*/
extern int API_GetSensorReading (INT8U SensorNum, GetSensorReadingRes_T* SensorRead, int BMCInst);

/*-----------------------------------------------------------------
 * @fn API_GetSensorNum
 * @brief Return sensor number based on sensor name
 * @return 	sensor number on success
 *			0xFF	if failed.
 *-----------------------------------------------------------------*/
extern INT8U API_GetSensorNum (const char * SensorName, int BMCInst);

/*-----------------------------------------------------------------
 * @fn API_GetSensorName
 * @brief Return sensor name based on sensor number
 * @return 	sensor name on success
 *			NULL	if failed.
 *-----------------------------------------------------------------*/
extern const char* API_GetSensorName (INT8U SensorNum, int BMCInst);

#endif	/* SDRAPI_H */
