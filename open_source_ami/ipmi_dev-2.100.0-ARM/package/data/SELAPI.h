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
 * SELAPI.h
 * SEL Access API.
 *
 *  Author: Govind Kothandapani <govindk@ami.com>
 *
 ******************************************************************/
#ifndef SELAPI_H
#define SELAPI_H
#include "Types.h"
#include "IPMI_SEL.h"
#define ADD_SEL_FAILED   0xFFFF


/*-----------------------------------------------------------------
 * @fn API_GetSELRepositoryInfo
 * @brief Gets the information regarding the SEL.
 * @param pRepositoryInfo - Pointer to the repository information.
 * @return	0	if Success
 *			-1  if no repository, or repository bad.
 *-----------------------------------------------------------------*/
extern int API_GetSELRepositoryInfo (SELInfo_T* pRepositoryInfo, int BMCInst);

/*-----------------------------------------------------------------
 * @fn API_GetSELAllocInfo
 * @brief Gets the SEL Allocation information.
 * @param pAllocInfo - Pointer to the allocation information.
 * @return 	0	if Success
 *			-1	if no repository, or repository bad.
 *-----------------------------------------------------------------*/
extern int API_GetSELAllocInfo (SELAllocInfo_T* pAllocInfo, int BMCInst);

/*-----------------------------------------------------------------
 * @fn API_GetFirstSEL
 * @brief Gets the first SEL record.
 * @param pSELRecord - Pointer to the buffer to hold the first SEL Record.
 * @return 	The next SEL Record ID
 *			-1	if no SEL Record is found.
 *-----------------------------------------------------------------*/
extern int API_GetFirstSEL (_FAR_ INT8U* pSELRecord, int BMCInst);

/*-----------------------------------------------------------------
 * @fn API_GetSELRecord
 * @brief Gets the SEL record corresponding to the record ID.
 * @param  RecordID  - ID of the record to fetch.
 * @param pSELRecord - Pointer to the buffer to hold the first SEL Record.
 * @return 	The next SEL Record number
 *			-1	if no next SEL Record is found.
 *-----------------------------------------------------------------*/
extern int API_GetNextSEL (INT16U RecordID, _FAR_ INT8U* pSELRecord, int BMCInst);

/*-----------------------------------------------------------------
 * @fn API_AddSELRecord
 * @brief Adds a new SEL Record.
 * @param pSELRecord	- Pointer to the buffer holding the SEL Record.
 * @return 	Record ID of the record added.
 *			-1	if failed.
 *-----------------------------------------------------------------*/
extern int API_AddSELRecord (_FAR_ INT8U* pSELRecord, int BMCInst);


/*-----------------------------------------------------------------
 * @fn API_ClearSELRepository
 * @brief Clears the SEL repository.
 * @return 	0   if success
 *			-1	if failed.
 *-----------------------------------------------------------------*/
extern int API_ClearSELRepository (int BMCInst);

/*-----------------------------------------------------------------
 *  * @fn API_GetSELTime
 *   * @brief Gets the SEL Time.
 *    * @param pSELTime - Pointer to hold the time Value
 *     * @return	0	if Success
 *      *			-1  if no repository, or repository bad.
 *       *---------------------------------------------------------------- */
extern int API_GetSELTime  (_FAR_ INT32U *pSELTime, int BMCInst);



#endif	/* SELAPI_H */

