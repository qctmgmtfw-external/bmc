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
 * ast_kcs.h
 * ASPEED AST2100/2050/2200/2150/2300/2400 LPC controller KCS-related
 * definitions, macros, prototypes
 *
*****************************************************************/

#ifndef __AST_KCS_H__
#define __AST_KCS_H__

#define AST_KCS_REG_BASE            0x1E789000
#define AST_KCS_REG_SIZE            SZ_4K
#define AST_KCS_IRQ                 8

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

#if defined(GROUP_AST2300) || defined(GROUP_AST2300_PLUS)
#define AST_KCS_CHANNEL_NUM         4
#elif defined(GROUP_AST1070_COMPANION)
#define AST_KCS_CHAN_NUM_EACH_NODE  3
#define AST_NODE_NUM                4
#define AST_KCS_CHANNEL_NUM         (AST_KCS_CHAN_NUM_EACH_NODE * AST_NODE_NUM * CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES)
#else
#define AST_KCS_CHANNEL_NUM         3
#endif

#define ERROR_STATE                 0xC0

/* channel 1 I/O port address : 0xCA0 0xCA4*/
#define AST_KCS_ADR1_HI         0x0C 
#define AST_KCS_ADR1_LO         0xA0

/* channel 2 I/O port address : 0xCA8 0xCAC*/
#define AST_KCS_ADR2_HI         0x0C
#define AST_KCS_ADR2_LO         0xA8

/* channel 3 I/O port address : 0xCA2 0xCA3*/
#define AST_KCS_ADR3_HI         0x0C
#define AST_KCS_ADR3_LO         0xA2

/* channel 4 I/O port address : 0xCA6 0xCA7  */
#define AST_KCS_ADR4            0x0CA70CA6

/* KCS-related registers of AST LPC controller */
#define AST_LPC_HICR0           0x00
#define AST_LPC_HICR1           0x04
#define AST_LPC_HICR2           0x08
#define AST_LPC_HICR3           0x0C
#define AST_LPC_HICR4           0x10
#define AST_LPC_LADR3H          0x14
#define AST_LPC_LADR3L          0x18
#define AST_LPC_LADR12H         0x1C
#define AST_LPC_LADR12L         0x20
#define AST_LPC_IDR1            0x24
#define AST_LPC_IDR2            0x28
#define AST_LPC_IDR3            0x2C
#define AST_LPC_ODR1            0x30
#define AST_LPC_ODR2            0x34
#define AST_LPC_ODR3            0x38
#define AST_LPC_STR1            0x3C
#define AST_LPC_STR2            0x40
#define AST_LPC_STR3            0x44
#define AST_LPC_SIRQCR0         0x70
#define AST_LPC_SIRQCR1         0x74
#define AST_LPC_SIRQCR2         0x78
#define AST_LPC_SIRQCR3         0x7C
#define AST_LPC_HICR5           0x80
#define AST_LPC_HICR6           0x84

#if defined(GROUP_AST2300) || defined(GROUP_AST2300_PLUS)
#define AST_LPC_HICRB           0x100
#define AST_LPC_IDR4            0x114
#define AST_LPC_ODR4            0x118
#define AST_LPC_STR4            0x11C
#define AST_LPC_LADR4           0x110
#define LPC_iBTCR2              0x148
#endif

#if defined(GROUP_AST1070_COMPANION)
#define AST_LPC_IDR_CH(ch)    (AST_LPC_IDR1 + ((ch%AST_KCS_CHAN_NUM_EACH_NODE) * 0x04))
#define AST_LPC_ODR_CH(ch)    (AST_LPC_ODR1 + ((ch%AST_KCS_CHAN_NUM_EACH_NODE) * 0x04))
#define AST_LPC_STR_CH(ch)    (AST_LPC_STR1 + ((ch%AST_KCS_CHAN_NUM_EACH_NODE) * 0x04))
#else
#define AST_LPC_IDR_CH(ch)    (AST_LPC_IDR1 + (ch * 0x04))
#define AST_LPC_ODR_CH(ch)    (AST_LPC_ODR1 + (ch * 0x04))
#define AST_LPC_STR_CH(ch)    (AST_LPC_STR1 + (ch * 0x04))
#endif


//#define AST_LPC_STR_CH(ch)    (ch==3)?AST_LPC_STR4:(AST_LPC_STR1 + (ch * 0x04))

/* bits of HICR0 */
#define AST_LPC_HICR0_LPC3E         0x80
#define AST_LPC_HICR0_LPC2E         0x40
#define AST_LPC_HICR0_LPC1E         0x20
#define AST_LPC_HICR0_SDWNE         0x08
#define AST_LPC_HICR0_PMEE          0x04

/* bits of HICR1 */
#define AST_LPC_HICR1_LPCBSY        0x80
#define AST_LPC_HICR1_CLKREQ        0x40
#define AST_LPC_HICR1_IRQBSY        0x20
#define AST_LPC_HICR1_LRSTB         0x10
#define AST_LPC_HICR1_SDWNB         0x08
#define AST_LPC_HICR1_PMEB          0x04

/* bits of HICR2, interrupt control register */
#define AST_LPC_HICR2_LRST          0x40
#define AST_LPC_HICR2_SDWN          0x20
#define AST_LPC_HICR2_ABRT          0x10
#define AST_LPC_HICR2_IBFIE3        0x08
#define AST_LPC_HICR2_IBFIE2        0x04
#define AST_LPC_HICR2_IBFIE1        0x02
#define AST_LPC_HICR2_ERRIE         0x01

/* bits of HICR3, pin states regsiter */
#define AST_LPC_HICR3_LFRAME        0x80
#define AST_LPC_HICR3_CLKRUN        0x40
#define AST_LPC_HICR3_SERIRQ        0x20
#define AST_LPC_HICR3_LRESET        0x10
#define AST_LPC_HICR3_LPCPD         0x08
#define AST_LPC_HICR3_PME           0x04

/* bits of HICR4, selection register */
#define AST_LPC_HICR4_LADR12SEL     0x80
#define AST_LPC_HICR4_KCSENBL       0x04
#define AST_LPC_HICR4_BTENBL        0x01

/* bits of STR[1:3], data full register */
#define AST_LPC_STR_CD              0x08
#define AST_LPC_STR_SMS_ATN         0x04
#define AST_LPC_STR_IBFA            0x02
#define AST_LPC_STR_OBFA            0x01

/* bits of HICR6 */
#define AST_LPC_HICR6_SNP1_STR      0x02
#define AST_LPC_HICR6_SNP0_STR      0x01

#if defined(GROUP_AST2300) || defined(GROUP_AST2300_PLUS)
/* bits of HICRB */
#define AST_LPC_HICRB0_KCS4E        0x01
#define AST_LPC_HICRB0_KCS4INTE     0x02
/* bits of iBTCR2 */
#define    LPC_iBTCR2_H2BI          (1<<0)
#endif

#endif /* ! __AST_KCS_H__ */
