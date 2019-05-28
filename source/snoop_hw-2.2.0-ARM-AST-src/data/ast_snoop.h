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
 * ast_snoop.h
 * ASPEED AST2100/2050/2200/2150 LPC controller Snoop-related
 * definitions, macros, prototypes
 *
*****************************************************************/

#ifndef __AST_SNOOP_H__
#define __AST_SNOOP_H__

#if defined(SOC_AST2300)
#define GROUP_AST2300 1
#endif
#if defined(SOC_AST2400)
#define GROUP_AST2300_PLUS 1
#endif
#if defined(CONFIG_SPX_FEATURE_BMCCOMPANIONDEVICE_AST1070)
#define GROUP_AST1070_COMPANION 1
#elif defined(SOC_AST1250)
#define GROUP_AST2300_PLUS 1
#endif
#if defined(SOC_AST1050_1070)
#define GROUP_AST1070_COMPANION 1
#endif

#define AST_SNOOP_REG_BASE		0x1E789000
#define AST_SNOOP_REG_SIZE		SZ_4K
#define AST_SNOOP_IRQ			8

#if defined(GROUP_AST1070_COMPANION)
#define AST_SNOOP_PORT_NUM		(4*CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES)
#else
#define AST_SNOOP_PORT_NUM		1
#endif
#define AST_SNOOP_ADDR_0		0x80
#define AST_SNOOP_ADDR_1		0x81

/* Snoop-related registers of AST LPC controller */
#define AST_LPC_HICR5		0x80
#define AST_LPC_HICR6		0x84
#define AST_LPC_SNPWADR		0x90
#define AST_LPC_SNPWDR		0x94
#if defined(GROUP_AST1070_COMPANION)
#define AST_BMCCC_LPC_80H_ADDR0_REG 0x220
#define AST_BMCCC_LPC_80H_ADDR1_REG 0x224
#define AST_BMCCC_LPC_80H_DATA_REG  0x228
#define AST_BMCCC_LPC_80H_CTRL_REG  0x22C

/* bits of 80H ADDR0 */
#define AST_BMCCC_LPC_ADDR0_MASK	0x000000FF /* bits[7:0] */
#define AST_BMCCC_LPC_ADDR0_SHIFT	0

/* bits of 80H DATA */
#define AST_BMCCC_LPC_DATA_MASK	    0x000000FF /* bits[7:0] */
#define AST_BMCCC_LPC_DATA_SHIFT	0

/* bits of 80H CONTROL */
#define AST_BMCCC_LPC_80H_WR_EVNT   0x10
#define AST_BMCCC_LPC_80H_INT_EN    0x02
#define AST_BMCCC_LPC_80H_EN        0x01
#endif

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

