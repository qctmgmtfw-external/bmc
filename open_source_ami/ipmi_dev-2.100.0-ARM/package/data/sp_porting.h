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
 *
 * sp_porting.h
 * Platfor specfic porting
 *
 * Author: Govind Kothandapani <govindk@ami.com>
 *
 *****************************************************************/

#ifndef SP_PORTING_H
#define SP_PORTING_H
#include "sp_types.h"
#include "SoftProc.h"
#include "OSPort.h"
#include "PDKHooks.h"
#include "Debug.h"
#include "PDKHW.h"
#include "libi2c.h"
#include "memory.h"
#include "gpio_info.h"


#define IPMI_Malloc malloc


#define SP_MALLOC(SIZE, PTR)	\
do								\
{								\
	PTR = IPMI_Malloc (SIZE);	\
} while (0)

#define SP_MEMCPY			memcpy
#define SP_MEMSET			memset
#define SP_STRNCMP			strncmp
#define SP_ASSERT(P1)	    IPMI_ASSERT (P1);
#define SP_DELAY_MS(MS)		OS_TIME_DELAY_HMSM (0, 0, 0, MS)


#if defined (HOST_BIG_ENDIAN)

#define SPTOH16(VAL)    VAL
#define HTOSP16(VAL)    VAL
#define SPTOH32(VAL)    VAL
#define HTOSP32(VAL)    VAL

#elif defined (HOST_LITTLE_ENDIAN)

#define SPTOH16(P1)			((u16)(((P1) >> 8) + ((P1) << 8)))
#define HTOSP16(P1)			((u16)(((P1) >> 8) + ((P1) << 8)))
#define SPTOH32(P1)			((SPTOH16 ((P1) & 0xffff) << 16) + (SPTOH16 ((P1) >> 16)))
#define HTOSP32(P1)			((HTOSP16 ((P1) & 0xffff) << 16) + (HTOSP16 ((P1) >> 16)))

#else

#error "Error HOST BYTE ORDER/ENDIAN not defined"

#endif

#define SPTOHU16_32(VAL) SPTOH16(VAL)
#define HTOSPU16_32(VAL) HTOSP16(VAL)


#define SP_ENTER_CRITICAL_SECTION()
#define SP_EXIT_CRITICAL_SECTION()
#define SP_ACQUIRE_SLOCK()
#define SP_RELEASE_SLOCK()

#define SP_OEM_CMD(CMD, REGS, PRED, MEM)    (PDK_SPOEMInstruction (CMD, (SPRegs_T*)REGS, MEM))
#define SP_READ_I2C_REG(BUS, SA, REG, VAL) 	(0)
#define SP_WRITE_I2C_REG(BUS, SA, REG, VAL) 	(0)
#define SP_RW_I2C(BUS, SA, NUM_WRITE, WRITE_BUF, NUM_READ, READ_BUF) (0)

#define SP_READ_GPIO(MEM,VAL,pErr)   do { \
*pErr  = get_gpio_data (SPTOH16 (MEM));   \
 if ( -1 != *pErr )  VAL = (u8) (*pErr);  \
}while (0)

#define SP_WRITE_GPIO(MEM,VAL,pErr)          			\
do {					        		\
	if (VAL) { if ( -1 == set_gpio_data_high (SPTOH16 (MEM)))  *pErr = -1;  }    	\
   	  else   { if ( -1 == set_gpio_data_low  (SPTOH16 (MEM)))  *pErr = -1;  }    	\
} while (0)

#define SP_GET_DIR(MEM,VAL,pErr)   do { \
*pErr  = get_gpio_dir (SPTOH16 (MEM));   \
 if ( -1 != *pErr )  VAL = (u8) (*pErr);  \
}while (0)
 
#define SP_SET_DIR(MEM,VAL,pErr)          	\
do {					        \
	if (VAL) {  set_gpio_dir_output  (SPTOH16 (MEM));   }   \
   	  else   {  set_gpio_dir_input   (SPTOH16 (MEM));  }    \
} while (0)

#define SP_GET_POL(MEM,VAL,pErr)    do { \
*pErr  = get_gpio_pol (SPTOH16 (MEM));   \
 if ( -1 != *pErr )  VAL = (u8) (*pErr); \
}while (0)


#define SP_SET_POL(MEM,VAL,pErr)     do {			        \
	if (VAL) {  set_gpio_pol_high (SPTOH16 (MEM));   }    	\
   	  else   {  set_gpio_pol_low  (SPTOH16 (MEM));  }    	\
} while (0)


#define SP_READ_HOST_MEM_BYTE(VAL, MEM)    (VAL = get_gpio_data (SPTOH16 (MEM)))
#define SP_READ_HOST_MEM_WORD(VAL, MEM)    (VAL = get_gpio_data (SPTOH16 (MEM)))
#define SP_WRITE_HOST_MEM_BYTE(MEM, VAL)				\
do {													\
	if (VAL) { set_gpio_high (SPTOH16 (MEM));}    	\
    else     { set_gpio_low  (SPTOH16 (MEM));}    	\
} while (0)


#define SP_WRITE_HOST_MEM_WORD(MEM, VAL)				\
do {													\
	if (VAL) { set_gpio_high (SPTOH16 (MEM));}    	\
    else     { set_gpio_low  (SPTOH16 (MEM));}    	\
} while (0)



/***  DEBUG MACROS ***/
#define SP_DBG_INIT()					PDK_DebugPortInit ()
#define SP_DBG_WRITE_BYTE(BYTE)			PDK_DbgOut (BYTE)
#define SP_DBG_READ_BYTE(PTRBYTE)		PDK_DbgIn (PTRBYTE)

#endif //SP_PORTING_H
