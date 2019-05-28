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
 * ast_usbhub.c
 * ASPEED AST2100/2050/2200/2150/2300/2400
 * virtual USB hub controller driver
 *
 ****************************************************************/

#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/dmapool.h>
#include <linux/dma-mapping.h>
#include <linux/sysctl.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/cacheflush.h>
#include <asm/unaligned.h>

#include <linux/usb/ch9.h>

#include "coreusb.h"
#include "usb_core.h"
#include "usb_hw.h"
#include "mod_reg.h"

#include "helper.h"
#include "dbgout.h"

#include "ast_usbhub.h"
#include "hub.h"

#define AST_USBHUB_DEBUG_IRQ		0x01
#define AST_USBHUB_DEBUG_SETUP		0x02
#define AST_USBHUB_DEBUG_CALL		0x04

#define AST_USBHUB_USE_DEV_NUM 4

/* There are 2 endpoint 0 in AST virtual hub: ep 0 for each device and ep 0 in ep pool.
   In order to distinguish them, ep0 for device is represented as AST_USBHUB_DEV_EP0_ID internally */
#define AST_USBHUB_DEV_EP0_ID 0x80

/*-----------------------------------------------------------------------------------------------*/

static const char *ast_usbhub_driver_name = "ast-usbhub";

static void *ast_usbhub_virt_base; /* virtual address of I/O registers */

static uint8_t *ast_usbhub_hub_ep0_buf; /* buffer for hub ep0 setup IN/OUT */
static dma_addr_t ast_usbhub_hub_ep0_buf_dma; /* DMA mapping of hub ep0 buffer */

static enum ep0_ctrl_data_stage_e ast_usbhub_hub_ctrl_data_stage;
static uint8_t saved_hub_ctrl_req; /* saved hub ep0 setup request */

static uint16_t ast_usbhub_hub_dev_addr; /* USB device address of hub */
static uint16_t ast_usbhub_hub_current_config_value; /* configuration value of hub */
static enum usb_device_speed ast_usbhub_hub_speed; /* current speed of hub */

/* for GET STATUS, CLEAR FEATURE, SET FEATURE */
static uint8_t ast_usbhub_hub_usb_status;
static struct usb_hub_status ast_usbhub_hub_class_status;
static struct usb_port_status ast_usbhub_port_status[AST_USBHUB_DEV_NUM];

static int ast_usbhub_device_connect_to_port[AST_USBHUB_DEV_NUM];
static int ast_usbhub_device_enabled[AST_USBHUB_DEV_NUM];
static int ast_usbhub_dev_tx_ready[AST_USBHUB_DEV_NUM];

static unsigned int ast_usbhub_remote_wakeup_enable;
#ifdef CONFIG_SPX_FEATURE_POWER_CONSUMPTION_SUPPORT
int IsUsb_Upstream_host = 0;
static void ast_usbhub_clear_and_disconnect(void);
static void ast_usbhub_init_and_connect(void);
#endif
/* for debug message control */
TDBG_DECLARE_DBGVAR(ast_usbhub_debug_flags);
static struct ctl_table_header *ast_usbhub_sys = NULL;
static struct ctl_table ast_usbhub_ctl_table[] = {
	{
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,11))
		.ctl_name = CTL_UNNUMBERED,
#endif
		.procname = "debug_flags",
		.data = &(TDBG_FORM_VAR_NAME(ast_usbhub_debug_flags)),
		.maxlen = sizeof(int),
		.mode = 0644,
		.child = NULL,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,11))
		.parent = NULL,
#endif
		.proc_handler = &proc_dointvec
	}, {
		0
	}
};

/*-----------------------------------------------------------------------------------------------*/

static uint8_t ast_usbhub_dev_addr[AST_USBHUB_DEV_NUM]; /* USB device address of device */
static int ast_usbhub_dev_set_addr_req[AST_USBHUB_DEV_NUM];
static enum usb_device_speed ast_usbhub_dev_speed[AST_USBHUB_DEV_NUM]; /* USB device speed of device */
static enum usb_device_speed ast_usbhub_dev_run_speed[AST_USBHUB_DEV_NUM]; /* USB device speed of device */

void *ast_usbhub_dev_ep0_buf_addr[AST_USBHUB_DEV_NUM]; /* buffer for endpoint 0 data read/write */
dma_addr_t ast_usbhub_dev_ep0_buf_dma[AST_USBHUB_DEV_NUM]; /* DMA mapping of endpoint 0 buffer */

void *ast_usbhub_ep_buf_addr[AST_USBHUB_EP_NUM]; /* buffer for endpoint data read/write */
dma_addr_t ast_usbhub_ep_buf_dma[AST_USBHUB_EP_NUM]; /* DMA mapping of buffer */

static USB_CORE usb_core_module;

/*-----------------------------------------------------------------------------------------------*/

/* ASTHUB Device Configuration Struture */
static struct ast_usbhub_dev_cfg_t ast_usbhub_dev_cfg[AST_USBHUB_DEV_NUM];
static usb_ctrl_driver ast_usbhub_driver[AST_USBHUB_DEV_NUM];

/*-----------------------------------------------------------------------------------------------*/

/* Descriptors of Hub, most are static, but some fields will be modified depend on speed. */

#define AST_USBHUB_USB_VENDOR_ID		0x046B	/* American Megatrends Inc. */
#define AST_USBHUB_PRODUCT_ID_HUB		0xFF01

struct usb_string {
	uint8_t id;
	char *s;
};

#define STRING_LANGUAGE_ID		0x0409 /* English(US) */

/* index of string descriptors */
#define STRING_MANUFACTURER		1
#define STRING_PRODUCT			2
#define STRING_SERIAL			3
#define STRING_CONFIG			4
#define STRING_INTERFACE		5

/* Static strings, in UTF-8 (for simplicity we use only ASCII characters) */
static struct usb_string ast_usbhub_hub_desc_strings[] = {
	{STRING_MANUFACTURER, "American Megatrends Inc."},
	{STRING_PRODUCT, "Virtual Hub"}, /* depend on device type, adjusted during setup */
	{STRING_SERIAL, "serial"},  /* depend on device id, adjusted during setup */
	{STRING_CONFIG, "Self-powered"},
	{STRING_INTERFACE, "7-port Hub"},
	{0, NULL}
};

static struct usb_device_descriptor ast_usbhub_hub_dev_desc = {
	.bLength = USB_DT_DEVICE_SIZE,
	.bDescriptorType = USB_DT_DEVICE,
	.bcdUSB = __constant_cpu_to_le16(0x0200), /* USB Specification 2.0 */

	.bDeviceClass = USB_CLASS_HUB,
	.bDeviceSubClass = 0,
	.bDeviceProtocol = 1,

	.bMaxPacketSize0 = 64, /* ep0 max packet size */

	.idVendor = __constant_cpu_to_le16(AST_USBHUB_USB_VENDOR_ID),
	.idProduct = __constant_cpu_to_le16(AST_USBHUB_PRODUCT_ID_HUB), /* Virtual Hub */
	.bcdDevice = __constant_cpu_to_le16(0x0100),

	.iManufacturer = STRING_MANUFACTURER,
	.iProduct = STRING_PRODUCT,
	.iSerialNumber = STRING_SERIAL,
	.bNumConfigurations = 1 /* we have only 1 configuration */
};

#define AST_USBHUB_HUB_CONFIG_VALUE		1

/* There is only one configuration */
static struct usb_config_descriptor ast_usbhub_hub_config_desc = {
	.bLength = USB_DT_CONFIG_SIZE,
	.bDescriptorType = USB_DT_CONFIG,

	/* wTotalLength computed by usb_gadget_config_buf() */
	.bNumInterfaces = 1, /* we have only 1 interface */
	.bConfigurationValue = AST_USBHUB_HUB_CONFIG_VALUE,
	.iConfiguration = STRING_CONFIG,
	.bmAttributes = USB_CONFIG_ATT_ONE | USB_CONFIG_ATT_SELFPOWER | USB_CONFIG_ATT_WAKEUP,
	.bMaxPower = 50 /* 100 mA */
};

/* There is only one interface. */
static struct usb_interface_descriptor ast_usbhub_hub_intf_desc = {
	.bLength = USB_DT_INTERFACE_SIZE,
	.bDescriptorType = USB_DT_INTERFACE,
	.bInterfaceNumber = 0,
	.bAlternateSetting = 0,
	.bNumEndpoints = 1, /* status change ep(Interrupt In) */

	.bInterfaceClass = USB_CLASS_HUB,
	.bInterfaceSubClass = 0,
	.bInterfaceProtocol = 0,

	.iInterface = STRING_INTERFACE,
};

/* Full-speed endpoint descriptors for status change endpoint of hub */
static struct usb_endpoint_descriptor ast_usbhub_hub_fs_intr_in_ep_desc = {
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = USB_DIR_IN | 0x01,
	.bmAttributes = USB_ENDPOINT_XFER_INT,
	.wMaxPacketSize = 0x01, /* 1 byte */
	.bInterval = 0xFF
};

static const struct usb_descriptor_header *ast_usbhub_hub_fs_function[] = {
	(struct usb_descriptor_header *) &ast_usbhub_hub_config_desc,
	(struct usb_descriptor_header *) &ast_usbhub_hub_intf_desc,
	(struct usb_descriptor_header *) &ast_usbhub_hub_fs_intr_in_ep_desc,
	NULL,
};

/*
 * USB 2.0 devices need to expose both high speed and full speed
 * descriptors, unless they only run at full speed.
 *
 * That means alternate endpoint descriptors (bigger packets)
 * and a "device qualifier" ... plus more construction options
 * for the config descriptor.
 */
static struct usb_qualifier_descriptor ast_usbhub_hub_dev_qualifier_desc = {
	.bLength = sizeof(ast_usbhub_hub_dev_qualifier_desc),
	.bDescriptorType = USB_DT_DEVICE_QUALIFIER,
	.bcdUSB = __constant_cpu_to_le16(0x0200), /* USB Specification 2.0 */

	.bDeviceClass = USB_CLASS_HUB,
	.bDeviceSubClass = 0,
	.bDeviceProtocol = 0,

	.bMaxPacketSize0 = 64, /* ep0 max packet size */

	.bNumConfigurations = 1,

	.bRESERVED = 0
};

/* High-speed endpoint descriptors for status change endpoint of hub */
static struct usb_endpoint_descriptor ast_usbhub_hub_hs_intr_in_ep_desc = {
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = USB_DIR_IN | 0x01,
	.bmAttributes = USB_ENDPOINT_XFER_INT,
	.wMaxPacketSize = 0x01, /* 1 byte */
	.bInterval = 0x0C /* 12 frames */
};

static const struct usb_descriptor_header *ast_usbhub_hub_hs_function[] = {
	(struct usb_descriptor_header *) &ast_usbhub_hub_config_desc,
	(struct usb_descriptor_header *) &ast_usbhub_hub_intf_desc,
	(struct usb_descriptor_header *) &ast_usbhub_hub_hs_intr_in_ep_desc,
	NULL,
};

struct usb_hub_descriptor ast_usbhub_hub_class_desc = {
	.bDescLength = USB_DT_HUB_SIZE,
	.bDescriptorType = USB_DT_HUB,
	.bNbrPorts = AST_USBHUB_DEV_NUM,
	.wHubCharacteristics = HUB_CHAR_LPSM_IDV | HUB_CHAR_TTTT_32 | HUB_CHAR_OCPM_IDV,
	.bPwrOn2PwrGood = 0x32, /* 100 ms */
	.bHubContrCurrent = 100, /* 100 mA */

	.DeviceRemovable = 0x00,
	.PortPwrCtrlMask = 0xFF /* all bit should be 1 for compatibility with USB 1.0 */
};

/*-----------------------------------------------------------------------------------------------*/

inline uint32_t ast_usbhub_read_reg(uint32_t reg)
{
	return ioread32((void * __iomem)ast_usbhub_virt_base + reg);
}

inline void ast_usbhub_write_reg(uint32_t data, uint32_t reg)
{
	iowrite32(data, (void * __iomem)ast_usbhub_virt_base + reg);
}

static int ast_usbhub_convert_dev_to_port(uint8_t dev_num)
{
	int i;

	for (i = 0; i < AST_USBHUB_DEV_NUM; i ++)  {
		if (ast_usbhub_dev_cfg[i].dev_num == dev_num)
			return i;
	}

	printk(KERN_ERR "%s: convert dev num to port failed for dev %d\n", ast_usbhub_driver_name, dev_num);
	return -1;
}

inline static uint8_t ast_usbhub_convert_ep_num(uint8_t dev_num, uint8_t ep_num)
{
	return (ep_num == 0) ? AST_USBHUB_DEV_EP0_ID : (dev_num * AST_USBHUB_DEV_EP_NUM) + ep_num - 1;
}

