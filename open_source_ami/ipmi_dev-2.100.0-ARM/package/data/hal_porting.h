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
/*****************************************************************
 *
 * hal_porting.h
 * HAL hardware porting macros
 *
 * Author: Vinoth kumar S <vinothkumars@ami.com> 
 * 
 *****************************************************************/
#ifndef HAL_HW_PORT_H
#define HAL_HW_PORT_H
 
#include "Types.h"
#include "hal_defs.h"
#include "Debug.h"

/* Memory mapped apis */
#define HAL_MMAP_READ(ADDR,MASK)            (0)//mmap_read32(ADDR,MASK)
#define HAL_MMAP_WRITE(ADDR,MASK,VAL)       (0)    //mmap_write32(ADDR,MASK,VAL)


#endif //#define HAL_HW_PORT_H
