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

/****************************************************************
 *
 * ast_lpcreset.h
 * ASPEED AST2100/2050/2200/2150 LPC controller reset-related
 * definitions, macros, prototypes
 *
*****************************************************************/

#ifndef __AST_LPCRESET_H__
#define __AST_LPCRESET_H__

#define AST_LPCRESET_REG_BASE	0x1E789000
#define AST_LPCRESET_REG_SIZE	SZ_4K
#define AST_LPCRESET_IRQ		8

#define AST_LPCRESET_HW_NUM		1

/* reset-related registers of AST LPC controller */
#define AST_LPC_HICR2			0x08
#define AST_LPC_HICR5			0x80 
#define AST_LPC_HICR6			0x84

/* bits of HICR2, interrupt control register */
#define AST_LPC_HICR2_LRST		0x40
#define AST_LPC_HICR2_SDWN		0x20
#define AST_LPC_HICR2_ABRT		0x10
#define AST_LPC_HICR2_IBFIE3	0x08
#define AST_LPC_HICR2_IBFIE2	0x04
#define AST_LPC_HICR2_IBFIE1	0x02
#define AST_LPC_HICR2_ERRIE		0x01

/* bits of HICR5 */
#define AST_LPC_HICR5_SNP1_ENINT		0x08
#define AST_LPC_HICR5_SNP1_EN			0x04
#define AST_LPC_HICR5_SNP0_ENINT		0x02
#define AST_LPC_HICR5_SNP0_EN			0x01

/* bits of HICR6 */
#define AST_LPC_HICR6_SNP1_STR			0x02
#define AST_LPC_HICR6_SNP0_STR			0x01

#endif /* ! __AST_LPCRESET_H__ */

