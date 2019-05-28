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
 * ast_pwmtach.h
 * ASPEED AST2100/2050/2200/2150/2300/2400 PWM & Fan Tach controller
 * definitions, macros, prototypes
 *
*****************************************************************/

#ifndef __AST_PWMTACH_H__
#define __AST_PWMTACH_H__

#define AST_TACH_CHANNEL_NUM		16
#if defined(SOC_AST2300) || defined(SOC_AST2400) || defined(SOC_AST2500)
#define AST_PWM_EXT_CHANNEL_NUM			4
#define AST_PWM_CHANNEL_NUM			8
#else
#define AST_PWM_CHANNEL_NUM			4
#endif
#define AST_PWMTACH_CLOCK_RATE		24000000

#define AST_PWMTACH_REG_BASE		0x1E786000
#define AST_PWMTACH_REG_SIZE		SZ_4K

/* Tach/PWM controller registers */
#define AST_PWMTACH_GEN				0x00
#define AST_PWMTACH_CLK				0x04
#define AST_PWMTACH_DUTY_0			0x08
#define AST_PWMTACH_DUTY_1			0x0C
#define AST_PWMTACH_TYPE_M_0		0x10
#define AST_PWMTACH_TYPE_M_1		0x14
#define AST_PWMTACH_TYPE_N_0		0x18
#define AST_PWMTACH_TYPE_N_1		0x1C
#define AST_PWMTACH_SRC				0x20
#define AST_PWMTACH_TRIGGER			0x28
#define AST_PWMTACH_RESULT			0x2C
#define AST_PWMTACH_IRQ_CTRL		0x30
#define AST_PWMTACH_IRQ_STATUS		0x34
#define AST_PWMTACH_M_LIMIT			0x38
#define AST_PWMTACH_N_LIMIT			0x3C
#if defined(SOC_AST2300) || defined(SOC_AST2400) || defined(SOC_AST2500)
#define AST_PWMTACH_EXT_GEN			0x40
#define AST_PWMTACH_EXT_CLK			0x44
#define AST_PWMTACH_DUTY_2			0x48
#define AST_PWMTACH_DUTY_3			0x4C
#endif

/* bits of general control register */
#define AST_PWMTACH_GEN_CLK				0x00000001 /* bit 0 */
#define AST_PWMTACH_GEN_PWM_EN			0x00000100 /* bit 8 */
#define AST_PWMTACH_GEN_PWM_TYPE		0x00001000 /* bit 12 */
#define AST_PWMTACH_GEN_TACH_EN			0x00010000 /* bit 16 */

/* bits of tach source register */
#define AST_PWMTACH_SRC_BITS			0x00000011 /* bit [1:0] */

/* bits of PWM clock control register */
#define AST_PWMTACH_CLK_M_CLOCK			0x000000FF /* bits [7:0] */
#define AST_PWMTACH_CLK_M_PERIOD		0x0000FF00 /* bits [15:8] */
#define AST_PWMTACH_CLK_N_CLOCK			0x00FF0000 /* bits [23:16] */
#define AST_PWMTACH_CLK_N_PERIOD		0xFF000000 /* bits [31:24] */

/* bis of tach control register 0 */
#define AST_PWMTACH_FAN_TACH_EN			0x00000001 /* bit 0 */
#define AST_PWMTACH_FAN_TACH_CLK		2
#define AST_PWMTACH_FAN_TACH_MODE		4
#define AST_PWMTACH_FAN_TACH_SMART		0x00000080 /* bit 7 */
#define AST_PWMTACH_FAN_TACH_PERIOD		0xFFFF0000 /* bits [31:16] */

/* bis of tach control register 1 */
#define AST_PWMTACH_FAN_TACH_RISING		0x0000FFFF /* bits [15:0] */
#define AST_PWMTACH_FAN_TACH_FALLING	0xFFFF0000 /* bits [31:16] */

/* bits of PWM duty control register */
#define AST_PWMTACH_DUTY_ID_PWM			0x0000FFFF /* bits[15:0] */
#define AST_PWMTACH_DUTY_ID_SHIFT		16
#define AST_PWMTACH_DUTY_RISING			0
#define AST_PWMTACH_DUTY_FALLING		8

/* bits of tach result register */
#define AST_PWMTACH_RESULT_STATUS		0x80000000 /* bit 31 */
#define AST_PWMTACH_RESULT_VALUE		0x000FFFFF /* bits[19:0] */

/* bits of interrupt control register */
#define AST_PWMTACH_IRQ_EN				0x00000001 /* bit 0 */

/* bits of interrupt status register */
#define AST_PWMTACH_IRQ_PENDING			0x00000001 /* bit 0 */

#define AST_PWMTACH_NO_FAN_VALUE1		0x000FFFFE
#define AST_PWMTACH_NO_FAN_VALUE2		0x000FFFFF

#define AST_PWMTACH_TYPE_M		0
#define AST_PWMTACH_TYPE_N		1

#define AST_SCU_UNLOCK_MAGIC	0x1688A8A8

#endif /* __AST_PWMTACH_H__ */

