/**
 * @file   libipmi_helper.h
 * @author Rajasekhar (rajasekharj@amiindia.co.in)
 * @date   02-Sep-2004
 *
 * @brief  Contains definition of helper routines 
 *			required by libipmi core
 *			
 */

#ifndef __LIBIPMI_HELPER_FNS_H__
#define __LIBIPMI_HELPER_FNS_H__

#include "coreTypes.h"

/**
\breif	Caluclates the checksum for the data between starting byte
		and ending byte. Adds the bytes from startbyte to endbyte and returns
		2's compliment of the result.
 @param	pbyStart	Starting byte
 @param pbyEnd		Ending byte

 @retval Returns the calculated checksum.
*/

uint8	LIBIPMI_CalculateCheckSum(uint8 *pbyStart, uint8 *pbyEnd);

/**
 \breif	Validates the checksum for an IPMI message.
	The IPMI Message header contains 2 checksum values.
	This routine calculates both the checksums and returns
	the result after comparing them with the received checksums.

 @param	pbyData		IPMI Message
 @param dwDataLen	IPMI Message length

 @retval returns 1 if calculated checksums matches with that of received checksum
		else returns 0.
*/
uint8	LIBIPMI_ValidateCheckSum(uint8	*pbyData, uint32 dwDataLen);

#endif



