/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2010-2011, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy, Building 200, Norcross,         **
 **                                                            **
 **        Georgia 30093, USA. Phone-(770)-246-8600.           **
 **                                                            **
 ****************************************************************
 ****************************************************************/
/*****************************************************************
 *
 * snoopifc.h
 * 
 * A simple library which expose the function to access the snoop driver.
 *
 * Author: Gokulakannan. S <gokulakannans@amiindia.co.in>
 *****************************************************************/

#ifndef SNOOPIFC_H
#define SNOOPIFC_H


/*
 * @fn ReadCurrentBiosCode
 * @brief Function to read the last nCount bytes of current 
 * 			bios code from the snoop driver.
 * @param[out] pBiosCode - pointer to get the bios code.
 * @param[in] nCount 	 - Count for the number of bytes to Read.
 * @retval      number of bytes read, on success,
 *              -1, if failed.
 */
extern int ReadCurrentBiosCode (unsigned char *pBiosCode, int nCount);


/*
 * @fn ReadPreviousBiosCode
 * @brief Function to read the last nCount bytes of previous 
 * 			bios code from the snoop driver.
 * @param[out] pBiosCode - pointer to get the bios code.
 * @param[in] nCount 	 - Count for the number of bytes to Read.
 * @retval      number of bytes read, on success,
 *              -1, if failed.
 */
extern int ReadPreviousBiosCode (unsigned char *pBiosCode, int nCount);

#endif
