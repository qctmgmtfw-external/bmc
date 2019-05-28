/****************************************************************
 **                                                            **
 **    (C)Copyright 2006-2009, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross              **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
****************************************************************/

#ifndef _AST_JTAG_MASTER_H_
#define _AST_JTAG_MASTER_H_

#define AST_JTAG_REG_BASE 0x1E6E4000
#define AST_JTAG_PORT_NUM 1

#ifdef SOC_AST2300
 #define AST_GPIO_REG_BASE 0x1E780000
 #define GPIO_DATA_VALUE   0x20
 #define GPIO_DIRECTION		 0x24
 #define GPIO_G5_BIT       0x00200000
#endif

// register
#define	JTAG_DATA		    0x00
#define	JTAG_INSTRUCTION	0x04
#define	JTAG_CONTROL		0x08
#define	JTAG_INTERRUPT		0x0c
#define	JTAG_STATUS		    0x10
#define	JTAG_TCK_CONTROL	0x14

// engine control
#define	AST_JTAG_CTRL_ENG_OUT_DIS	0x00000000
//#define		AST_JTAG_CTRL_ENG_OUT_DIS	0x80000000
#define	AST_JTAG_CTRL_ENGINE_ENABLE	0x80000000
#define	AST_JTAG_CTRL_OUTPUT_ENABLE	0x40000000
#define	AST_JTAG_CTRL_FORCE_RESET	0x20000000
#define	AST_JTAG_CTRL_ENABLE		(AST_JTAG_CTRL_ENGINE_ENABLE | AST_JTAG_CTRL_OUTPUT_ENABLE)
#define	AST_JTAG_CTRL_INST_LAST_TRANS	0x00020000
#define	AST_JTAG_CTRL_INST_ENABLE	0x00010000
#define	AST_JTAG_CTRL_INST_LEN_OFFSET	20
#define	AST_JTAG_CTRL_DATA_LAST_TRANS	0x00000002
#define	AST_JTAG_CTRL_DATA_ENABLE	0x00000001
#define	AST_JTAG_CTRL_DATA_LEN_OFFSET	4
#define	AST_JTAG_CTRL_MAX_DATA_LENGTH	32
#define	DEVICE_INS_LENGTH		8

// interrupt status / enable / flag
#define	AST_JTAG_INTR_INST_PAUSE	0x00080000
#define	AST_JTAG_INTR_INST_COMPLETE	0x00040000
#define	AST_JTAG_INTR_DATA_PAUSE	0x00020000
#define	AST_JTAG_INTR_DATA_COMPLETE	0x00010000
#define	AST_JTAG_INTR_STATUS_MASK	(AST_JTAG_INTR_INST_PAUSE | AST_JTAG_INTR_INST_COMPLETE | AST_JTAG_INTR_DATA_PAUSE | AST_JTAG_INTR_DATA_COMPLETE)
#define	AST_JTAG_INTR_EN_INST_PAUSE	0x00000008
#define	AST_JTAG_INTR_EN_INST_COMP	0x00000004
#define	AST_JTAG_INTR_EN_DATA_PAUSE	0x00000002
#define	AST_JTAG_INTR_EN_DATA_COMP	0x00000001
#define	AST_JTAG_INTR_ENABLE_MASK	(AST_JTAG_INTR_EN_INST_PAUSE | AST_JTAG_INTR_EN_INST_COMP | AST_JTAG_INTR_EN_DATA_PAUSE | AST_JTAG_INTR_EN_DATA_COMP)
#define	AST_JTAG_INTR_STATUS_INITIAL		0
#define	AST_JTAG_INTR_STATUS_INST_COMPLETE	1
#define	AST_JTAG_INTR_STATUS_DATA_PAUSE		2
#define	AST_JTAG_INTR_STATUS_DATA_COMPLETE	3

// Software mode register
#define SOFTWARE_MODE_ENABLE    0x00080000
#define SOFTWARE_TCK_BIT        0x00040000
#define SOFTWARE_TMS_BIT        0x00020000
#define SOFTWARE_TDIO_BIT       0x00010000

//Jtag CLK
#define JTAG_PROGRAM_CLK 0x000007ff       // 11.7kHz slowest!!
#define JTAG_FASTER_READ_CLK 0x00000004


/* -------------------------------------------------- */
extern int g_is_Support_LoopFunc;
extern void ast_jtag_reset (void);
extern void jtag_sdr(unsigned short bits, unsigned int *TDI,unsigned int *TDO);
extern void jtag_sir(unsigned short bits, unsigned int TDI);
extern void jtag_runtest_idle(unsigned int tcks, unsigned int min_mSec);
extern void jtag_dr_pause(unsigned int min_mSec);

/* -------------------------------------------------- */
//#define JTAG_PROGRAM_DEBUG_NOT_SVF
#ifdef JTAG_PROGRAM_DEBUG_NOT_SVF
  #define JTAG_PROGRAM_DEBUG
#endif
/* -------------------------------------------------- */
#define JTAG_IDLE_OFF_TCK

#define DEBUG
#endif /* _AST_JTAG_MASTER_H_ */
