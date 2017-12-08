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
 * ast_snoop.h
 * ASPEED AST2100/2050/2200/2150 LPC controller Snoop-related
 * definitions, macros, prototypes
 *
*****************************************************************/

#ifndef __AST_SNOOP_H__
#define __AST_SNOOP_H__

#define AST_SNOOP_REG_BASE		0x1E789000
#define AST_SNOOP_REG_SIZE		SZ_4K
#define AST_SNOOP_IRQ			8

#define AST_SNOOP_PORT_NUM		2
#define AST_SNOOP_ADDR_0		0x80
#define AST_SNOOP_ADDR_1		0x81

/* Snoop-related registers of AST LPC controller */
#define AST_LPC_HICR5		0x80
#define AST_LPC_HICR6		0x84
#define AST_LPC_SNPWADR		0x90
#define AST_LPC_SNPWDR		0x94

/* bits of HICR5 */
#define AST_LPC_HICR5_SNP1_ENINT	0x08
#define AST_LPC_HICR5_SNP1_EN		0x04
#define AST_LPC_HICR5_SNP0_ENINT	0x02
#define AST_LPC_HICR5_SNP0_EN		0x01

/* bits of HICR6 */
#define AST_LPC_HICR6_SNP1_STR		0x02
#define AST_LPC_HICR6_SNP0_STR		0x01

/* bits of SNPWADR */
#define AST_LPC_SNPWADR_ADDR1_MASK	0xFFFF0000 /* bits[31:16] */
#define AST_LPC_SNPWADR_ADDR1_SHIFT	16
#define AST_LPC_SNPWADR_ADDR0_MASK	0x0000FFFF /* bits[15:0] */
#define AST_LPC_SNPWADR_ADDR0_SHIFT	0

/* bits of SNPWDR */
#define AST_LPC_SNPWDR_DATA1_MASK	0x0000FF00 /* bits[15:8] */
#define AST_LPC_SNPWDR_DATA1_SHIFT	8
#define AST_LPC_SNPWDR_DATA0_MASK	0x000000FF /* bits[7:0] */
#define AST_LPC_SNPWDR_DATA0_SHIFT	0

#endif /* ! __AST_SNOOP_H__ */

