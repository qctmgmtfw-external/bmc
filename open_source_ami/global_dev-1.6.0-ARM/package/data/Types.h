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
 ****************************************************************
 ****************************************************************
 *
 * types.h
 * Standard Type definitions
 *
 * Author: Govind Kothandapani <govindk@ami.com>
 *
 *****************************************************************/
#ifndef TYPES_H
#define TYPES_H

/*----------------------------------------------
 * Include the platform specific types here.
 *----------------------------------------------*/
#include "coreTypes.h"
#include "icc_what.h"

/*----------------------------------------------
 * Bit fields
 *----------------------------------------------*/
#define BIT0    0x0001
#define BIT1	0x0002
#define BIT2	0x0004
#define BIT3	0x0008
#define BIT4	0x0010
#define BIT5	0x0020
#define BIT6	0x0040
#define BIT7	0x0080
#define BIT8	0x0100
#define BIT9	0x0200
#define BIT10   0x0400
#define BIT11   0x0800
#define BIT12   0x1000
#define BIT13   0x2000
#define BIT14   0x4000
#define BIT15   0x8000

/*----------------------------------------------
 * Processor specfic type definetion
 *----------------------------------------------*/
 typedef unsigned char  INT8U;
 typedef unsigned short INT16U;
 typedef unsigned long  INT32U;

 typedef char  INT8S;
 typedef short INT16S;
 typedef long  INT32S;

/*-----------------------------------------------
 * Other Types
 *-----------------------------------------------*/
typedef unsigned int		INTU;

#define _NEAR_
#define _FAR_



#ifndef PACKED 

#ifdef ICC_OS_LINUX
#define PACKED __attribute__ ((packed))
#else
#define PACKED
#endif

#endif 


#endif	/* TYPES_H */
