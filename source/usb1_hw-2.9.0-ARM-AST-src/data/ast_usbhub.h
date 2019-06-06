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
 * ast_usbhub.h
 * ASPEED AST2100/2050/2200/2150/2300/2400
 * virtual USB hub controller definitions, macros, prototypes
 *
*****************************************************************/

#ifndef __AST_USBHUB_H__
#define __AST_USBHUB_H__

/*-----------------------------------------------------------------------------------------------*/
/* hardware information about SCU */

#define AST_SCU_REG_BASE		0x1E6E2000
#define AST_SCU_REG_SIZE		SZ_4K
#define AST_SCU_UNLOCK_MAGIC	0x1688A8A8

/* registers of SCU */
#define AST_SCU_LOCK_KEY		0x00
#define AST_SCU_RESET_CTRL		0x04
#define AST_SCU_CLK_STOP		0x0C

#define AST_SCU_RESET_CTRL_UDC	0x00004000 /* bit 14 */
#define AST_SCU_CLK_STOP_UDC	0x00004000 /* bit 14 */

/*-----------------------------------------------------------------------------------------------*/
/* hardware information about UDC hub */

#define AST_USBHUB_REG_BASE		0x1E6A0000
#define AST_USBHUB_REG_SIZE		SZ_4K
#define AST_USBHUB_IRQ			5

/* registers of UDC */
#define AST_USBHUB_ROOT				0x00
#define AST_USBHUB_HUB_ADDR			0x04
#define AST_USBHUB_ICR				0x08
#define AST_USBHUB_ISR				0x0C
#define AST_USBHUB_EP_ACK_ICR		0x10
#define AST_USBHUB_EP_NAK_ICR		0x14
#define AST_USBHUB_EP_ACK_ISR		0x18
#define AST_USBHUB_EP_NAK_ISR		0x1C
#define AST_USBHUB_DEV_RESET		0x20
#define AST_USBHUB_USB_STATUS		0x24
#define AST_USBHUB_EP_TOGGLE		0x28
#define AST_USBHUB_HUB_EP0_CSR		0x30
#define AST_USBHUB_HUB_EP0_BUF		0x34
#define AST_USBHUB_HUB_EP1_CSR		0x38
#define AST_USBHUB_HUB_EP1_MAP		0x3C

#define AST_USBHUB_HUB_SETUP_BUF	0x80

/* bits of root control and status register */
#define AST_USBHUB_ROOT_PHY_CLK						0x80000000 /* bit 31 */
#define AST_USBHUB_ROOT_COMPLETE_SPLIT_IN_TRANS		0x00010000 /* bit 16 */
#define AST_USBHUB_ROOT_LOOP_BACK_RESULT			0x00008000 /* bit 15 */
#define AST_USBHUB_ROOT_LOOP_BACK_DONE				0x00004000 /* bit 14 */
#define AST_USBHUB_ROOT_USB_PHY_BIST_STATUS			0x00002000 /* bit 13 */
#define AST_USBHUB_ROOT_USB_PHY_BIST_CTRL			0x00001000 /* bit 12 */
#define AST_USBHUB_ROOT_USB_PHY_RESET				0x00000800 /* bit 11 */
#define AST_USBHUB_ROOT_TEST_MODE					0x00000100 /* bit 8 */
#define AST_USBHUB_ROOT_REMOTE_WAKE_WIDTH			0x00000020 /* bit 5 */
#define AST_USBHUB_ROOT_MANUAL_REMOTE_WAKEUP		0x00000010 /* bit 4 */
#define AST_USBHUB_ROOT_AUTO_REMOTE_WAKEUP			0x00000008 /* bit 3 */
#define AST_USBHUB_ROOT_SUSPEND_CLK_STOP			0x00000004 /* bit 2 */
#define AST_USBHUB_ROOT_UPSTREAM_PORT_SPEED			0x00000002 /* bit 1 */
#define AST_USBHUB_ROOT_UPSTREAM_PORT_CONNECT		0x00000001 /* bit 0 */

/* bits of root control and status register */
#define AST_USBHUB_USB_STATUS_BUS_SPEED				0x08000000 /* bit 27 */
#define AST_USBHUB_USB_STATUS_BUS_HIGH				0x08000000
#define AST_USBHUB_USB_STATUS_BUS_LOW				0x00000000