inline static uint8_t ast_usbhub_invert_ep_num(uint8_t ep_num)
{
	return (ep_num == AST_USBHUB_DEV_EP0_ID) ? 0 : (ep_num % AST_USBHUB_DEV_EP_NUM) + 1;
}

static void put_value_to_bytes_le32(uint8_t *bytes, uint32_t val)
{
	bytes[0] = val & 0x000000FF;
	bytes[1] = (val & 0x0000FF00) >> 8;
	bytes[2] = (val & 0x00FF0000) >> 16;
	bytes[3] = (val & 0xFF000000) >> 24;
}

#if 0
static void put_value_to_bytes_be32(uint8_t *bytes, uint32_t val)
{
	bytes[0] = (val & 0xFF000000) >> 24;
	bytes[1] = (val & 0x00FF0000) >> 16;
	bytes[2] = (val & 0x0000FF00) >> 8;
	bytes[3] = val & 0x000000FF;
}
#endif

/*-----------------------------------------------------------------------------------------------*/

static void ast_usbhub_set_bitmap(uint8_t port)
{
	uint32_t reg;

	reg = ast_usbhub_read_reg(AST_USBHUB_HUB_EP1_MAP);
	reg |= (AST_USBHUB_HUB_EP1_MAP_DEV_SHIFT << port);
	ast_usbhub_write_reg(reg, AST_USBHUB_HUB_EP1_MAP);
}

static void ast_usbhub_clear_bitmap(uint8_t port)
{
	uint32_t reg;

	reg = ast_usbhub_read_reg(AST_USBHUB_HUB_EP1_MAP);
	reg &= ~(AST_USBHUB_HUB_EP1_MAP_DEV_SHIFT << port);
	ast_usbhub_write_reg(reg, AST_USBHUB_HUB_EP1_MAP);
}

static void ast_usbhub_reset_dev(uint8_t dev_num)
{
	uint32_t reg;

	/* start the reset process */
	reg = ast_usbhub_read_reg(AST_USBHUB_DEV_RESET);
	reg |= AST_USBHUB_DEV_RESET_DEV_BASE << (dev_num + 1);
	ast_usbhub_write_reg(reg, AST_USBHUB_DEV_RESET);

	/* stop the reset process */
	reg = ast_usbhub_read_reg(AST_USBHUB_DEV_RESET);
	reg &= ~(AST_USBHUB_DEV_RESET_DEV_BASE << (dev_num + 1));
	ast_usbhub_write_reg(reg, AST_USBHUB_DEV_RESET);
}

/*-----------------------------------------------------------------------------------------------*/

static ep_config_t ast_usbhub_ep_configs[AST_USBHUB_DEV_EP_NUM] = {
	{
		.ep_num = 1,
		.ep_dir = CONFIGURABLE,
		.ep_size = AST_USBHUB_EP_BUF_SIZE,
		.ep_type = CONFIGURABLE,
		.is_ep_used = 0
	}, {
		.ep_num = 2,
		.ep_dir = CONFIGURABLE,
		.ep_size = AST_USBHUB_EP_BUF_SIZE,
		.ep_type = CONFIGURABLE,
		.is_ep_used = 0
	}, {
		.ep_num = 3,
		.ep_dir = CONFIGURABLE,
		.ep_size = AST_USBHUB_EP_BUF_SIZE,
		.ep_type = CONFIGURABLE,
		.is_ep_used = 0
	}
};

static void ast_usbhub_alloc_dma_buf(void)
{
	int i;

	for (i = 0; i < AST_USBHUB_DEV_NUM; i ++) {
		/* allocate DMA buffer for device endpoint 0 */
		ast_usbhub_dev_ep0_buf_addr[i] = dma_alloc_coherent(NULL, AST_USBHUB_DEV_EP0_BUF_SIZE, &(ast_usbhub_dev_ep0_buf_dma[i]), GFP_KERNEL);
	}

	for (i = 0; i < AST_USBHUB_EP_NUM; i ++) {
		/* allocate DMA buffer for endpoint */
		ast_usbhub_ep_buf_addr[i] = dma_alloc_coherent(NULL, AST_USBHUB_EP_BUF_SIZE, &(ast_usbhub_ep_buf_dma[i]), GFP_KERNEL);
	}
}

static void ast_usbhub_free_dma_buf(void)
{
	int i;

	for (i = 0; i < AST_USBHUB_DEV_NUM; i ++) {
		/* free DMA buffer for device endpoint 0 */
		dma_free_coherent(NULL, AST_USBHUB_DEV_EP0_BUF_SIZE, ast_usbhub_dev_ep0_buf_addr[i], ast_usbhub_dev_ep0_buf_dma[i]);
	}

	for (i = 0; i < AST_USBHUB_DEV_EP_NUM; i ++) {
		/* free DMA buffer for endpoint */
		dma_free_coherent(NULL, AST_USBHUB_EP_BUF_SIZE, ast_usbhub_ep_buf_addr[i], ast_usbhub_ep_buf_dma[i]);
	}
}

static void ast_usbhub_configure_dev(uint8_t dev_num)
{
	if (ast_usbhub_hub_speed == USB_SPEED_HIGH) {
		ast_usbhub_dev_run_speed[dev_num] = ast_usbhub_dev_speed[dev_num];
		if (ast_usbhub_dev_speed[dev_num] == SUPPORT_LOW_SPEED) {
			usb_core_module.CoreUsbConfigureLS(ast_usbhub_dev_cfg[dev_num].dev_num);
		} else if (ast_usbhub_dev_speed[dev_num] == SUPPORT_FULL_SPEED) {
			usb_core_module.CoreUsbConfigureFS(ast_usbhub_dev_cfg[dev_num].dev_num);
		} else {
			usb_core_module.CoreUsbConfigureHS(ast_usbhub_dev_cfg[dev_num].dev_num);
		}
	} else { /* USB_SPEED_FULL */
		#if defined(SOC_AST2050) || defined(SOC_AST2150)
		/* According to AST2100 spec, virtual hub controller does not support PRE packet,
		   when upstream port is full speed, the downstream ports can not be low speed */
		ast_usbhub_dev_run_speed[dev_num] = SUPPORT_FULL_SPEED;
		usb_core_module.CoreUsbConfigureFS(ast_usbhub_dev_cfg[dev_num].dev_num);
		#else
		if (ast_usbhub_dev_speed[dev_num] == SUPPORT_LOW_SPEED) {
			ast_usbhub_dev_run_speed[dev_num] = SUPPORT_LOW_SPEED;
			usb_core_module.CoreUsbConfigureLS(ast_usbhub_dev_cfg[dev_num].dev_num);
		} else {
			ast_usbhub_dev_run_speed[dev_num] = SUPPORT_FULL_SPEED;
			usb_core_module.CoreUsbConfigureFS(ast_usbhub_dev_cfg[dev_num].dev_num);
		}
		#endif
	}
}

void ast_usbhub_enable_dev(uint8_t dev_num, uint8_t speed)
{
	uint32_t reg;

	TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_CALL, "call en dev %d\n", dev_num);

	dev_num = ast_usbhub_convert_dev_to_port(dev_num);

	/* hardware works with bus(physical) address */
	ast_usbhub_write_reg(ast_usbhub_dev_ep0_buf_dma[dev_num], AST_USBHUB_DEV_EP0_BUF(dev_num));

	/* clear device global interrupt */
	reg = ast_usbhub_read_reg(AST_USBHUB_ISR);
	reg |= AST_USBHUB_IRQ_DEV_BASE << dev_num;
	ast_usbhub_write_reg(reg, AST_USBHUB_ISR);

	/* enable device global interrupt */
	reg = ast_usbhub_read_reg(AST_USBHUB_ICR);
	reg |= (AST_USBHUB_IRQ_DEV_BASE << dev_num);
	ast_usbhub_write_reg(reg, AST_USBHUB_ICR);

	/* claer interrupt status */
	ast_usbhub_write_reg(AST_USBHUB_DEV_ISR_EP0_IRQ_MASK, AST_USBHUB_DEV_EP0_ISR(dev_num));

	/* enable interrupt */
	reg = AST_USBHUB_DEV_ICR_EP0_SETUP_ACK | AST_USBHUB_DEV_ICR_EP0_IN_ACK | AST_USBHUB_DEV_ICR_EP0_IN_NAK | AST_USBHUB_DEV_ICR_EP0_OUT_ACK | AST_USBHUB_DEV_ICR_EP0_OUT_NAK;
	if (speed == SUPPORT_HIGH_SPEED)
		reg |= AST_USBHUB_DEV_ICR_SPEED;
	ast_usbhub_write_reg(reg, AST_USBHUB_DEV_EP0_ICR(dev_num));

	ast_usbhub_device_enabled[dev_num] = 1;
	ast_usbhub_dev_speed[dev_num] = speed;
	ast_usbhub_configure_dev(dev_num);
}

void ast_usbhub_disable_dev(uint8_t dev_num, uint8_t speed)
{
	uint32_t reg;

	TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_CALL, "call dis dev %d\n", dev_num);

	dev_num = ast_usbhub_convert_dev_to_port(dev_num);

	/* disable device and clear address */
	reg = ast_usbhub_read_reg(AST_USBHUB_DEV_EP0_ICR(dev_num)) & ~(AST_USBHUB_DEV_ICR_ADDR_MASK | AST_USBHUB_DEV_ICR_ENABLE);
	ast_usbhub_write_reg(reg, AST_USBHUB_DEV_EP0_ICR(dev_num));

	/* disable device interrupt */
	ast_usbhub_write_reg(0, AST_USBHUB_DEV_EP0_ICR(dev_num));

	/* clear device interrupt status */
	ast_usbhub_write_reg(AST_USBHUB_DEV_ISR_EP0_IRQ_MASK, AST_USBHUB_DEV_EP0_ISR(dev_num));

	/* disable device global interrupt */
	reg = ast_usbhub_read_reg(AST_USBHUB_ICR);
	reg &= ~(AST_USBHUB_IRQ_DEV_BASE << dev_num);
	ast_usbhub_write_reg(reg, AST_USBHUB_ICR);

	/* clear device global interrupt */
	reg = ast_usbhub_read_reg(AST_USBHUB_ISR);
	reg |= AST_USBHUB_IRQ_DEV_BASE << dev_num;
	ast_usbhub_write_reg(reg, AST_USBHUB_ISR);

	ast_usbhub_device_enabled[dev_num] = 0;
}

void ast_usbhub_dev_disconnect(uint8_t dev_num)
{
	uint32_t reg;

	TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_CALL, "call disconn dev %d\n", dev_num);

	ast_usbhub_disable_dev(dev_num, 0);

	dev_num = ast_usbhub_convert_dev_to_port(dev_num);

	if (!ast_usbhub_device_connect_to_port[dev_num]) {
		printk(KERN_WARNING "dev %d does not connect to host\n", dev_num);
		return;
	}

	/* change port status */
	ast_usbhub_port_status[dev_num].wPortStatus &= ~(USB_PORT_STAT_CONNECTION | USB_PORT_STAT_ENABLE | USB_PORT_STAT_SUSPEND | USB_PORT_STAT_HIGH_SPEED | USB_PORT_STAT_LOW_SPEED);
	ast_usbhub_port_status[dev_num].wPortChange |= USB_PORT_STAT_C_CONNECTION;

	/* inform host status change */
	ast_usbhub_set_bitmap(dev_num);

	if (ast_usbhub_remote_wakeup_enable) {
		reg = ast_usbhub_read_reg(AST_USBHUB_ROOT);
		reg |= AST_USBHUB_ROOT_MANUAL_REMOTE_WAKEUP;
		ast_usbhub_write_reg(reg, AST_USBHUB_ROOT);
	}

	ast_usbhub_device_connect_to_port[dev_num] = 0;
}

void ast_usbhub_dev_connect(uint8_t dev_num)
{
	uint32_t reg;

	TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_CALL, "call conn dev %d\n", dev_num);

	dev_num = ast_usbhub_convert_dev_to_port(dev_num);

	if (ast_usbhub_device_connect_to_port[dev_num]) {
		printk(KERN_WARNING "dev %d already connect to host\n", dev_num);
		return;
	}

	/* change port status */
	ast_usbhub_port_status[dev_num].wPortStatus &= ~USB_PORT_STAT_SUSPEND;
	ast_usbhub_port_status[dev_num].wPortStatus |= USB_PORT_STAT_CONNECTION;
	ast_usbhub_port_status[dev_num].wPortChange |= USB_PORT_STAT_C_CONNECTION;

	/* inform host status change */
	ast_usbhub_set_bitmap(dev_num);

	if (ast_usbhub_remote_wakeup_enable) {
		reg = ast_usbhub_read_reg(AST_USBHUB_ROOT);
		reg |= AST_USBHUB_ROOT_MANUAL_REMOTE_WAKEUP;
		ast_usbhub_write_reg(reg, AST_USBHUB_ROOT);
	}

	ast_usbhub_device_connect_to_port[dev_num] = 1;
}

