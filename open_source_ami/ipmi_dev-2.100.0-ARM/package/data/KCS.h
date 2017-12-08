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
 * KCS.h
 * KCS Functions and Macros Definitions.
 *
 *  Author: Rama Rao Bisa <ramab@ami.com>
 ******************************************************************/
#ifndef KCS_H
#define KCS_H

#include "Types.h"
#include "OSPort.h"

/**
 * @brief Initialize KCS interface.
 * @return 0 if success, -1 if error.
**/
extern int InitKCS (void);


/*** External Definitions ***/
#define	KCS_1_CHANNEL			0
#define	KCS_2_CHANNEL			1
#define MAX_KCS_PKT_LEN			(33*1024)


#endif	/* KCS_H */
