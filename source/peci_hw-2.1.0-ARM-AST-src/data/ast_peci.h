/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2009-2015, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross              **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 ****************************************************************/

/****************************************************************
 *
 * ast_peci.h
 * ASPEED AST2100/2050/2200/2150 PECI controller
 * definitions, macros, prototypes
 *
*****************************************************************/

#ifndef __AST_PECI_H__
#define __AST_PECI_H__

#define AST_PECI_CHANNEL_NUM	4

#define AST_PECI_REG_BASE		0x1E78B000
#define AST_PECI_REG_SIZE		SZ_4K
#define AST_PECI_IRQ			15

#define AST_PECI_XFER_BUF_SZ	16
#define AST_PECI_TIMEOUT		1000 /* milli seconds */

#define AST_PECI_MSG_TIMING_PERIOD		64
#define AST_PECI_ADDR_TIMING_PERIOD		64

/* registers of PECI controller */
#define AST_PECI_CTRL			0x00
#define AST_PECI_TIMING			0x04
#define AST_PECI_CMD			0x08
#define AST_PECI_RW_LEN			0x0C
#define AST_PECI_HW_FCS			0x10
#define AST_PECI_CAP_FCS		0x14
#define AST_PECI_ICR			0x18
#define AST_PECI_ISR			0x1C
#define AST_PECI_W_DATA_0		0x20
#define AST_PECI_W_DATA_1		0x24
#define AST_PECI_W_DATA_2		0x28
#define AST_PECI_W_DATA_3		0x2C
#define AST_PECI_R_DATA_0		0x30
#define AST_PECI_R_DATA_1		0x34
#define AST_PECI_R_DATA_2		0x38
#define AST_PECI_R_DATA_3		0x3C

/* bits of PECI control register */
#define AST_PECI_CTRL_ENABLE_CLK			0x00000001
#define AST_PECI_CTRL_AUTO_GEN_AWFCS		0x00000002
#define AST_PECI_CTRL_ENABLE_PECI			0x00000010
#define AST_PECI_CTRL_CONTENTION			0x00000020
#define AST_PECI_CTRL_CLK_SHIFT				8
#define AST_PECI_CTRL_CLK_MASK				0x00000F00 /* bits[11:8] */
#define AST_PECI_CTRL_READ_MODE_SHIFT		12
#define AST_PECI_CTRL_READ_MODE_MASK		0x00003000 /* bits[13:12] */
#define AST_PECI_CTRL_SAMPLING_SHIFT		16
#define AST_PECI_CTRL_SAMPLING_MASK			0x000F0000 /* bits[19:16] */

/* read mode */
#define AST_PECI_CTRL_READ_MODE_POINT		0x0
#define AST_PECI_CTRL_READ_MODE_PULSE		0x1
#define AST_PECI_CTRL_READ_MODE_DEBUG		0x2

/* clock divider */
#define AST_PECI_CTRL_CLK_1		0x0
#define AST_PECI_CTRL_CLK_2		0x1
#define AST_PECI_CTRL_CLK_4		0x2
#define AST_PECI_CTRL_CLK_8		0x3
#define AST_PECI_CTRL_CLK_16	0x4
#define AST_PECI_CTRL_CLK_32	0x5
#define AST_PECI_CTRL_CLK_64	0x6
#define AST_PECI_CTRL_CLK_128	0x7

/* read sampling point */
#define AST_PECI_CTRL_SAMPLE_POINT_0	0x0
#define AST_PECI_CTRL_SAMPLE_POINT_1	0x1
#define AST_PECI_CTRL_SAMPLE_POINT_2	0x2
#define AST_PECI_CTRL_SAMPLE_POINT_3	0x3
#define AST_PECI_CTRL_SAMPLE_POINT_4	0x4
#define AST_PECI_CTRL_SAMPLE_POINT_5	0x5
#define AST_PECI_CTRL_SAMPLE_POINT_6	0x6
#define AST_PECI_CTRL_SAMPLE_POINT_7	0x7
#define AST_PECI_CTRL_SAMPLE_POINT_8	0x8
#define AST_PECI_CTRL_SAMPLE_POINT_9	0x9
#define AST_PECI_CTRL_SAMPLE_POINT_10	0xA
#define AST_PECI_CTRL_SAMPLE_POINT_11	0xB
#define AST_PECI_CTRL_SAMPLE_POINT_12	0xC
#define AST_PECI_CTRL_SAMPLE_POINT_13	0xD
#define AST_PECI_CTRL_SAMPLE_POINT_14	0xE
#define AST_PECI_CTRL_SAMPLE_POINT_15	0xF