int ast_usbhub_enable_ep(uint8_t dev_num, uint8_t ep_num, uint8_t ep_dir, uint8_t ep_type)
{
	int32_t reg;
	int8_t type;

	TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_CALL, "call en ep %d %d %d %d\n", dev_num, ep_num, ep_dir, ep_type);

	if (ep_num > AST_USBHUB_DEV_EP_NUM)
		return -EINVAL;

	if (ep_num == 0) {
		printk(KERN_WARNING "ASTHUB: Warning, attemp to enable EP 0 of DEV %d\n", dev_num);
		return 0;
	}

	dev_num = ast_usbhub_convert_dev_to_port(dev_num);
	ep_num = ast_usbhub_convert_ep_num(dev_num, ep_num);

	switch (ep_type & USB_ENDPOINT_XFERTYPE_MASK) {
	case USB_ENDPOINT_XFER_BULK:
		type = (ep_dir == DIR_IN) ? AST_USBHUB_EP_TYPE_BULK_IN : AST_USBHUB_EP_TYPE_BULK_OUT;
		break;
	case USB_ENDPOINT_XFER_INT:
		type = (ep_dir == DIR_IN) ? AST_USBHUB_EP_TYPE_INTR_IN : AST_USBHUB_EP_TYPE_INTR_OUT;
		break;
	default:
		return -EINVAL;
	}

	/* reset descriptor list */
	ast_usbhub_write_reg(AST_USBHUB_EP_DESC_CSR_RESET, AST_USBHUB_EP_DESC_CSR(ep_num));

	/* we use single descriptor mode */
	ast_usbhub_write_reg(AST_USBHUB_EP_DESC_CSR_SINGLE, AST_USBHUB_EP_DESC_CSR(ep_num));

	/* hardware works with bus(physical) address */
	ast_usbhub_write_reg(ast_usbhub_ep_buf_dma[ep_num], AST_USBHUB_EP_DESC_ADDR(ep_num));

	/* initialize data toggle value */
	reg = AST_USBHUB_EP_TOGGLE_DATA0 | ((ep_num << AST_USBHUB_EP_TOGGLE_INDEX_SHIFT) & AST_USBHUB_EP_TOGGLE_INDEX_MASK);
	ast_usbhub_write_reg(reg, AST_USBHUB_EP_TOGGLE);

	/* enable interrupts */
	reg = ast_usbhub_read_reg(AST_USBHUB_EP_ACK_ICR) | (AST_USBHUB_EP_IRQ_SHIFT << ep_num);
	ast_usbhub_write_reg(reg, AST_USBHUB_EP_ACK_ICR);

	/* configure endpoint and enable it */
	reg = (ast_usbhub_invert_ep_num(ep_num) << AST_USBHUB_EP_CONF_NUM_SHIFT) |
			(type << AST_USBHUB_EP_CONF_TYPE_SHIFT) |
			((dev_num + 1) << AST_USBHUB_EP_CONF_PORT_SHIFT) |
			AST_USBHUB_EP_CONF_ENABLB;
	ast_usbhub_write_reg(reg, AST_USBHUB_EP_CONF(ep_num));

	/* initialize descriptor read/write pointer */
	reg = ((type & AST_USBHUB_EP_TYPE_DIR_MASK) == AST_USBHUB_EP_TYPE_DIR_OUT) ? AST_USBHUB_EP_DESC_PTR_SW_WRITE_SINGLE : 0;
	ast_usbhub_write_reg(reg, AST_USBHUB_EP_DESC_PTR(ep_num));

	return 0;
}

int ast_usbhub_disable_ep(uint8_t dev_num, uint8_t ep_num, uint8_t ep_dir, uint8_t ep_type)
{
	int32_t reg;

	TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_CALL, "call dis ep %d %d\n", dev_num, ep_num);

	if (ep_num > AST_USBHUB_DEV_EP_NUM)
		return -EINVAL;

	if (ep_num == 0) {
		printk(KERN_WARNING "ASTHUB: Warning, attemp to disable EP 0 of DEV %d\n", dev_num);
		return 0;
	}

	dev_num = ast_usbhub_convert_dev_to_port(dev_num);
	ep_num = ast_usbhub_convert_ep_num(dev_num, ep_num);

	/* disable interrupts */
	reg = ast_usbhub_read_reg(AST_USBHUB_EP_ACK_ICR) & ~(AST_USBHUB_EP_IRQ_SHIFT << ep_num);
	ast_usbhub_write_reg(reg, AST_USBHUB_EP_ACK_ICR);

	/* reset descriptor list */
	ast_usbhub_write_reg(AST_USBHUB_EP_DESC_CSR_RESET, AST_USBHUB_EP_DESC_CSR(ep_num));

	/* disable ep */
	ast_usbhub_write_reg(0, AST_USBHUB_EP_CONF(ep_num));

	return 0;
}

int ast_usbhub_get_ep_status(uint8_t dev_num, uint8_t ep_num, uint8 ep_dir, uint8_t *enable, uint8_t *stall)
{
	int32_t reg;

	TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_CALL, "call get ep %d %d\n", dev_num, ep_num);

	if (ep_num > AST_USBHUB_DEV_EP_NUM)
		return -EINVAL;

	if (ep_num == 0) {
		printk(KERN_WARNING "ASTHUB: Warning, attemp to disable EP 0 of DEV %d\n", dev_num);
		return 0;
	}

	dev_num = ast_usbhub_convert_dev_to_port(dev_num);
	ep_num = ast_usbhub_convert_ep_num(dev_num, ep_num);

	if (ep_num == AST_USBHUB_DEV_EP0_ID) {
		reg = ast_usbhub_read_reg(AST_USBHUB_DEV_EP0_CSR(dev_num));
		*stall = (reg & AST_USBHUB_DEV_EP0_STALL) ? 1 : 0;
	} else {
		reg = ast_usbhub_read_reg(AST_USBHUB_EP_CONF(ep_num));
		*stall = (reg & AST_USBHUB_EP_CONF_STALL) ? 1 : 0;
	}

	return 0;
}

int ast_usbhub_stall_ep(uint8_t dev_num, uint8_t ep_num, uint8_t ep_dir)
{
	uint32_t reg;

	TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_CALL, "call stall ep %d %d\n", dev_num, ep_num);

	if (ep_num > AST_USBHUB_DEV_EP_NUM)
		return -EINVAL;

	dev_num = ast_usbhub_convert_dev_to_port(dev_num);
	ep_num = ast_usbhub_convert_ep_num(dev_num, ep_num);

	if (ep_num == AST_USBHUB_DEV_EP0_ID) {
		ast_usbhub_write_reg(AST_USBHUB_DEV_EP0_STALL, AST_USBHUB_DEV_EP0_CSR(dev_num));
	} else {
		reg = ast_usbhub_read_reg(AST_USBHUB_EP_CONF(ep_num));
		reg |= AST_USBHUB_EP_CONF_STALL;
		ast_usbhub_write_reg(reg, AST_USBHUB_EP_CONF(ep_num));
	}

	/* call halt handler of device module via core module */
	usb_core_module.CoreUsbHaltHandler(ast_usbhub_dev_cfg[dev_num].dev_num, ast_usbhub_invert_ep_num(ep_num), ep_dir);

	return 0;
}

int ast_usbhub_unstall_ep(uint8_t dev_num, uint8_t ep_num, uint8_t ep_dir)
{
	uint32_t reg;

	TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_CALL, "call unstall ep %d %d\n", dev_num, ep_num);

	if (ep_num > AST_USBHUB_DEV_EP_NUM)
		return -EINVAL;

	dev_num = ast_usbhub_convert_dev_to_port(dev_num);
	ep_num = ast_usbhub_convert_ep_num(dev_num, ep_num);

	if (ep_num == AST_USBHUB_DEV_EP0_ID) {
		ast_usbhub_write_reg(0, AST_USBHUB_DEV_EP0_CSR(dev_num));
	} else {
		/* clear STALL */
		reg = ast_usbhub_read_reg(AST_USBHUB_EP_CONF(ep_num));
		reg &= ~AST_USBHUB_EP_CONF_STALL;
		ast_usbhub_write_reg(reg, AST_USBHUB_EP_CONF(ep_num));

		/* initialize data toggle value */
		reg = AST_USBHUB_EP_TOGGLE_DATA0 | ((ep_num << AST_USBHUB_EP_TOGGLE_INDEX_SHIFT) & AST_USBHUB_EP_TOGGLE_INDEX_MASK);
		ast_usbhub_write_reg(reg, AST_USBHUB_EP_TOGGLE);
	}

	/* call unhalt handler of device module via core module */
	usb_core_module.CoreUsbUnHaltHandler(ast_usbhub_dev_cfg[dev_num].dev_num, ast_usbhub_invert_ep_num(ep_num),ep_dir);

	return 0;
}

int ast_usbhub_get_ep_configs(uint8_t dev_num, uint8_t *config_data, uint8_t *num_eps)
{
	TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_CALL, "call get ep cfg %d\n", dev_num);

	memcpy(config_data, ast_usbhub_ep_configs, sizeof(ast_usbhub_ep_configs));
	*num_eps = sizeof(ast_usbhub_ep_configs) / sizeof(ep_config_t); /* AST_USBHUB_DEV_EP_NUM */

	return 0;
}

/* TODO remote wakeup for each device */
void ast_usbhub_set_remote_wakeup(uint8_t dev_num, uint8_t enable)
{
	TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_CALL, "call set wake %d\n", dev_num);

	#ifdef AST_USBHUB_AUTO_REMOTE_WAKEUP
	reg = ast_usbhub_read_reg(AST_USBHUB_ROOT);
	reg = (enable) ? (reg | AST_USBHUB_ROOT_AUTO_REMOTE_WAKEUP) : (reg & ~AST_USBHUB_ROOT_AUTO_REMOTE_WAKEUP);
	ast_usbhub_write_reg(reg, AST_USBHUB_ROOT);
	#else /* manually remote wakeup */
	ast_usbhub_remote_wakeup_enable = enable ? 1 : 0;
	#endif
}

uint8_t ast_usbhub_get_remote_wakeup(uint8_t dev_num)
{
	TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_CALL, "call get wake %d\n", dev_num);

	#ifdef AST_USBHUB_AUTO_REMOTE_WAKEUP
	reg = ast_usbhub_read_reg(AST_USBHUB_ROOT);
	return (reg & AST_USBHUB_ROOT_AUTO_REMOTE_WAKEUP) ? 1 : 0;
	#else /* manually remote wakeup */
	return ast_usbhub_remote_wakeup_enable;
	#endif
}

void ast_usbhub_remote_wakeup(uint8_t dev_num)
{
	uint32_t reg;

	TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_CALL, "call wake %d\n", dev_num);

	dev_num = ast_usbhub_convert_dev_to_port(dev_num);

	if (!ast_usbhub_remote_wakeup_enable)
		return;


	if (ast_usbhub_port_status[dev_num].wPortStatus & USB_PORT_STAT_SUSPEND) {
		/* clear suspend status and inform host status change */
		ast_usbhub_port_status[dev_num].wPortStatus &= ~USB_PORT_STAT_SUSPEND;
		ast_usbhub_port_status[dev_num].wPortChange |= USB_PORT_STAT_C_SUSPEND;
		ast_usbhub_set_bitmap(dev_num);
	}

	/* trigger a remote wakup signal */
	reg = ast_usbhub_read_reg(AST_USBHUB_ROOT);
	reg |= AST_USBHUB_ROOT_MANUAL_REMOTE_WAKEUP;
	ast_usbhub_write_reg(reg, AST_USBHUB_ROOT);
}

