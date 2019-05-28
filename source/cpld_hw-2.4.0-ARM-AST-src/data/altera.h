/****************************************************************
 **                                                            **
 **    (C)Copyright 2006-2015, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross              **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
****************************************************************/

#ifndef _ALTERA_H_
#define _ALTERA_H_

#define Filter_Version(ID) ((ID)&0x0FFFFFFF)
#define Manufacturer_Identity(ID) ((ID)&0x7FF)

#define	ALTERA_CPLD_MAX_V_DEVNAME	"ALTERA 5M570Z CPLD"
#define	ALTERA_CPLD_MAX_V_IDCODE	0x020A60DD

#define	ALTERA_CPLD_EPM240_DEVNAME	"ALTERA EPM240 CPLD"
#define	ALTERA_CPLD_EPM240_IDCODE	0x020A10DD

#define	ALTERA_CPLD_EPM570_DEVNAME	"ALTERA EPM570 CPLD"
#define	ALTERA_CPLD_EPM570_IDCODE	0x020A20DD

#define	ALTERA_CPLD_EPM1270_DEVNAME	"ALTERA EPM1270 CPLD"
#define	ALTERA_CPLD_EPM1270_IDCODE	0x020A30DD

#define	ALTERA_CPLD_EPM2210_DEVNAME	"ALTERA EPM2210 CPLD"
#define	ALTERA_CPLD_EPM2210_IDCODE	0x020A40DD

#define	ALTERA_CPLD_EPM240Z_DEVNAME	"ALTERA EPM240Z CPLD"
#define	ALTERA_CPLD_EPM240Z_IDCODE	0x020A50DD

#define	ALTERA_CPLD_EPM570Z_DEVNAME	"ALTERA EPM570Z CPLD"
#define	ALTERA_CPLD_EPM570Z_IDCODE	0x020A60DD


// ALTERA CPLD standard command 
#define SIR_BITS        10

#define SAMPLE          0x005
#define EXTEST          0x00F
#define BYPASS          0x3FF
#define USERCODE        0x007
#define IDCODE          0x006
#define HIGHZ           0x00B

#define CLAMP           0x00A
#define USER0           0x00C
#define USER1           0x00E

//program mode
#define	LATTICE_CPLD_NOT_PROGRAM_MODE	0
#define	LATTICE_CPLD_NORMAL_MODE        1
#define	LATTICE_CPLD_BACKGROUND_MODE	2

#define OGRAM_MODE LATTICE_CPLD_BACKGROUND_MODE
#define JTAG_PROGRAM_RETRY_MAX          5 
#define JTAG_ERASE_WHEN_PROGRAM_FAIL

// CPLD Device Related Structure

struct DEVICE_INFO{
  int idcode;
  char name[64];
  int (* func)(uint32_t cmd,unsigned int* buf, struct DEVICE_INFO * const);
  int support_loop_func;
};

/* -------------------------------------------------- */
//#define JTAG_PROGRAM_DEBUG_NOT_SVF
#ifdef JTAG_PROGRAM_DEBUG_NOT_SVF
  #define JTAG_PROGRAM_DEBUG
#endif
/* -------------------------------------------------- */

#endif /* _ALTERA_H_ */