/* bits of Timing negotiation register */
#define AST_PECI_TIMING_MSG_SHIFT		8
#define AST_PECI_TIMING_MSG_MASK		0x0000FF00 /* bits[15:8] */
#define AST_PECI_TIMING_ADDR_SHIFT		0
#define AST_PECI_TIMING_ADDR_MASK		0x000000FF /* bits[7:0] */

/* bits of PECI command register */
#define AST_PECI_CMD_STATE_SHIFT		24
#define AST_PECI_CMD_STATE_MASK			0x0F000000 /* bits[27:24] */
#define AST_PECI_CMD_FIRE				0x00000001

/* bits of read/write length register */
#define AST_PECI_RW_LEN_ENABLE_AW_FCS	0x80000000 /* bit 31 */
#define AST_PECI_RW_LEN_READ_SHIFT		16
#define AST_PECI_RW_LEN_READ_MASK		0x00FF0000 /* bits[23:16] */
#define AST_PECI_RW_LEN_WRITE_SHIFT		8
#define AST_PECI_RW_LEN_WRITE_MASK		0x0000FF00 /* bits[15:8] */
#define AST_PECI_RW_LEN_ADDR_SHIFT		0
#define AST_PECI_RW_LEN_ADDR_MASK		0x000000FF /* bits[7:0] */

/* bits of interrupt control/status regisgter */
#define AST_PECI_IRQ_DONE				0x00000001
#define AST_PECI_IRQ_WRITE_FCS_ABORT	0x00000002
#define AST_PECI_IRQ_WRITE_FCS_BAD		0x00000004
#define AST_PECI_IRQ_BUS_CONTENT		0x00000008
#define AST_PECI_IRQ_TIMING_SEL_SHIFT	30
#define AST_PECI_IRQ_TIMING_SEL_MASK	0xC0000000

#define AST_PECI_IRQ_TIMING_FIRST_BIT	0x00
#define AST_PECI_IRQ_TIMING_SECOND_BIT	0x01
#define AST_PECI_IRQ_TIMING_MSB			0x02

/* bits of expected FCS register */
#define AST_PECI_HW_FCS_AW_FCS_P_SHIFT		24
#define AST_PECI_HW_FCS_AW_FCS_P_MASK		0xFF000000 /* bits[31:24] */
#define AST_PECI_HW_FCS_RD_FCS_SHIFT		16
#define AST_PECI_HW_FCS_RD_FCS_MASK			0x00FF0000 /* bits[23:16] */
#define AST_PECI_HW_FCS_AW_FCS_A_SHIFT		8
#define AST_PECI_HW_FCS_AW_FCS_A_MASK		0x0000FF00 /* bits[15:8] */
#define AST_PECI_HW_FCS_WR_FCS_SHIFT		0
#define AST_PECI_HW_FCS_WR_FCS_MASK			0x000000FF /* bits[7:0] */

/* bits of captured FCS register */
#define AST_PECI_CAP_FCS_RD_FCS_SHIFT		16
#define AST_PECI_CAP_FCS_RD_FCS_MASK		0x00FF0000 /* bits[23:16] */
#define AST_PECI_CAP_FCS_WR_FCS_SHIFT		0
#define AST_PECI_CAP_FCS_WR_FCS_MASK		0x000000FF /* bits[7:0] */

/* state of PECI controller */
#define AST_PECI_CTRL_STATE_IDLE		0x00
#define AST_PECI_CTRL_STATE_FIRE		0x01
#define AST_PECI_CTRL_STATE_INIT		0x02
#define AST_PECI_CTRL_STATE_ADDR_TIME	0x03
#define AST_PECI_CTRL_STATE_ADDR		0x04
#define AST_PECI_CTRL_STATE_MSG_TIME	0x05
#define AST_PECI_CTRL_STATE_LEN			0x06
#define AST_PECI_CTRL_STATE_WRITE_DATA	0x07
#define AST_PECI_CTRL_STATE_WRITE_FCS	0x09
#define AST_PECI_CTRL_STATE_READ_DATA	0x0A
#define AST_PECI_CTRL_STATE_READ_FCS	0x0B
#define AST_PECI_CTRL_STATE_STOP		0x0C

#endif /* __AST_PECI_H__ */