int ast_usbhub_read_data_from_host(uint8_t dev_num, uint8_t ep_num, uint8_t *buf, uint16_t *len, uint8_t is_setup)
{
	uint32_t reg;

	if (ep_num == AST_USBHUB_DEV_EP0_ID) {
		if (is_setup) {
			/* read request data */
			reg = ast_usbhub_read_reg(AST_USBHUB_DEV_SETUP_BUF(dev_num));
			put_value_to_bytes_le32(buf, reg);
			reg = ast_usbhub_read_reg(AST_USBHUB_DEV_SETUP_BUF(dev_num) + 0x04);
			put_value_to_bytes_le32(buf + 4, reg);
			*len = 8;
		} else {
			/* read data length */
			reg = ast_usbhub_read_reg(AST_USBHUB_DEV_EP0_CSR(dev_num));
			*len = (reg & AST_USBHUB_DEV_EP0_OUT_BUF_LEN_MASK) >> AST_USBHUB_DEV_EP0_OUT_BUF_LEN_SHIFT;

			if (*len != 0) {
				/* read data received in data stage */
				memcpy(buf, ast_usbhub_dev_ep0_buf_addr[dev_num], *len);
			}
		}
	} else {
		/* read data length */
		reg = ast_usbhub_read_reg(AST_USBHUB_EP_DESC_PTR(ep_num));
		*len = (reg & AST_USBHUB_EP_DESC_PTR_SIZE_MASK) >> AST_USBHUB_EP_DESC_PTR_SIZE_SHITF;

		if (*len != 0) {
			/* read data received in data stage */
			memcpy(buf, ast_usbhub_ep_buf_addr[ep_num], *len);
		}

		wmb();

		/* ready for receive next packet */
		reg = AST_USBHUB_EP_DESC_PTR_SW_WRITE_SINGLE;
		ast_usbhub_write_reg(reg, AST_USBHUB_EP_DESC_PTR(ep_num));
	}

	return 0;
}

int ast_usbhub_write_data_to_host(uint8_t dev_num, uint8_t ep_num, uint8_t *buf, uint16_t len)
{
	uint32_t reg;

	TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_CALL, "call write %d %d %d\n", dev_num, ep_num, len);
	if (ep_num > AST_USBHUB_DEV_EP_NUM)
		return -EINVAL;

	dev_num = ast_usbhub_convert_dev_to_port(dev_num);
	ep_num = ast_usbhub_convert_ep_num(dev_num, ep_num);

	if (ep_num == AST_USBHUB_DEV_EP0_ID) {
		if (len != 0) {
			/* write data from buffer into DMA memory */
			memcpy(ast_usbhub_dev_ep0_buf_addr[dev_num], buf, len);
		}

		wmb();

		/* set data length */
		reg = (len << AST_USBHUB_DEV_EP0_IN_BUF_LEN_SHIFT) & AST_USBHUB_DEV_EP0_IN_BUF_LEN_MASK;
		ast_usbhub_write_reg(reg, AST_USBHUB_DEV_EP0_CSR(dev_num));

		ast_usbhub_dev_tx_ready[dev_num] = 1;
	} else {
		if (len != 0) {
			/* write data from buffer into DMA memory */
			memcpy(ast_usbhub_ep_buf_addr[ep_num], buf, len);
		}

		wmb();

		/* set data length */
		reg = (len << AST_USBHUB_EP_DESC_PTR_SIZE_SHITF) & AST_USBHUB_EP_DESC_PTR_SIZE_MASK;
		ast_usbhub_write_reg(reg, AST_USBHUB_EP_DESC_PTR(ep_num));

		wmb();

		/* trigger a write operation */
		reg = ast_usbhub_read_reg(AST_USBHUB_EP_DESC_PTR(ep_num));
		reg |= AST_USBHUB_EP_DESC_PTR_SW_WRITE_SINGLE;
		ast_usbhub_write_reg(reg, AST_USBHUB_EP_DESC_PTR(ep_num));
	}

	return 0;
}

void ast_usbhub_set_addr(uint8_t dev_num, uint8_t addr, uint8_t enable)
{
	uint32_t reg;

	TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_CALL, "call set addr %d %d\n", dev_num, addr);

	dev_num = ast_usbhub_convert_dev_to_port(dev_num);

	ast_usbhub_dev_addr[dev_num] = addr;
	if (addr == 0)
		return;

	ast_usbhub_dev_set_addr_req[dev_num] = 1;

	/* according to AST2100 spec, when upstream port is high and downstream port is full/low,
	   we must make hardware to finish the SPLIT IN transcation without waiting the next START SPLIT packet */
	if (ast_usbhub_hub_speed == USB_SPEED_HIGH && ast_usbhub_dev_run_speed[dev_num] != SUPPORT_HIGH_SPEED) {
		reg = ast_usbhub_read_reg(AST_USBHUB_ROOT);
		reg |= AST_USBHUB_ROOT_COMPLETE_SPLIT_IN_TRANS;
		ast_usbhub_write_reg(reg, AST_USBHUB_ROOT);
	}
}

void ast_usbhub_get_addr(uint8_t dev_num, uint8_t *addr, uint8_t *enable)
{
	TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_CALL, "call get addr %d\n", dev_num);

	dev_num = ast_usbhub_convert_dev_to_port(dev_num);

	*addr = ast_usbhub_dev_addr[dev_num];
}

int ast_usbhub_alloc_buf(uint8_t dev_num, uint8_t ep_num, uint16_t buf, uint16_t max_pkt, uint8_t dir, uint8_t type)
{
	return 0;
}

void ast_usbhub_complete_req(uint8_t dev_num, uint8_t status, DEVICE_REQUEST *ctrl_req)
{
	uint32_t reg;
	int complete_type;

	TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_CALL, "call comp req %d %d for %x\n", dev_num, status, ctrl_req->bRequest);

	dev_num = ast_usbhub_convert_dev_to_port(dev_num);

	if (status == 1) {
		ast_usbhub_stall_ep(dev_num, 0, DIR_IN);
	} else {
		if (le16_to_cpu(ctrl_req->wLength) == 0x0000) { /* SETUP -> IN */
			complete_type = 1;
		} else {
			if (ctrl_req->bmRequestType & USB_DIR_IN) { /* SETUP -> IN -> OUT */
				complete_type = 0;
			} else { /* SETUP -> OUT -> IN */
				complete_type = 1;
			}
		}

		/* complete status stage */
		if (complete_type == 1) {
			reg = AST_USBHUB_DEV_EP0_IN_BUF_TX;
			ast_usbhub_write_reg(reg, AST_USBHUB_DEV_EP0_CSR(dev_num));
		} else {
			reg = AST_USBHUB_DEV_EP0_OUT_BUF_RX;
			ast_usbhub_write_reg(reg, AST_USBHUB_DEV_EP0_CSR(dev_num));
		}
	}
}
#ifdef CONFIG_SPX_FEATURE_POWER_CONSUMPTION_SUPPORT
static int ast_usbhub_UpstreamDisable(uint8 DevNo)
{
	ast_usbhub_clear_and_disconnect();
	return 0;
}

static int ast_usbhub_UpstreamEnable(uint8 DevNo)
{
	//init and connect usb only if "IsUsb_Upstream_host" value is not 1
	if(IsUsb_Upstream_host  != 1){
		IsUsb_Upstream_host = 1;
		ast_usbhub_init_and_connect();
	}
	return 0;
}
#endif
/* ASTHUB Hardware functions structure */
USB_HW ast_usbhub_hw = {
	.UsbHwEnable = ast_usbhub_enable_dev,
	.UsbHwDisable = ast_usbhub_disable_dev,
	.UsbHwIntr = NULL,
	.UsbHwSetAddr = ast_usbhub_set_addr,
	.UsbHwGetAddr = ast_usbhub_get_addr, /* uesless */
	.UsbHwWrite = ast_usbhub_write_data_to_host,
	.UsbTxComplete = NULL,
	.UsbHwRead = ast_usbhub_read_data_from_host, /* only used by hardware driver itself */
	.UsbHwAllocBuffer = ast_usbhub_alloc_buf,
	.UsbHwEnableEp = ast_usbhub_enable_ep,
	.UsbHwDisableEp = ast_usbhub_disable_ep,
	.UsbHwGetEpStatus = ast_usbhub_get_ep_status,
	.UsbHwStallEp = ast_usbhub_stall_ep,
	.UsbHwUnstallEp = ast_usbhub_unstall_ep,
	.UsbHwSetRemoteWakeup = ast_usbhub_set_remote_wakeup,
	.UsbHwGetRemoteWakeup = ast_usbhub_get_remote_wakeup,
 	.UsbHwCompleteRequest = ast_usbhub_complete_req,
	.UsbHwDeviceDisconnect = ast_usbhub_dev_disconnect,
	.UsbHwDeviceReconnect = ast_usbhub_dev_connect,
	.UsbHwRemoteWakeup = ast_usbhub_remote_wakeup,
	.UsbHwGetEps = ast_usbhub_get_ep_configs,
	.NumEndpoints = AST_USBHUB_DEV_EP_NUM,
#ifdef CONFIG_SPX_FEATURE_POWER_CONSUMPTION_SUPPORT
	.UsbHwUpstreamDisable = ast_usbhub_UpstreamDisable,
	.UsbHwUpstreamEnable = ast_usbhub_UpstreamEnable,
#endif
	.BigEndian = 0, /* Little Endian */
	.WriteFifoLock = 0, /* write FIFO lock is not needed */
	.EP0Size = AST_USBHUB_DEV_EP0_BUF_SIZE,
#ifdef CONFIG_SPX_FEATURE_USB11_PRESENTED
	.SupportedSpeeds = SUPPORT_HIGH_SPEED | SUPPORT_FULL_SPEED,
#else
	.SupportedSpeeds = SUPPORT_HIGH_SPEED | SUPPORT_FULL_SPEED | SUPPORT_LOW_SPEED,
#endif
};

/*-----------------------------------------------------------------------------------------------*/

static inline int ast_usbhub_ep0_is_stall(uint8_t dev_num)
{
	return (ast_usbhub_read_reg(AST_USBHUB_DEV_EP0_CSR(dev_num)) & AST_USBHUB_DEV_EP0_STALL) ? 1 : 0;
}

void ast_usbhub_ep0_rx_done(uint8_t dev_num, int is_setup)
{
	uint8_t *buf;
	uint16_t len;

	/* get ep 0 buffer */
	buf = usb_core_module.CoreUsbGetRxData(ast_usbhub_dev_cfg[dev_num].dev_num, 0);
	
	if(buf == NULL)
	{
		return;
	
	}

	/* read data */
	ast_usbhub_read_data_from_host(dev_num, AST_USBHUB_DEV_EP0_ID, buf, &len, is_setup);

	/* set length of buffer */
	usb_core_module.CoreUsbSetRxDataLen(ast_usbhub_dev_cfg[dev_num].dev_num, 0, len);

	/* call rx handler of device module via core */
	usb_core_module.CoreUsbRxHandler0(ast_usbhub_dev_cfg[dev_num].dev_num, buf, len, is_setup);
}

void ast_usbhub_ep0_tx_done(uint8_t dev_num)
{
	uint32_t reg;

	if (ast_usbhub_dev_set_addr_req[dev_num]) {
		ast_usbhub_dev_set_addr_req[dev_num] = 0;

		reg = ast_usbhub_read_reg(AST_USBHUB_DEV_EP0_ICR(dev_num));
		reg &= ~AST_USBHUB_DEV_ICR_ADDR_MASK; /* clear address field */
		reg |= (ast_usbhub_dev_addr[dev_num] << AST_USBHUB_DEV_ICR_ADDR_SHIFT) & AST_USBHUB_DEV_ICR_ADDR_MASK;
		ast_usbhub_write_reg(reg, AST_USBHUB_DEV_EP0_ICR(dev_num));

		/* according to AST2100 spec, when upstream port is high and downstream port is full/low,
		   we must make hardware to finish the SPLIT IN transcation without waiting the next START SPLIT packet */
		if (ast_usbhub_hub_speed == USB_SPEED_HIGH && ast_usbhub_dev_run_speed[dev_num] != SUPPORT_HIGH_SPEED) {
			reg = ast_usbhub_read_reg(AST_USBHUB_ROOT);
			reg &= ~AST_USBHUB_ROOT_COMPLETE_SPLIT_IN_TRANS;
			ast_usbhub_write_reg(reg, AST_USBHUB_ROOT);
		}
	}

	/* call tx handler of device module via core */
	usb_core_module.CoreUsbTxHandler(ast_usbhub_dev_cfg[dev_num].dev_num, 0);
}