/* bits of interrupt control register */
#define AST_USBHUB_IRQ_DEAD_LOCK					0x00040000 /* bit 18 */
#define AST_USBHUB_IRQ_EP_POOL_NAK					0x00020000 /* bit 17 */
#define AST_USBHUB_IRQ_EP_POOL_ACK					0x00010000 /* bit 16 */
#define AST_USBHUB_IRQ_DEV_MASK						0x0000FE00 /* bits[15:9] */
#define AST_USBHUB_IRQ_DEV_BASE						0x00000200 /* bit 9 */
#define AST_USBHUB_IRQ_RESUME						0x00000100 /* bit 8 */
#define AST_USBHUB_IRQ_SUSPEND						0x00000080 /* bit 7 */
#define AST_USBHUB_IRQ_BUS_RESET					0x00000040 /* bit 6 */
#define AST_USBHUB_IRQ_EP1_IN_ACK					0x00000020 /* bit 5 */
#define AST_USBHUB_IRQ_EP0_IN_NAK					0x00000010 /* bit 4 */
#define AST_USBHUB_IRQ_EP0_IN_ACK					0x00000008 /* bit 3 */
#define AST_USBHUB_IRQ_EP0_OUT_NAK					0x00000004 /* bit 2 */
#define AST_USBHUB_IRQ_EP0_OUT_ACK					0x00000002 /* bit 1 */
#define AST_USBHUB_IRQ_EP0_SETUP_ACK				0x00000001 /* bit 0 */

/* bit of device software reset register */
#define AST_USBHUB_DEV_RESET_DEV_BASE				0x00000001 /* bit 0 */

/* bits of endpoint pool dtata toggle value regster */
#define AST_USBHUB_EP_TOGGLE_DATA0					0x00000000 /* bit 8 */
#define AST_USBHUB_EP_TOGGLE_DATA1					0x00000100 /* bit 8 */
#define AST_USBHUB_EP_TOGGLE_INDEX_SHIFT			0
#define AST_USBHUB_EP_TOGGLE_INDEX_MASK				0x0000001F /* bits[4:0] */

/* bit of endpoint pool ACK/NAK interrupt control/status register */
#define AST_USBHUB_EP_IRQ_SHIFT						0x00000001 /* bit 0 */

/* bits of hub ep0 control and status register */
#define AST_USBHUB_HUB_EP0_OUT_BUF_LEN_MASK			0x007F0000 /* bits[22:16] */
#define AST_USBHUB_HUB_EP0_OUT_BUF_LEN_SHIFT		16
#define AST_USBHUB_HUB_EP0_IN_BUF_LEN_MASK			0x00007F00 /* bits[14:8] */
#define AST_USBHUB_HUB_EP0_IN_BUF_LEN_SHIFT			8
#define AST_USBHUB_HUB_EP0_OUT_BUF_RX				0x00000004 /* bit 2 */
#define AST_USBHUB_HUB_EP0_IN_BUF_TX				0x00000002 /* bit 1 */
#define AST_USBHUB_HUB_EP0_STALL					0x00000001 /* bit 0 */

/* bits of hub ep1 control and status register */
#define AST_USBHUB_HUB_EP1_RESET_TOGGLE				0x00000004 /* bit 2 */
#define AST_USBHUB_HUB_EP1_STALL					0x00000002 /* bit 1 */
#define AST_USBHUB_HUB_EP1_CTRL						0x00000001 /* bit 0 */

/* bit of hub ep1 map register */
#define AST_USBHUB_HUB_EP1_MAP_DEV_SHIFT			0x00000002 /* bit 1 */

#define AST_USBHUB_EP0_BUF_SIZE 64

/*-----------------------------------------------------------------------------------------------*/
/* hardware information about UDC device */

#if defined(SOC_AST2050) || defined(SOC_AST2150)
#define AST_USBHUB_DEV_NUM			7
#else
#define AST_USBHUB_DEV_NUM			5
#endif

/* registers of UDC device */
#define AST_USBHUB_DEV_REG_OFFSET			0x100
#define AST_USBHUB_DEV_REG_SIZE				0x10
#define AST_USBHUB_DEV_REG_BASE(p) 			(AST_USBHUB_DEV_REG_OFFSET + (p * AST_USBHUB_DEV_REG_SIZE))

#define AST_USBHUB_DEV_EP0_ICR(p)			(AST_USBHUB_DEV_REG_BASE(p) + 0x00)
#define AST_USBHUB_DEV_EP0_ISR(p)			(AST_USBHUB_DEV_REG_BASE(p) + 0x04)
#define AST_USBHUB_DEV_EP0_CSR(p)			(AST_USBHUB_DEV_REG_BASE(p) + 0x08)
#define AST_USBHUB_DEV_EP0_BUF(p)			(AST_USBHUB_DEV_REG_BASE(p) + 0x0C)

