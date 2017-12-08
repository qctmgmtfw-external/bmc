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
/******************************************************************
 *
 * PDKInt.c
 * PDK interrupt handling.
 *
 *  Author: Vinothkumar S <vinothkumars@ami.com>
 ******************************************************************/

#ifndef __PDKINT__H 
#define  __PDKINT__H 

#include "Types.h"
#include "OSPort.h"
#include "Debug.h"
#include "API.h"
#include "SharedMem.h"
#include "ipmi_int.h"


extern int PDK_RegIntFDs ( struct pollfd **pfd, int **pioctl_wait_req, int **pCount ,int BMCInst);

extern IPMI_INTInfo_T* PDK_CheckForInt ( int interrupt_num , INT8U reading_on_assertion, int BMCInst);

extern int  PDK_RegInt ( IPMI_INTInfo_T *pIntInfo ,int BMCINst);

#endif //__PDKINT__H 