void ast_usbhub_dev_irq_handler(uint8_t port)
{
	uint32_t reg;
	uint32_t irq_status;

	irq_status = ast_usbhub_read_reg(AST_USBHUB_DEV_EP0_ISR(port));
	ast_usbhub_write_reg(irq_status, AST_USBHUB_DEV_EP0_ISR(port));

	if (irq_status & AST_USBHUB_DEV_ISR_EP0_OUT_ACK) { /* finish OUT token */
		TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_IRQ, "dev %d ctrl OUT ACK\n", port);
		if (ast_usbhub_ep0_is_stall(port))
			return;
		ast_usbhub_ep0_rx_done(port, 0);
	} else if (irq_status & AST_USBHUB_DEV_ISR_EP0_OUT_NAK) { /* receive OUT token */
		/* ready for rx */
		reg = ast_usbhub_read_reg(AST_USBHUB_DEV_EP0_CSR(port));
		reg |= AST_USBHUB_DEV_EP0_OUT_BUF_RX;
		ast_usbhub_write_reg(reg, AST_USBHUB_DEV_EP0_CSR(port));
	}

	if (irq_status & AST_USBHUB_DEV_ISR_EP0_IN_ACK) { /* finish IN token */
		TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_IRQ, "dev %d ctrl IN ACK\n", port);
		if (ast_usbhub_ep0_is_stall(port))
			return;
		ast_usbhub_ep0_tx_done(port);
	} else if (irq_status & AST_USBHUB_DEV_ISR_EP0_IN_NAK) { /* receive IN token */
		if (ast_usbhub_dev_tx_ready[port]) {
			ast_usbhub_dev_tx_ready[port] = 0;

			/* trigger a write operation */
			reg = ast_usbhub_read_reg(AST_USBHUB_DEV_EP0_CSR(port));
			reg |= AST_USBHUB_DEV_EP0_IN_BUF_TX;
			ast_usbhub_write_reg(reg, AST_USBHUB_DEV_EP0_CSR(port));
		}
	}

	if (irq_status & AST_USBHUB_DEV_ISR_EP0_SETUP_ACK) { /* receive SETUP token and data */
		TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_IRQ, "dev %d SETUP\n", port);
		ast_usbhub_ep0_rx_done(port, 1);
	}
}

void ast_usbhub_ep_tx_done(uint8_t dev_num, uint8_t ep_num)
{
	TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_IRQ, "dev %d ep %d IN ACK\n", dev_num, ep_num);

	usb_core_module.CoreUsbTxHandler(ast_usbhub_dev_cfg[dev_num].dev_num, ast_usbhub_invert_ep_num(ep_num));
}

void ast_usbhub_ep_rx_done(uint8_t dev_num, uint8_t ep_num)
{
	uint8_t *buf;
	uint16_t len;

	TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_IRQ, "dev %d ep %d OUT ACK\n", dev_num, ep_num);

	/* get ep buffer */
	buf = usb_core_module.CoreUsbGetRxData(ast_usbhub_dev_cfg[dev_num].dev_num, ast_usbhub_invert_ep_num(ep_num));
	
	if(buf == NULL)
	{
		return;
	}

	/* read data */
	ast_usbhub_read_data_from_host(dev_num, ep_num, buf, &len, 0);

	/* set length of buffer */
	usb_core_module.CoreUsbSetRxDataLen(ast_usbhub_dev_cfg[dev_num].dev_num, ast_usbhub_invert_ep_num(ep_num), len);

	/* call rx handler of device module via core */
	usb_core_module.CoreUsbRxHandler(ast_usbhub_dev_cfg[dev_num].dev_num, ast_usbhub_invert_ep_num(ep_num));
}

void ast_usbhub_ep_ack_irq_handler(uint8_t ep_num)
{
	uint32_t reg;
	uint8_t dev_num;
	uint8_t type;

	/* clear interrupt status */
	reg = ast_usbhub_read_reg(AST_USBHUB_EP_ACK_ISR) & (AST_USBHUB_EP_IRQ_SHIFT << ep_num);
	ast_usbhub_write_reg(reg, AST_USBHUB_EP_ACK_ISR);

	reg = ast_usbhub_read_reg(AST_USBHUB_EP_CONF(ep_num));

	if (reg & AST_USBHUB_EP_CONF_STALL) { /* the endpoint is stall */
		TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_IRQ, "ep %d is STALL\n", ep_num);
		return;
	}

	dev_num = ((reg & AST_USBHUB_EP_CONF_PORT_MASK) >> AST_USBHUB_EP_CONF_PORT_SHIFT) - 1;
	type = (reg & AST_USBHUB_EP_CONF_TYPE_MASK) >> AST_USBHUB_EP_CONF_TYPE_SHIFT;
	if ((type & AST_USBHUB_EP_TYPE_DIR_MASK) == AST_USBHUB_EP_TYPE_DIR_OUT) { /* OUT ACK */
		ast_usbhub_ep_rx_done(dev_num, ep_num);
	} else { /* IN ACK */
		ast_usbhub_ep_tx_done(dev_num, ep_num);
	}
}

/*-----------------------------------------------------------------------------------------------*/

static int fill_config_buf(uint8_t *buf, unsigned int buf_len, const struct usb_descriptor_header **config_descs)
{
	unsigned int desc_len;
	unsigned int total_len;

	if (!config_descs)
		return -EINVAL;

	/* fill buffer from config_descs[] until null descriptor pointer */
	total_len = 0;
	while (*config_descs != NULL) {
		desc_len = (*config_descs)->bLength;
		if (buf_len < desc_len) /* no enough space */
			return -EINVAL;

		memcpy(buf, *config_descs, desc_len);
		buf_len -= desc_len;
		buf += desc_len;
		total_len += desc_len;
		config_descs ++; /* nex descriptor */
	}

	return total_len;
}

static int get_config_descs(uint8_t *buf, uint8_t type, uint8_t index)
{
	const struct usb_descriptor_header **function;
	enum usb_device_speed speed;
	int len;

	if (index > 0) /* we only support one configuration */
		return -EINVAL;

	speed = ast_usbhub_hub_speed;
	if (type == USB_DT_OTHER_SPEED_CONFIG)
		speed = (USB_SPEED_FULL + USB_SPEED_HIGH) - speed; /* get the other speed */

	function = (speed == USB_SPEED_HIGH) ? ast_usbhub_hub_hs_function : ast_usbhub_hub_fs_function;

	len = fill_config_buf(buf, AST_USBHUB_EP0_BUF_SIZE, function);
	((struct usb_config_descriptor *)buf)->bDescriptorType = type;
	((struct usb_config_descriptor *)buf)->wTotalLength = cpu_to_le16(len);

	return len;
}

static int ascii_to_unicode(const char *str, uint8_t *buf, unsigned int len)
{
	int i;

	i = 0;
	while (len != 0 && *str != '\0') {
		buf[i] = *str;
		buf[i + 1] = 0x00;
		i += 2;
		str ++;
		len --;
	}

	return i;
}

static int ast_usbhub_get_string_desc(struct usb_string *table, int id, uint8_t *buf)
{
	int i;
	int len;
	char *s;

	/* descriptor 0 has the language id */
	if (id == 0) {
		buf[0] = 4;
		buf[1] = USB_DT_STRING;
		buf[2] = (uint8_t) STRING_LANGUAGE_ID;
		buf[3] = (uint8_t) STRING_LANGUAGE_ID >> 8;
		return 4;
	}

	for (i = 0; table[i].s != NULL; i ++)
		if (table[i].id == id)
			break;

	/* unrecognized: STALL */
	if (table[i].s == NULL)
		return -EINVAL;
	s = table[i].s;

	/* string descriptors have length, tag, then UTF16-LE text */
	len = min((size_t) 126, strlen(s)); /* maximum length of string descriptor is 255, minus the first 2, remains 253 */
	memset(buf + 2, 0x00, 2 * len);
	len = ascii_to_unicode(s, buf + 2, len);

	buf [0] = 2 + len;
	buf [1] = USB_DT_STRING;

	return buf[0];
}

static void ast_usbhub_reset_port(uint8_t port)
{
	uint32_t reg;

	printk(KERN_WARNING "HUB port %d reset\n", port);

	ast_usbhub_disable_dev(ast_usbhub_dev_cfg[port].dev_num, ast_usbhub_dev_speed[port]);

	usb_core_module.CoreUsbBusReset(ast_usbhub_dev_cfg[port].dev_num);
	ast_usbhub_reset_dev(port);

	ast_usbhub_enable_dev(ast_usbhub_dev_cfg[port].dev_num, ast_usbhub_dev_speed[port]);

	wmb();

	reg = ast_usbhub_read_reg(AST_USBHUB_HUB_ADDR) & 0x7F;
	if (reg != 0) {
		/* enable device */
		reg = ast_usbhub_read_reg(AST_USBHUB_DEV_EP0_ICR(port)) | AST_USBHUB_DEV_ICR_ENABLE;
		ast_usbhub_write_reg(reg, AST_USBHUB_DEV_EP0_ICR(port));
	}
}

static void ast_usbhub_enable_port(uint8_t port)
{
	if (!ast_usbhub_device_enabled[port]) {
		ast_usbhub_enable_dev(ast_usbhub_dev_cfg[port].dev_num, ast_usbhub_dev_speed[port]);
	}
}

static void ast_usbhub_disable_port(uint8_t port)
{
	if (ast_usbhub_device_enabled[port]) {
		ast_usbhub_disable_dev(ast_usbhub_dev_cfg[port].dev_num, ast_usbhub_dev_speed[port]);
		usb_core_module.CoreUsbBusReset(ast_usbhub_dev_cfg[port].dev_num);
	}
}

static void ast_usbhub_suspend_port(uint8_t port)
{
	if (ast_usbhub_device_connect_to_port[port]) {
		usb_core_module.CoreUsbBusSuspend(ast_usbhub_dev_cfg[port].dev_num);
	}
}

static void ast_usbhub_resume_port(uint8_t port)
{
	if (ast_usbhub_device_connect_to_port[port]) {
		usb_core_module.CoreUsbBusResume(ast_usbhub_dev_cfg[port].dev_num);
	}
}

static void ast_usbhub_power_port(uint8_t port, int on)
{
	#if 0
	if (ast_usbhub_device_connect_to_port[port]) {
		if (on) {
			ast_usbhub_enable_dev(ast_usbhub_dev_cfg[port].dev_num);
		} else { /* off */
			ast_usbhub_disable_dev(ast_usbhub_dev_cfg[port].dev_num);
			usb_core_module.CoreUsbBusDisconnect(ast_usbhub_dev_cfg[port].dev_num);
		}
	}
	#endif
}

static void ast_usbhub_set_speed(void)
{
	uint32_t reg;
	int i;

	/* judge bus speed */
	reg = ast_usbhub_read_reg(AST_USBHUB_USB_STATUS);
	ast_usbhub_hub_speed = ((reg & AST_USBHUB_USB_STATUS_BUS_SPEED) == AST_USBHUB_USB_STATUS_BUS_HIGH) ? USB_SPEED_HIGH : USB_SPEED_FULL;

	for (i = 0; i < AST_USBHUB_DEV_NUM; i ++) {
		if (ast_usbhub_device_connect_to_port[i]) {
			ast_usbhub_configure_dev(i);
		}
	}
}