#define AST_USBHUB_DEV_SETUP_BUF_OFFSET		0x88
#define AST_USBHUB_DEV_SETUP_BUF_SIZE		0x08
#define AST_USBHUB_DEV_SETUP_BUF(p)			(AST_USBHUB_DEV_SETUP_BUF_OFFSET + (p * AST_USBHUB_DEV_SETUP_BUF_SIZE))

/* bits of interrupt control register */
#define AST_USBHUB_DEV_ICR_ADDR_MASK				0x00007F00 /* bits[14:8] */
#define AST_USBHUB_DEV_ICR_ADDR_SHIFT				8
#define AST_USBHUB_DEV_ICR_EP0_IN_NAK				0x00000040 /* bit 6 */
#define AST_USBHUB_DEV_ICR_EP0_IN_ACK				0x00000020 /* bit 5 */
#define AST_USBHUB_DEV_ICR_EP0_OUT_NAK				0x00000010 /* bit 4 */
#define AST_USBHUB_DEV_ICR_EP0_OUT_ACK				0x00000008 /* bit 3 */
#define AST_USBHUB_DEV_ICR_EP0_SETUP_ACK			0x00000004 /* bit 2 */
#define AST_USBHUB_DEV_ICR_SPEED					0x00000002 /* bit 1 */
#define AST_USBHUB_DEV_ICR_ENABLE					0x00000001 /* bit 0 */

/* bits of interrupt status register */
#define AST_USBHUB_DEV_ISR_EP0_IRQ_MASK				0x0000001F /* bits[4:0] */
#define AST_USBHUB_DEV_ISR_EP0_IN_NAK				0x00000010 /* bit 4 */
#define AST_USBHUB_DEV_ISR_EP0_IN_ACK				0x00000008 /* bit 3 */
#define AST_USBHUB_DEV_ISR_EP0_OUT_NAK				0x00000004 /* bit 2 */
#define AST_USBHUB_DEV_ISR_EP0_OUT_ACK				0x00000002 /* bit 1 */
#define AST_USBHUB_DEV_ISR_EP0_SETUP_ACK			0x00000001 /* bit 0 */

/* bits of device ep0 control and status register */
#define AST_USBHUB_DEV_EP0_OUT_BUF_LEN_MASK			0x007F0000 /* bits[22:16] */
#define AST_USBHUB_DEV_EP0_OUT_BUF_LEN_SHIFT		16
#define AST_USBHUB_DEV_EP0_IN_BUF_LEN_MASK			0x00007F00 /* bits[14:8] */
#define AST_USBHUB_DEV_EP0_IN_BUF_LEN_SHIFT			8
#define AST_USBHUB_DEV_EP0_OUT_BUF_RX				0x00000004 /* bit 2 */
#define AST_USBHUB_DEV_EP0_IN_BUF_TX				0x00000002 /* bit 1 */
#define AST_USBHUB_DEV_EP0_STALL					0x00000001 /* bit 0 */

/*-----------------------------------------------------------------------------------------------*/
/* hardware information about UDC endpoint */

#define AST_USBHUB_EP_NUM			21
#define AST_USBHUB_DEV_EP_NUM		3 /* endpoint for each device */

/* registers of UDC endpoint */
#define AST_USBHUB_EP_REG_OFFSET		0x200
#define AST_USBHUB_EP_REG_SIZE			0x10
#define AST_USBHUB_EP_REG_BASE(n) 		(AST_USBHUB_EP_REG_OFFSET + (n * AST_USBHUB_EP_REG_SIZE))

#define AST_USBHUB_EP_CONF(p)			(AST_USBHUB_EP_REG_BASE(p) + 0x00)
#define AST_USBHUB_EP_DESC_CSR(p)		(AST_USBHUB_EP_REG_BASE(p) + 0x04)
#define AST_USBHUB_EP_DESC_ADDR(p)		(AST_USBHUB_EP_REG_BASE(p) + 0x08)
#define AST_USBHUB_EP_DESC_PTR(p)		(AST_USBHUB_EP_REG_BASE(p) + 0x0C)

/* bits of configuration register */
#define AST_USBHUB_EP_CONF_DISABLE_TOGGLE		0x00002000 /* bit 13 */
#define AST_USBHUB_EP_CONF_STALL				0x00001000 /* bit 12 */
#define AST_USBHUB_EP_CONF_NUM_MASK				0x00000F00 /* bits[11:8] */
#define AST_USBHUB_EP_CONF_NUM_SHIFT			8
#define AST_USBHUB_EP_CONF_TYPE_MASK			0x00000070 /* bits[6:4] */
#define AST_USBHUB_EP_CONF_TYPE_SHIFT			4
#define AST_USBHUB_EP_CONF_PORT_MASK			0x0000000E /* bits[3:1] */
#define AST_USBHUB_EP_CONF_PORT_SHIFT			1
#define AST_USBHUB_EP_CONF_ENABLB				0x00000001 /* bit 0 */

