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
 * ast_lpcreset.h
 * ASPEED AST2100/2050/2200/2150 LPC controller reset-related
 * definitions, macros, prototypes
 *
*****************************************************************/

#ifndef __AST_LPCRESET_H__
#define __AST_LPCRESET_H__

#define AST_LPCRESET_REG_BASE       0x1E789000
#define AST_LPCRESET_REG_SIZE       SZ_4K
#define AST_LPCRESET_IRQ            8


#if defined(SOC_AST1050_1070) || defined(CONFIG_SPX_FEATURE_BMCCOMPANIONDEVICE_AST1070)
#define GROUP_AST1070_COMPANION 1
#endif

#if defined(GROUP_AST1070_COMPANION)
#define AST_NODE_NUM                4
#define AST_LPCRESET_HW_NUM         (AST_NODE_NUM * CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES)
#else 
#define AST_LPCRESET_HW_NUM         1
#endif

/* reset-related registers of AST LPC controller */
#define AST_LPC_HICR2               0x08
#define AST_LPC_HICR5               0x80 
#define AST_LPC_HICR6               0x84

/* BT registers of AST LPC controller */
#define AST_LPC_IBTCR1              0x144
#define AST_LPC_IBTCR2              0x148

/* Bits of IBTCR1 */
#define AST_LPC_IBTCR1_H2B_INTR     0x01

/* bits of HICR2, interrupt control register */
#define AST_LPC_HICR2_LRST          0x40
#define AST_LPC_HICR2_SDWN          0x20
#define AST_LPC_HICR2_ABRT          0x10
#define AST_LPC_HICR2_IBFIE3        0x08
#define AST_LPC_HICR2_IBFIE2        0x04
#define AST_LPC_HICR2_IBFIE1        0x02
#define AST_LPC_HICR2_ERRIE         0x01

/* bits of HICR5 */
#define AST_LPC_HICR5_SNP1_ENINT    0x08
#define AST_LPC_HICR5_SNP1_EN       0x04
#define AST_LPC_HICR5_SNP0_ENINT    0x02
#define AST_LPC_HICR5_SNP0_EN       0x01

/* bits of HICR6 */
#define AST_LPC_HICR6_BAUD_INTR     0x08
#define AST_LPC_HICR6_PME_INTR      0x04
#define AST_LPC_HICR6_SNP1_STR      0x02
#define AST_LPC_HICR6_SNP0_STR      0x01

#endif /* ! __AST_LPCRESET_H__ */