/* handle hub endpoint 0 control transfer class-specific setup request */
static int ast_usbhub_hub_setup_class_req(const struct usb_ctrlrequest *ctrl_req)
{
	uint16_t w_index = le16_to_cpu(ctrl_req->wIndex);
	uint16_t w_value = le16_to_cpu(ctrl_req->wValue);
	uint16_t w_length = le16_to_cpu(ctrl_req->wLength);
	uint8_t desc_type, desc_index;
	uint8_t port;
	int value = -EOPNOTSUPP;

	switch (ctrl_req->bRequest) {
	case USB_REQ_GET_DESCRIPTOR:
		TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_SETUP, "GET HUB DESC\n");
		ast_usbhub_hub_ctrl_data_stage = EP0_CTRL_DATA_STAGE_IN;
		if (ctrl_req->bRequestType != (USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_DEVICE)) /* 1 01 00000 */
			break;

		/* descriptor type in high byte of wValue, descriptor index in low byte of wValue */
		desc_type = w_value >> 8;
		desc_index = w_value & 0xFF;
		if ((desc_type != USB_DT_HUB) && (desc_type != 0x00))
			break;
		if (desc_index != 0) /* we only have 1 hub descriptor */
			break;

		value = (w_length < USB_DT_HUB_SIZE) ? w_length : USB_DT_HUB_SIZE;
		memcpy(ast_usbhub_hub_ep0_buf, &ast_usbhub_hub_class_desc, value);
		break;
	case USB_REQ_GET_STATUS:
		ast_usbhub_hub_ctrl_data_stage = EP0_CTRL_DATA_STAGE_IN;
		switch (ctrl_req->bRequestType) {
		case (USB_DIR_IN | USB_RT_HUB):
			TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_SETUP, "GET HUB STAT\n");

			if (w_index != 0 || w_value != 0 || w_length != USB_HUB_STATUS_SIZE)
				break;

			memcpy(ast_usbhub_hub_ep0_buf, (uint8_t *) &ast_usbhub_hub_class_status, USB_HUB_STATUS_SIZE);
			value = USB_HUB_STATUS_SIZE;
			break;
		case (USB_DIR_IN | USB_RT_PORT):
			if (w_value != 0 || w_length != USB_PORT_STATUS_SIZE)
				break;

			port = (w_index & 0xFF) - 1; /* USB Hub port number is begin from 1 */

			TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_SETUP, "GET PORT %d STAT\n", port);

			if (port >= AST_USBHUB_DEV_NUM)
				break;

			memcpy(ast_usbhub_hub_ep0_buf, &(ast_usbhub_port_status[port]), USB_PORT_STATUS_SIZE);
			value = USB_PORT_STATUS_SIZE;
			break;
		}
		break;
	case USB_REQ_CLEAR_FEATURE:
		switch (ctrl_req->bRequestType) {
		case USB_RT_HUB:
			if (w_index != 0 || w_length != 0 || w_value > C_HUB_OVER_CURRENT)
				break;

			ast_usbhub_hub_class_status.wHubChange &= ~(1 << w_value);
			value = 0;
			break;
		case USB_RT_PORT:
			port = (w_index & 0xFF) - 1; /* USB Hub port number is begin from 1 */
			if (port >= AST_USBHUB_DEV_NUM)
				break;

			ast_usbhub_clear_bitmap(port);
			switch (w_value) {
			case USB_PORT_FEAT_CONNECTION:
			case USB_PORT_FEAT_RESET:
				/* no operation */
				break;
			case USB_PORT_FEAT_ENABLE:
				TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_SETUP, "DIS PORT %d\n", port);

				ast_usbhub_port_status[port].wPortStatus &= ~(USB_PORT_STAT_ENABLE | USB_PORT_STAT_HIGH_SPEED);
				ast_usbhub_disable_port(port);
				break;
			case USB_PORT_FEAT_SUSPEND:
				TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_SETUP, "RESUME PORT %d\n", port);

				ast_usbhub_port_status[port].wPortStatus &= ~USB_PORT_STAT_SUSPEND;
				ast_usbhub_port_status[port].wPortChange |= USB_PORT_STAT_C_SUSPEND;
				ast_usbhub_resume_port(port);
				ast_usbhub_set_bitmap(port);
				break;
			case USB_PORT_FEAT_POWER:
				TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_SETUP, "PWR OFF PORT %d\n", port);

				ast_usbhub_port_status[port].wPortStatus = 0;
				ast_usbhub_port_status[port].wPortChange = 0;
				ast_usbhub_power_port(port, 0);
				break;
			case USB_PORT_FEAT_C_CONNECTION:
				ast_usbhub_port_status[port].wPortChange &= ~USB_PORT_STAT_C_CONNECTION;
				break;
			case USB_PORT_FEAT_C_ENABLE:
				ast_usbhub_port_status[port].wPortChange &= ~USB_PORT_STAT_C_ENABLE;
				break;
			case USB_PORT_FEAT_C_SUSPEND:
				ast_usbhub_port_status[port].wPortChange &= ~USB_PORT_STAT_C_SUSPEND;
				break;
			case USB_PORT_FEAT_C_RESET:
				ast_usbhub_port_status[port].wPortChange &= ~USB_PORT_STAT_C_RESET;
				break;
			}
			value = 0;
			break;
		}
		break;
	case USB_REQ_SET_FEATURE:
		switch (ctrl_req->bRequestType) {
		case USB_RT_HUB:
			if (w_index != 0 || w_length != 0 || w_value > C_HUB_OVER_CURRENT)
				break;

			ast_usbhub_hub_class_status.wHubChange |= (1 << w_value);
			value = 0;
			break;
		case USB_RT_PORT:
			port = (w_index & 0xFF) - 1; /* USB Hub port number is begin from 1 */
			if (port >= AST_USBHUB_DEV_NUM)
				break;

			switch (w_value) {
			case USB_PORT_FEAT_CONNECTION:
				/* no operation */
				break;
			case USB_PORT_FEAT_ENABLE:
				TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_SETUP, "EN PORT %d\n", port);

				ast_usbhub_port_status[port].wPortStatus |= (USB_PORT_STAT_POWER | USB_PORT_STAT_ENABLE | USB_PORT_STAT_CONNECTION);
				ast_usbhub_port_status[port].wPortChange |= USB_PORT_STAT_C_ENABLE;

				if (ast_usbhub_device_connect_to_port[port]) {
					if (ast_usbhub_hub_speed == USB_SPEED_HIGH) {
						if (ast_usbhub_dev_run_speed[port] == SUPPORT_HIGH_SPEED)
							ast_usbhub_port_status[port].wPortStatus |= USB_PORT_STAT_HIGH_SPEED;
						else if (ast_usbhub_dev_run_speed[port] == SUPPORT_LOW_SPEED)
							ast_usbhub_port_status[port].wPortStatus |= USB_PORT_STAT_LOW_SPEED;
					}
				}

				ast_usbhub_enable_port(port);
				break;
			case USB_PORT_FEAT_SUSPEND:
				TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_SETUP, "SUS PORT %d\n", port);

				ast_usbhub_port_status[port].wPortStatus |= USB_PORT_STAT_SUSPEND;

				ast_usbhub_suspend_port(port);
				break;
			case USB_PORT_FEAT_RESET:
				ast_usbhub_port_status[port].wPortStatus &= ~USB_PORT_STAT_SUSPEND;
				ast_usbhub_port_status[port].wPortStatus |= USB_PORT_STAT_POWER;
				ast_usbhub_port_status[port].wPortChange |= USB_PORT_STAT_C_RESET;

				if (ast_usbhub_device_connect_to_port[port]) { /* there is a device attached to this port */
					ast_usbhub_port_status[port].wPortStatus |= (USB_PORT_STAT_ENABLE | USB_PORT_STAT_CONNECTION);
					if (ast_usbhub_hub_speed == USB_SPEED_HIGH) {
						if (ast_usbhub_dev_speed[port] == SUPPORT_HIGH_SPEED)
							ast_usbhub_port_status[port].wPortStatus |= USB_PORT_STAT_HIGH_SPEED;
						else if (ast_usbhub_dev_speed[port] == SUPPORT_LOW_SPEED)
							ast_usbhub_port_status[port].wPortStatus |= USB_PORT_STAT_LOW_SPEED;
					}

					ast_usbhub_reset_port(port);
				}

				ast_usbhub_set_bitmap(port);
				break;
			case USB_PORT_FEAT_POWER:
				TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_SETUP, "PWR ON PORT %d\n", port);

				ast_usbhub_port_status[port].wPortStatus |= USB_PORT_STAT_POWER;

				if (ast_usbhub_device_connect_to_port[port]) { /* there is a device attached to this port */
					ast_usbhub_port_status[port].wPortStatus |= USB_PORT_STAT_CONNECTION;
					ast_usbhub_port_status[port].wPortChange |= USB_PORT_STAT_C_CONNECTION;

					if (ast_usbhub_hub_speed == USB_SPEED_HIGH) {
						if (ast_usbhub_dev_run_speed[port] == SUPPORT_HIGH_SPEED)
							ast_usbhub_port_status[port].wPortStatus |= USB_PORT_STAT_HIGH_SPEED;
						else if (ast_usbhub_dev_run_speed[port] == SUPPORT_LOW_SPEED)
							ast_usbhub_port_status[port].wPortStatus |= USB_PORT_STAT_LOW_SPEED;
					}

					ast_usbhub_set_bitmap(port);
					ast_usbhub_power_port(port, 1);
				}
				break;
			}
			value = 0;
			break;
		}
		break;
	case HUB_CLEAR_TT_BUFFER:
		TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_SETUP, "HUB CLEAR TT BUF\n");

		value = 0;
		break;
	case HUB_RESET_TT:
		TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_SETUP, "HUB RESET TT\n");

		value = 0;
		break;
	case HUB_GET_TT_STATE:
		TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_SETUP, "HUB GET TT STATUS\n");

		value = 0;
		break;
	case HUB_STOP_TT:
		TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_SETUP, "HUB STOP TT\n");

		value = 0;
		break;
	default:
		printk(KERN_WARNING "unknown hub req t %02x r %02x v %04x i %04x l %u\n",
			ctrl_req->bRequestType, ctrl_req->bRequest, w_value, w_index, le16_to_cpu(ctrl_req->wLength));
	}

	return value;
}

/* handle hub endpoint 0 control transfer standard setup request */
static int ast_usbhub_hub_setup_standard_req(const struct usb_ctrlrequest *ctrl_req)
{
	uint16_t w_index = le16_to_cpu(ctrl_req->wIndex);
	uint16_t w_value = le16_to_cpu(ctrl_req->wValue);
	uint16_t w_length = le16_to_cpu(ctrl_req->wLength);
	uint8_t desc_type, desc_index, recipient;
	uint32_t reg;
	int value = -EOPNOTSUPP;

	switch (ctrl_req->bRequest) {
	case USB_REQ_SET_ADDRESS:
		TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_SETUP, "SET ADDR\n");

		ast_usbhub_hub_dev_addr = w_value;
		value = 0;
		break;
	case USB_REQ_GET_DESCRIPTOR:
		ast_usbhub_hub_ctrl_data_stage = EP0_CTRL_DATA_STAGE_IN;
		if (ctrl_req->bRequestType != (USB_DIR_IN | USB_TYPE_STANDARD | USB_RECIP_DEVICE)) /* 1 00 00000 */
			break;

		/* descriptor type in high byte of wValue, descriptor index in low byte of wValue */
		desc_type = w_value >> 8;
		desc_index = w_value & 0xFF;
		switch (desc_type) {
		case USB_DT_DEVICE:
			TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_SETUP, "DEV DESC\n");

			value = USB_DT_DEVICE_SIZE;
			memcpy(ast_usbhub_hub_ep0_buf, &ast_usbhub_hub_dev_desc, value);
			break;
		case USB_DT_CONFIG:
			TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_SETUP, "CFG DESC\n");

			value = get_config_descs(ast_usbhub_hub_ep0_buf, desc_type, desc_index);
			break;
		case USB_DT_STRING:
			TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_SETUP, "STR DESC\n");

			/* wIndex == language code */
			value = ast_usbhub_get_string_desc(ast_usbhub_hub_desc_strings, desc_index, ast_usbhub_hub_ep0_buf);
			break;
		case USB_DT_DEVICE_QUALIFIER:
			TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_SETUP, "QUAL DESC\n");

			value = sizeof(ast_usbhub_hub_dev_qualifier_desc);
			ast_usbhub_hub_dev_qualifier_desc.bDeviceProtocol = (ast_usbhub_hub_speed == USB_SPEED_FULL) ? 0x01 : 0x00;
			memcpy(ast_usbhub_hub_ep0_buf, &ast_usbhub_hub_dev_qualifier_desc, value);
			break;
		case USB_DT_OTHER_SPEED_CONFIG:

			value = get_config_descs(ast_usbhub_hub_ep0_buf, desc_type, desc_index);
			break;
		}
		if (value > w_length)
			value = w_length;
		break;
	/* One config, two speeds */
	case USB_REQ_SET_CONFIGURATION:
		TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_SETUP, "SET CFG\n");

		if (ctrl_req->bRequestType != (USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_DEVICE)) /* 0 00 00000 */
			break;

		if (w_value == AST_USBHUB_HUB_CONFIG_VALUE || w_value == 0) {
			ast_usbhub_hub_current_config_value = w_value;
			value = 0;
		}
		break;
	case USB_REQ_GET_CONFIGURATION:
		TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_SETUP, "GET CFG\n");

		ast_usbhub_hub_ctrl_data_stage = EP0_CTRL_DATA_STAGE_IN;
		if (ctrl_req->bRequestType != (USB_DIR_IN | USB_TYPE_STANDARD | USB_RECIP_DEVICE)) /* 1 00 00000 */
			break;

		ast_usbhub_hub_ep0_buf[0] = ast_usbhub_hub_current_config_value;
		value = 1;
		break;
	case USB_REQ_SET_INTERFACE:
		TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_SETUP, "SET IF\n");

		if (ctrl_req->bRequestType != (USB_DIR_OUT| USB_TYPE_STANDARD | USB_RECIP_INTERFACE)) /* 0 00 00001 */
			break;

		value = 0;
		break;
	case USB_REQ_GET_INTERFACE:
		TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_SETUP, "GET IF\n");

		ast_usbhub_hub_ctrl_data_stage = EP0_CTRL_DATA_STAGE_IN;
		if (ctrl_req->bRequestType != (USB_DIR_IN | USB_TYPE_STANDARD | USB_RECIP_INTERFACE)) /* 1 00 00001 */
			break;
		if (!ast_usbhub_hub_current_config_value)
			break;
		if (w_index != 0) { /* we only have 1 interface: interface 0 */
			value = -EDOM;
			break;
		}

		ast_usbhub_hub_ep0_buf[0] = 0;
		value = 1;
		break;
	case USB_REQ_CLEAR_FEATURE:
		TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_SETUP, "CLR FEAT\n");

		if (w_value == USB_ENDPOINT_HALT) {
			if (ctrl_req->bRequestType != (USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_ENDPOINT)) /* 0 00 00010 */
				break;

			if ((w_index & 0x7F) == 1) { /* ep1 */
				reg = AST_USBHUB_HUB_EP1_RESET_TOGGLE | AST_USBHUB_HUB_EP1_CTRL;
				ast_usbhub_write_reg(reg, AST_USBHUB_HUB_EP1_CSR);
				value = 0;
			}
		} else if (w_value == USB_DEVICE_REMOTE_WAKEUP) {
			if (ctrl_req->bRequestType != (USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_DEVICE)) /* 0 00 00010 */
				break;

			/* disable remote wakeup */
			#ifdef AST_USBHUB_AUTO_REMOTE_WAKEUP
			reg = ast_usbhub_read_reg(AST_USBHUB_ROOT);
			reg &= ~AST_USBHUB_ROOT_AUTO_REMOTE_WAKEUP;
			ast_usbhub_write_reg(reg, AST_USBHUB_ROOT);
			#else /* manually remote wakeup */
			ast_usbhub_remote_wakeup_enable = 0;
			#endif

			ast_usbhub_hub_usb_status &= ~(1 << USB_DEVICE_REMOTE_WAKEUP);
			value = 0;
		}
		break;
	case USB_REQ_SET_FEATURE:
		TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_SETUP, "SET FEAT\n");

		if (w_value == USB_ENDPOINT_HALT) {
			if (ctrl_req->bRequestType != (USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_ENDPOINT)) /* 0 00 00010 */
				break;

			if ((w_index & 0x7F) == 1) { /* ep1 */
				/* halt ep1 */
				reg = ast_usbhub_read_reg(AST_USBHUB_HUB_EP1_CSR);
				reg |= AST_USBHUB_HUB_EP1_STALL;
				ast_usbhub_write_reg(reg, AST_USBHUB_HUB_EP1_CSR);
				value = 0;
			}
		} else if (w_value == USB_DEVICE_REMOTE_WAKEUP) {
			if (ctrl_req->bRequestType != (USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_DEVICE)) /* 0 00 00010 */
				break;

			/* enable remote wakeup */
			#ifdef AST_USBHUB_AUTO_REMOTE_WAKEUP
			reg = ast_usbhub_read_reg(AST_USBHUB_ROOT);
			reg |= AST_USBHUB_ROOT_AUTO_REMOTE_WAKEUP;
			ast_usbhub_write_reg(reg, AST_USBHUB_ROOT);
			#else /* manually remote wakeup */
			ast_usbhub_remote_wakeup_enable = 1;
			#endif

			ast_usbhub_hub_usb_status |= (1 << USB_DEVICE_REMOTE_WAKEUP);
			value = 0;
		}
		break;
	case USB_REQ_GET_STATUS:
		TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_SETUP, "GET STAT\n");

		ast_usbhub_hub_ctrl_data_stage = EP0_CTRL_DATA_STAGE_IN;
		recipient = ctrl_req->bRequestType & USB_RECIP_MASK;
		if (recipient == USB_RECIP_DEVICE) {
			ast_usbhub_hub_ep0_buf[0] = ast_usbhub_hub_usb_status;
		} else if (recipient == USB_RECIP_INTERFACE) {
			if (w_index == 0) { /* we only have 1 interface: interface 0 */
				ast_usbhub_hub_ep0_buf[0] = 0;
			} else {
				break;
			}
		} else if (recipient == USB_RECIP_ENDPOINT) {
			reg = ast_usbhub_read_reg(AST_USBHUB_HUB_EP1_CSR);
			ast_usbhub_hub_ep0_buf[0] = (reg & AST_USBHUB_HUB_EP1_STALL) ? 1 : 0;
		} else {
			break;
		}
		ast_usbhub_hub_ep0_buf[1] = 0;
		value = 2;
		break;
	default:
		printk(KERN_WARNING "unknown std req t %02x r %02x v %04x i %04x l %u\n",
				ctrl_req->bRequestType, ctrl_req->bRequest, w_value, w_index, le16_to_cpu(ctrl_req->wLength));
	}

	return value;
}