/* bits of descriptor list control/status register */
#define AST_USBHUB_EP_DESC_CSR_RESET			0x00000004 /* bit 2 */
#define AST_USBHUB_EP_DESC_CSR_SINGLE			0x00000002 /* bit 1 */
#define AST_USBHUB_EP_DESC_CSR_RING				0x00000001 /* bit 0 */

/* bits of descriptor list read/write pointer register */
#define AST_USBHUB_EP_DESC_PTR_EMPTY			0x80000000 /* bit 31 */
#define AST_USBHUB_EP_DESC_PTR_TOGGLE_MASK		0x30000000 /* bits[29:28] */
#define AST_USBHUB_EP_DESC_PTR_TOGGLE_SHITF		28
#define AST_USBHUB_EP_DESC_PTR_SIZE_MASK		0x07FF0000 /* bits[26:16] */
#define AST_USBHUB_EP_DESC_PTR_SIZE_SHITF		16
#define AST_USBHUB_EP_DESC_PTR_HW_READ_MASK		0x00001F00 /* bits[12:8] */
#define AST_USBHUB_EP_DESC_PTR_HW_READ_SHIFT	8
#define AST_USBHUB_EP_DESC_PTR_SW_WRITE_MASK	0x0000001F /* bits[4:0] */
#define AST_USBHUB_EP_DESC_PTR_SW_WRITE_SHITF	0
#define AST_USBHUB_EP_DESC_PTR_SW_WRITE_SINGLE	0x00000001

/* types of endpoint */
#define AST_USBHUB_EP_TYPE_BULK_IN		0x02
#define AST_USBHUB_EP_TYPE_BULK_OUT		0x03
#define AST_USBHUB_EP_TYPE_INTR_IN		0x04
#define AST_USBHUB_EP_TYPE_INTR_OUT		0x05
#define AST_USBHUB_EP_TYPE_ISOC_IN		0x06
#define AST_USBHUB_EP_TYPE_ISOC_OUT		0x07

#define AST_USBHUB_EP_TYPE_DIR_MASK		0x01
#define AST_USBHUB_EP_TYPE_DIR_OUT		0x01
#define AST_USBHUB_EP_TYPE_DIR_IN		0x00

/*-----------------------------------------------------------------------------------------------*/
/* DMA descriptor of UDC endpoint */

#define AST_USBHUB_EP_DESC_NUM		32

struct ast_usbhub_ep_buf_desc {
	u32 addr;
	u32 status;
};

/* bits of status field of descriptor */
#define AST_USBHUB_EP_DMA_DESC_IRQ					0x80000000 /* bit 31 */
#define AST_USBHUB_EP_DMA_DESC_PORT_MASK			0x70000000 /* bits[30:28] */
#define AST_USBHUB_EP_DMA_DESC_PORT_SHIFT			28
#define AST_USBHUB_EP_DMA_DESC_EP_MASK				0x0F000000 /* bits[27:24] */
#define AST_USBHUB_EP_DMA_DESC_EP_SHIFT				24
#define AST_USBHUB_EP_DMA_DESC_ADDR_MASK			0x007F0000 /* bits[22:16] */
#define AST_USBHUB_EP_DMA_DESC_ADDR_SHIFT			16
#define AST_USBHUB_EP_DMA_DESC_PID_MASK				0x0000C000 /* bits[15:14] */
#define AST_USBHUB_EP_DMA_DESC_PACKET_END			0x00002000 /* bit 13 */
#define AST_USBHUB_EP_DMA_DESC_OUT_VALID			0x00000800 /* bit 11 */
#define AST_USBHUB_EP_DMA_DESC_LEN_MASK				0x000007FF /* bits[10:0] */

#define AST_USBHUB_DEV_EP0_BUF_SIZE 64
#define AST_USBHUB_EP_BUF_SIZE 1024

enum ep0_ctrl_data_stage_e {
	EP0_CTRL_DATA_STAGE_NONE, /* SETUP -> IN */
	EP0_CTRL_DATA_STAGE_IN, /* SETUP -> IN -> OUT */
	EP0_CTRL_DATA_STAGE_OUT /* SETUP -> OUT -> IN */
};

struct ast_usbhub_dev_cfg_t {
	unsigned int dev_num;
};

#endif /* !__AST_USBHUB_H__ */
