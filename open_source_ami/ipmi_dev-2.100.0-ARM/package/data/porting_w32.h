/*************************************************************** 
**                                                            **
**    (C)Copyright 2006-2009,American Megatrends Inc.         **
**                                                            **
**            All Rights Reserved.                            **
**                                                            **
**        5555 Oakbrook Pkwy Suite 200, Norcross,             **
**                                                            **
**        Georgia - 30093, USA. Phone-(770)-246-8600.         **
**                                                            **
***************************************************************/

//#include "Debug.h"


#define SP_READ_HOST_MEM_BYTE(P1, P2)
#define SP_READ_HOST_MEM_WORD(P1, P2)
#define SP_WRITE_HOST_MEM_BYTE(P1, P2)
#define SP_WRITE_HOST_MEM_WORD(P1, P2)
#define SP_GET_ACPI_STATE(ACPI_STATE)   (ACPI_STATE = 1);
#define SP_READ_I2C_REG(BUS, SA, REG, VAL)	(VAL = 1)
#define SP_WRITE_I2C_REG(P1, P2, P3, P4)	0
#define SP_RW_I2C(P1, P2, P3, P4, P5, P6)	0
#define SP_ENTER_CRITICAL_SECTION()
#define SP_EXIT_CRITICAL_SECTION()
#define SP_ACQUIRE_SLOCK()
#define SP_RELEASE_SLOCK()
#define SP_OEM_CMD(P1, P2, P3, P4)
 

#define SP_MALLOC(SIZE, PTR)	\
do								\
{								\
	PTR = malloc (SIZE);		\
} while (0)

#define SP_MEMCPY			memcpy
#define SP_MEMSET			memset
#define SP_STRNCMP			strncmp
#define SP_ASSERT(P1)	    assert (P1);
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

/***  DEBUG MACROS ***/ 
#define SP_DBG_INIT(BAUD_RATE)
#define SP_DBG_WRITE_BYTE(BYTE)
#define SP_DBG_READ_BYTE(BYTE)