irqreturn_t ast_usbhub_irq_handler(int irq, void *priv)
{
	uint32_t irq_status;
	uint32_t reg;
	uint8_t setup_data[8];
	uint8_t port;//Quanta
	struct usb_ctrlrequest *ctrl_req;
	int length, i;

	irq_status = ast_usbhub_read_reg(AST_USBHUB_ISR);
	ast_usbhub_write_reg(irq_status, AST_USBHUB_ISR); /* clear interrupt status */

	if (irq_status & AST_USBHUB_IRQ_DEAD_LOCK)
		printk(KERN_ERR "USB DEAD LOCK\n");

	if (irq_status & AST_USBHUB_IRQ_BUS_RESET) {
		TDBG_FLAGGED(ast_usbhub_debug_flags, AST_USBHUB_DEBUG_SETUP, "USB bus reset\n");

		ast_usbhub_hub_dev_addr = 0x00;
		ast_usbhub_hub_current_config_value = 0;
		ast_usbhub_hub_usb_status = (1 << USB_DEVICE_SELF_POWERED);
		ast_usbhub_hub_class_status.wHubStatus = 0; /* Local power supply is good, no over-current */
		ast_usbhub_hub_class_status.wHubChange = 0; /* No change has occurred */
		ast_usbhub_hub_speed = USB_SPEED_FULL;
		ast_usbhub_remote_wakeup_enable = 0;

		for (i = 0; i < AST_USBHUB_DEV_NUM; i ++) {
			ast_usbhub_dev_addr[i] = 0x00;
			ast_usbhub_dev_set_addr_req[i] = 0;
			ast_usbhub_dev_tx_ready[i] = 0;
			ast_usbhub_port_status[i].wPortStatus = 0;
			ast_usbhub_port_status[i].wPortChange = 0;
			ast_usbhub_dev_run_speed[i] = SUPPORT_FULL_SPEED;
			//Quanta++
			port = ast_usbhub_convert_dev_to_port(ast_usbhub_dev_cfg[i].dev_num);
			ast_usbhub_clear_bitmap(port);
			//Quanta==
			if (ast_usbhub_device_connect_to_port[i]) {
				usb_core_module.CoreUsbBusReset(ast_usbhub_dev_cfg[i].dev_num);
			}

			if (ast_usbhub_device_enabled[i]) {
				/* disable device */
				reg = ast_usbhub_read_reg(AST_USBHUB_DEV_EP0_ICR(i));
				reg &= ~AST_USBHUB_DEV_ICR_ENABLE;
				ast_usbhub_write_reg(reg, AST_USBHUB_DEV_EP0_ICR(i));
				ast_usbhub_device_enabled[i] = 0;
			}
		}
	}

	if (irq_status & AST_USBHUB_IRQ_SUSPEND) {
		for (i = 0; i < AST_USBHUB_DEV_NUM; i ++) {
			/* call suspend handler of device */
			if (ast_usbhub_device_connect_to_port[i]) {
				usb_core_module.CoreUsbBusSuspend(ast_usbhub_dev_cfg[i].dev_num);
			}
		}
	}

	if (irq_status & AST_USBHUB_IRQ_RESUME) {
		for (i = 0; i < AST_USBHUB_DEV_NUM; i ++) {
			/* call resume handler of device */
			if (ast_usbhub_device_connect_to_port[i]) {
				usb_core_module.CoreUsbBusResume(ast_usbhub_dev_cfg[i].dev_num);
			}
		}
	}

	if (irq_status & AST_USBHUB_IRQ_EP0_IN_ACK) { /* finish IN token */
		if (saved_hub_ctrl_req == USB_REQ_SET_ADDRESS) {
			ast_usbhub_write_reg(ast_usbhub_hub_dev_addr & 0x7F, AST_USBHUB_HUB_ADDR);
			ast_usbhub_set_speed();
		}

		if (ast_usbhub_hub_ctrl_data_stage == EP0_CTRL_DATA_STAGE_IN) {
			/* complete status stage */
			reg = AST_USBHUB_HUB_EP0_OUT_BUF_RX;
			ast_usbhub_write_reg(reg, AST_USBHUB_HUB_EP0_CSR);
		}
	}

	if (irq_status & AST_USBHUB_IRQ_EP0_OUT_ACK) { /* finish OUT token */
		if (ast_usbhub_hub_ctrl_data_stage == EP0_CTRL_DATA_STAGE_OUT) {
			/* read data received in data stage */
			reg = ast_usbhub_read_reg(AST_USBHUB_HUB_EP0_CSR);
			length = (reg & AST_USBHUB_HUB_EP0_OUT_BUF_LEN_MASK) >> AST_USBHUB_HUB_EP0_OUT_BUF_LEN_SHIFT;
			/* TODO process data by saved_hub_ctrl_req */
			/* accroding to USB 2.0 spec, only SET_DESCRIPTOR request will transmit data to device in data stage */

			/* complete status stage*/
			reg = AST_USBHUB_HUB_EP0_IN_BUF_TX;
			ast_usbhub_write_reg(reg, AST_USBHUB_HUB_EP0_CSR);
		}
	}

	if (irq_status & AST_USBHUB_IRQ_EP0_SETUP_ACK) { /* receive SETUP token and data */
		/* read request data */
		reg = ast_usbhub_read_reg(AST_USBHUB_HUB_SETUP_BUF);
		put_value_to_bytes_le32(setup_data, reg);
		reg = ast_usbhub_read_reg(AST_USBHUB_HUB_SETUP_BUF + 0x04);
		put_value_to_bytes_le32(setup_data + 4, reg);
		ctrl_req = (struct usb_ctrlrequest *) setup_data;
		saved_hub_ctrl_req = ctrl_req->bRequest;

		/* handle request */
		ast_usbhub_hub_ctrl_data_stage = EP0_CTRL_DATA_STAGE_NONE;
		if ((ctrl_req->bRequestType & USB_TYPE_MASK) == USB_TYPE_CLASS) /* class-specific request */
			length = ast_usbhub_hub_setup_class_req(ctrl_req);
		else /* stardard request */
			length = ast_usbhub_hub_setup_standard_req(ctrl_req);

		if (length < 0) { /* error */
			reg = AST_USBHUB_HUB_EP0_STALL;
		} else {
			if (ast_usbhub_hub_ctrl_data_stage == EP0_CTRL_DATA_STAGE_OUT) {
				/* receive data from host */
				reg = AST_USBHUB_HUB_EP0_OUT_BUF_RX;
			} else {  /* IN or NONE */
				/* transmit data to host */
				reg = ((length << AST_USBHUB_HUB_EP0_IN_BUF_LEN_SHIFT) & AST_USBHUB_HUB_EP0_IN_BUF_LEN_MASK) | AST_USBHUB_HUB_EP0_IN_BUF_TX;
			}
		}
		wmb();
		ast_usbhub_write_reg(reg, AST_USBHUB_HUB_EP0_CSR);
	}

	if (irq_status & AST_USBHUB_IRQ_DEV_MASK) { /* device ep0 */
		for (i = 0; i < AST_USBHUB_DEV_NUM; i ++) {
			if (irq_status & (AST_USBHUB_IRQ_DEV_BASE << i)) {
				ast_usbhub_dev_irq_handler(i);
			}
		}
	}

	if (irq_status & AST_USBHUB_IRQ_EP_POOL_ACK) { /* endpoint */
		reg = ast_usbhub_read_reg(AST_USBHUB_EP_ACK_ISR);
		for (i = 0; i < AST_USBHUB_EP_NUM; i ++) {
			if (reg & (AST_USBHUB_EP_IRQ_SHIFT << i)) {
				ast_usbhub_ep_ack_irq_handler(i);
			}
		}
	}

	return IRQ_HANDLED;
}

int ast_usbhub_init(uint8_t dev_num, USB_HW *usb_hw_module, void **dev_config)
{
	int i;

	for (i = 0; i < AST_USBHUB_USE_DEV_NUM; i ++) {
		if (ast_usbhub_dev_cfg[i].dev_num == 0xFF)
			break;
	}

	if (i == AST_USBHUB_USE_DEV_NUM)
		return -1;

	ast_usbhub_dev_cfg[i].dev_num = dev_num;

	memcpy(usb_hw_module, &ast_usbhub_hw, sizeof(USB_HW));

	*dev_config = &(ast_usbhub_dev_cfg[i]);

	return 0;
}

void ast_usbhub_exit(uint8_t dev_num)
{
	dev_num = ast_usbhub_convert_dev_to_port(dev_num);
	if (dev_num == -1)
		return;
	ast_usbhub_dev_cfg[dev_num].dev_num = 0xFF;
}

