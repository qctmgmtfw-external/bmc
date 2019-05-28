/****************************************************************
 **                                                            **
 **    (C)Copyright 2009-2015, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross              **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************/

/****************************************************************
 *
 * bt_hw.h
 * Rainier AST series LPC controller BT-related
 * definitions, macros, prototypes
 *
*****************************************************************/

#ifndef __BT_HW_H__
#define __BT_HW_H__

#define BT_REG_BASE		0x1E789000
#define BT_REG_SIZE		0x200
#define BT_IRQ			8

#define BT_CHANNEL_NUM	1

/* channel 3 BT I/O port addresses : 0xE4-0xE6 */
#define BT_ADR3_HI		0x00
#define BT_ADR3_LO		0xE4

/* iBT Register Definitions */
#define LPC_iBTCR0			0x140
#define LPC_iBTCR0_EnableiBT	(1<<0)
#define LPC_iBTCR0_EnClrSvWrP	(1<<2)
#define LPC_iBTCR0_EnClrSvRdP	(1<<3)


#define LPC_iBTCR1			0x144
#define LPC_iBTCR1_H2BI		(1<<0)
#define LPC_iBTCR2			0x148
#define	LPC_iBTCR2_H2BI		(1<<0)
	
#define LPC_iBTCR3			0x14C
#define LPC_iBTCR4			0x150
#define LPC_iBTCR4_SMS_ATN	(1<<4)	
#define LPC_iBTCR4_B2H_ATN	(1<<3)
#define LPC_iBTCR4_B_BUSY	(1<<7)
#define LPC_iBTCR4_H_BUSY   (1<<6) 
#define LPC_iBTCR4_H2B_ATN	(1<<2)
#define LPC_iBTCR4_CLR_RD_PTR	(1<<1)
#define LPC_iBTCR4_CLR_WR_PTR	(1<<0)
#define LPC_iBTCR5			0x154
#define LPC_iBTCR6			0x158
#define LPC_STR4			0x11C
#define LPC_STR4_IBF		(1<<1)

#define LPC_HICR2			0x08
#define LPC_HICR2_LRST		(1<<6)
#define LPC_HICR2_SDWN		(1<<5)
#define LPC_HICR2_ABRT		(1<<4)
#define LPC_HICR2_ERRIE		(1<<0)

#define LPC_STR1			0x3C
#define LPC_STR1_IBF		(1<<1)			
#define LPC_STR2			0x40
#define LPC_STR2_IBF		(1<<1)
#define LPC_STR3			0x44
#define LPC_STR3_IBF		(1<<1)

#define LPC_HICR6			0x84
#define LPC_HICR6_SNP0_STR	(1<<0)
#define LPC_HICR6_SNP1_STR	(1<<1)



#endif /* __BT_HW_H__ */