static void ast_usbhub_init_and_connect(void)
{
	uint32_t reg;
#ifdef CONFIG_SPX_FEATURE_POWER_CONSUMPTION_SUPPORT
	if(IsUsb_Upstream_host  == 0){
		return;
		}
#endif
	/* allocate buffer for hub ep0 */
	ast_usbhub_hub_ep0_buf = dma_alloc_coherent(NULL, AST_USBHUB_EP0_BUF_SIZE, &ast_usbhub_hub_ep0_buf_dma, GFP_KERNEL);
	if (ast_usbhub_hub_ep0_buf == NULL) {
		printk(KERN_ERR "%s: allocate hub ep0 buffer failed.\n", ast_usbhub_driver_name);
	}

	/* TODO 64-bit boundary */

	/* hardware works with bus address */
	ast_usbhub_write_reg(ast_usbhub_hub_ep0_buf_dma, AST_USBHUB_HUB_EP0_BUF);

	/* enable hub ep1(status change endpoint) */
	reg = AST_USBHUB_HUB_EP1_RESET_TOGGLE | AST_USBHUB_HUB_EP1_CTRL;
	ast_usbhub_write_reg(reg, AST_USBHUB_HUB_EP1_CSR);

	/* enable interrupt(s) */
	reg = AST_USBHUB_IRQ_EP0_SETUP_ACK | AST_USBHUB_IRQ_EP0_OUT_ACK | AST_USBHUB_IRQ_EP0_IN_ACK |
			AST_USBHUB_IRQ_EP1_IN_ACK | AST_USBHUB_IRQ_EP_POOL_ACK |
			AST_USBHUB_IRQ_BUS_RESET | AST_USBHUB_IRQ_SUSPEND | AST_USBHUB_IRQ_RESUME;
	ast_usbhub_write_reg(reg, AST_USBHUB_ICR);

	/* stop clock in suspend state */
	/* set pulse width of remote wakeup signal */
	reg = ast_usbhub_read_reg(AST_USBHUB_ROOT) | AST_USBHUB_ROOT_SUSPEND_CLK_STOP | AST_USBHUB_ROOT_REMOTE_WAKE_WIDTH;
	ast_usbhub_write_reg(reg, AST_USBHUB_ROOT);

	/* connect upstream port to host */
	reg = ast_usbhub_read_reg(AST_USBHUB_ROOT) | AST_USBHUB_ROOT_UPSTREAM_PORT_CONNECT;
	ast_usbhub_write_reg(reg, AST_USBHUB_ROOT);
#ifdef CONFIG_SPX_FEATURE_POWER_CONSUMPTION_SUPPORT
	IsUsb_Upstream_host = 1;
#endif

}

static void ast_usbhub_clear_and_disconnect(void)
{
	uint32_t reg;
#ifdef CONFIG_SPX_FEATURE_POWER_CONSUMPTION_SUPPORT
	if(IsUsb_Upstream_host  == 0){
		return;
		}
#endif

	/* disconnect upstream port from host */
	reg = ast_usbhub_read_reg(AST_USBHUB_ROOT) & ~AST_USBHUB_ROOT_UPSTREAM_PORT_CONNECT;
	ast_usbhub_write_reg(reg, AST_USBHUB_ROOT);

	/* disable interrupt(s) */
	ast_usbhub_write_reg(0, AST_USBHUB_ICR);

	/* disable hub ep1 */
	ast_usbhub_write_reg(0, AST_USBHUB_HUB_EP1_CSR);

	dma_free_coherent(NULL, AST_USBHUB_EP0_BUF_SIZE, ast_usbhub_hub_ep0_buf, ast_usbhub_hub_ep0_buf_dma);

	/* disable UDC PHY clock generation for power saving */
	reg = ast_usbhub_read_reg(AST_USBHUB_ROOT) & ~AST_USBHUB_ROOT_PHY_CLK;
	ast_usbhub_write_reg(reg, AST_USBHUB_ROOT);	
#ifdef CONFIG_SPX_FEATURE_POWER_CONSUMPTION_SUPPORT
	IsUsb_Upstream_host = 0;
#endif
}

static int ast_usbhub_global_hw_reset(void)
{
	void *scu_virt_base;
	uint32_t reg;
	int ret;

	ret = 0;

	if (!request_mem_region(AST_SCU_REG_BASE, AST_SCU_REG_SIZE, "ast_scu")) {
		return -EBUSY;
	}

	scu_virt_base = ioremap(AST_SCU_REG_BASE, AST_SCU_REG_SIZE);
	if (!scu_virt_base) {
		ret = -ENOMEM;
		goto out_mem_region;
	}

	/*
	   According to AST2100 spec, the UDC reset procedure is :
	   1. Enable UDC global reset by setting SCU_04[14] = 1 (also enable UDC PHY reset)
	   2. Enable UDC clock running by setting SCU_0C[14] = 1
	   3. Waiting 3 ms for clock stable
	   4. Disable UDC global reset by setting SCU_04[14] = 0
	   5. Disable UDC PHY reset by setting UDC_HUB_00[11] = 1
	 */
	iowrite32(AST_SCU_UNLOCK_MAGIC, (void * __iomem)scu_virt_base + AST_SCU_LOCK_KEY); /* unlock SCU */
	if (ioread32((void * __iomem)scu_virt_base) == 0x00000001) { /* SCU is under unlocked state */
		reg = ioread32((void * __iomem)scu_virt_base + AST_SCU_RESET_CTRL) | AST_SCU_RESET_CTRL_UDC;
		iowrite32(reg, (void * __iomem)scu_virt_base + AST_SCU_RESET_CTRL);

		reg = ioread32((void * __iomem)scu_virt_base + AST_SCU_CLK_STOP) | AST_SCU_CLK_STOP_UDC;
		iowrite32(reg, (void * __iomem)scu_virt_base + AST_SCU_CLK_STOP);

		mdelay(10); /* for clock stable */

		/* disable UDC global reset */
		reg = ioread32((void * __iomem)scu_virt_base + AST_SCU_RESET_CTRL) & ~AST_SCU_RESET_CTRL_UDC;
		iowrite32(reg, (void * __iomem)scu_virt_base + AST_SCU_RESET_CTRL);
	} else { /* unlock SCU failed */
		ret = -EIO;
		goto out_iomap;
	}
	iowrite32(0, (void * __iomem)scu_virt_base + AST_SCU_LOCK_KEY); /* lock SCU */

	/* disable UDC PHY reset */
	reg = ast_usbhub_read_reg(AST_USBHUB_ROOT) | AST_USBHUB_ROOT_USB_PHY_RESET;
	ast_usbhub_write_reg(reg, AST_USBHUB_ROOT);

	/* clear all interrupt status */
	ast_usbhub_write_reg(0xFFFFFFFF, AST_USBHUB_ISR);
	ast_usbhub_write_reg(0xFFFFFFFF, AST_USBHUB_EP_ACK_ISR);
	ast_usbhub_write_reg(0xFFFFFFFF, AST_USBHUB_EP_NAK_ISR);

	/* reset device 1-7 and ep pool */
	ast_usbhub_write_reg(0x000002FE, AST_USBHUB_DEV_RESET);
	ast_usbhub_write_reg(0, AST_USBHUB_DEV_RESET);

out_iomap:
	iounmap(scu_virt_base);
out_mem_region:
	release_mem_region(AST_SCU_REG_BASE, AST_SCU_REG_SIZE);

	return ret;
}

int ast_usbhub_module_init(void)
{
	int i;
	int ret;

	if (get_usb_core_funcs(&usb_core_module) != 0) {
		printk(KERN_ERR "%s: get core module functions failed\n", ast_usbhub_driver_name);
		return -1;
	}

	if (!request_mem_region(AST_USBHUB_REG_BASE, AST_USBHUB_REG_SIZE, ast_usbhub_driver_name)) {
		printk(KERN_ERR "%s: Request memory region failed.\n", ast_usbhub_driver_name);
		return -EBUSY;
	}

	ast_usbhub_virt_base = ioremap(AST_USBHUB_REG_BASE, AST_USBHUB_REG_SIZE);
	if (ast_usbhub_virt_base == NULL) {
		printk(KERN_ERR "%s: Remap I/O memory failed.\n", ast_usbhub_driver_name);
		ret = -ENOMEM;
		goto out_mem_region;
	}

	ret = ast_usbhub_global_hw_reset();
	if (ret < 0) {
		printk(KERN_ERR "%s: global hardware reset failed.\n", ast_usbhub_driver_name);
		goto out_iomap;
	}

	IRQ_SET_LEVEL_TRIGGER(AST_USBHUB_IRQ);
	IRQ_SET_HIGH_LEVEL(AST_USBHUB_IRQ);

	ret = request_irq(AST_USBHUB_IRQ, ast_usbhub_irq_handler, IRQF_DISABLED, ast_usbhub_driver_name, NULL);
	if (ret != 0) {
		printk(KERN_ERR "%s: Request IRQ %d failed.\n", ast_usbhub_driver_name, AST_USBHUB_IRQ);
		goto out_iomap;
	}

	ast_usbhub_hub_dev_addr = 0x00;
	ast_usbhub_hub_current_config_value = 0;
	ast_usbhub_hub_usb_status = (1 << USB_DEVICE_SELF_POWERED);
	ast_usbhub_hub_class_status.wHubStatus = 0; /* Local power supply is good, no over-current */
	ast_usbhub_hub_class_status.wHubChange = 0; /* No change has occurred */
	ast_usbhub_hub_speed = USB_SPEED_FULL;
	ast_usbhub_remote_wakeup_enable = 0;

	for (i = 0; i < AST_USBHUB_DEV_NUM; i ++) {
		ast_usbhub_dev_addr[i] = 0x00;
		ast_usbhub_dev_set_addr_req[i] = 0;
		ast_usbhub_dev_tx_ready[i] = 0;
		ast_usbhub_device_connect_to_port[i] = 0;
		ast_usbhub_device_enabled[i] = 0;
		ast_usbhub_port_status[i].wPortStatus = 0;
		ast_usbhub_port_status[i].wPortChange = 0;
		ast_usbhub_dev_speed[i] = SUPPORT_FULL_SPEED;
		ast_usbhub_dev_run_speed[i] = SUPPORT_FULL_SPEED;
		ast_usbhub_dev_cfg[i].dev_num = 0xFF; /* 0xFF means unconfigured/not used */

		sprintf(ast_usbhub_driver[i].name, "ast-usbhub-dev-%d", i + 1);
		ast_usbhub_driver[i].module = THIS_MODULE;
		ast_usbhub_driver[i].usb_driver_init = ast_usbhub_init;
		ast_usbhub_driver[i].usb_driver_exit = ast_usbhub_exit;
		ast_usbhub_driver[i].devnum = 0xFF;
	}

	for (i = 0; i < AST_USBHUB_USE_DEV_NUM; i ++) {
		ret = register_usb_chip_driver_module(&(ast_usbhub_driver[i]));
		if (ret < 0) {
			printk("%s: register dev %d to usb core failed\n", ast_usbhub_driver_name, i);
			ret = i;
			break;
		}
	}

	if (i == AST_USBHUB_USE_DEV_NUM) { /* all device register successful */
		ast_usbhub_alloc_dma_buf();
		ast_usbhub_init_and_connect();
		ast_usbhub_sys = AddSysctlTable("ast-usbhub", ast_usbhub_ctl_table);
		return 0;
	} else { /* register failed, ret is the failed device */
		for (i = 0; i < ret; i ++) {
			unregister_usb_chip_driver_module(&(ast_usbhub_driver[i]));
		}
		ret = -EBUSY;
	}

	free_irq(AST_USBHUB_IRQ, NULL);
out_iomap:
	iounmap(ast_usbhub_virt_base);
out_mem_region:
	release_mem_region(AST_USBHUB_REG_BASE, AST_USBHUB_REG_SIZE);

	return ret;
}

void ast_usbhub_module_exit(void)
{
	int i;

	RemoveSysctlTable(ast_usbhub_sys);

	for (i = 0; i < AST_USBHUB_USE_DEV_NUM; i ++)
		unregister_usb_chip_driver_module(&(ast_usbhub_driver[i]));

	free_irq(AST_USBHUB_IRQ, NULL);

	ast_usbhub_clear_and_disconnect();
	ast_usbhub_free_dma_buf();
	iounmap(ast_usbhub_virt_base);
	release_mem_region(AST_USBHUB_REG_BASE, AST_USBHUB_REG_SIZE);
}

module_init(ast_usbhub_module_init);
module_exit(ast_usbhub_module_exit);

MODULE_AUTHOR("American Megatrends Inc.");
MODULE_DESCRIPTION("AST2100/2050/2200/2150/2300/2400 virtual USB hub controller driver");
MODULE_LICENSE("GPL");
