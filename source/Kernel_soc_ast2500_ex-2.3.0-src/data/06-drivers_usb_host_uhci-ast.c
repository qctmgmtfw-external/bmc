--- linux/drivers/usb/host/uhci-ast.c	1970-01-01 08:00:00.000000000 +0800
+++ linux.new/drivers/usb/host/uhci-ast.c	2015-01-23 00:43:23.587871131 +0800
@@ -0,0 +1,299 @@
+/*
+ * usbhost setup code used by AST SoC
+ *
+ * This file is licensed under the terms of the GNU General Public
+ * License version 2.  This program is licensed "as is" without any
+ * warranty of any kind, whether express or implied.
+ */
+
+#include <linux/kernel.h>
+#include <linux/init.h>
+#include <linux/platform_device.h>
+
+#define AST_SCU_LOCK_KEY        0x00
+#define AST_SCU_UNLOCK_MAGIC    0x1688A8A8
+
+//SCU_SYS_RESET_REG
+#define USB2_HOST_1_RESET_BIT    (1 << 14)
+#define USB2_HOST_2_RESET_BIT    (1 << 3)
+
+//SCU_CLK_STOP_REG
+#define USB2_HOST_1_CLOCK_BIT    (1 << 14)
+#define USB2_HOST_2_CLOCK_BIT    (1 << 7)
+#define USB11_HOST_RESET_BIT     (1 << 15)
+#define USB11_HOST_CLOCK_BIT     (1 << 9)
+
+//SCU_FUN_PIN_CTRL5
+#define SCU_FUN_PIN_CTRL5		 (AST_SCU_VA_BASE + 0x90)
+#define USB2_HOST_1_PORTS        (1 << 29)
+
+//SCU_FUN_PIN_CTRL6
+#define SCU_FUN_PIN_CTRL6		 (AST_SCU_VA_BASE + 0x94)
+
+/*****************************************************************************
+ * UHCI
+ ****************************************************************************/
+static u64 uhci_dmamask = 0xffffffff;
+
+static struct resource aspeed_uhci_resources[] = {
+	[0] = {
+		.start = AST_USB11_BASE,
+		.end   = AST_USB11_BASE + SZ_64K - 1,
+		.flags = IORESOURCE_MEM,
+	},
+	[1] = {
+		.start = IRQ_USB11,
+		.end   = IRQ_USB11,
+		.flags = IORESOURCE_IRQ,
+	}
+};
+
+static struct platform_device __maybe_unused aspeed_uhci = {
+	.name = "ast_uhci_hc",
+	.num_resources = ARRAY_SIZE(aspeed_uhci_resources),
+	.resource = aspeed_uhci_resources,
+	.dev = {
+		.dma_mask = &uhci_dmamask,
+		.coherent_dma_mask = 0xffffffffUL
+	}
+};
+
+/*
+ * Implement ASPEED EHCI USB controller specification guidelines
+ */
+static void aspeed_usb_setup(void)
+{
+	unsigned long flags;
+	uint32_t __maybe_unused reg;
+
+	/* Unlock SCU */
+	iowrite32(AST_SCU_UNLOCK_MAGIC, (void * __iomem)(AST_SCU_VA_BASE + AST_SCU_LOCK_KEY)); /* Unlock SCU */
+	local_irq_save(flags);
+
+#if defined(CONFIG_SPX_FEATURE_GLOBAL_UHCI1_USB_HOST_CONTROLLER) && !defined(CONFIG_SPX_FEATURE_GLOBAL_EHCI1_USB_HOST_CONTROLLER)
+	/* Set SCU90[29] to Enable USB2 HOST Port1 controller */
+	reg = ioread32((void * __iomem)SCU_FUN_PIN_CTRL5);
+	reg |= (USB2_HOST_1_PORTS);
+	iowrite32(reg, (void * __iomem)SCU_FUN_PIN_CTRL5);
+	wmb();
+
+	/* Enable reset of USB20 */
+	reg = ioread32((void * __iomem)SCU_SYS_RESET_REG);
+	reg |= USB2_HOST_1_RESET_BIT; 
+	iowrite32(reg, (void * __iomem)SCU_SYS_RESET_REG);
+	wmb();
+
+	/* Enable USB20 clock */
+	reg = ioread32((void * __iomem)SCU_CLK_STOP_REG);
+	reg |= USB2_HOST_1_CLOCK_BIT;
+	iowrite32(reg, (void * __iomem)SCU_CLK_STOP_REG);
+
+	local_irq_restore(flags);
+	msleep(10);
+	local_irq_save(flags);
+
+	/* Disable reset of USB20 */
+	reg = ioread32((void * __iomem)SCU_SYS_RESET_REG);
+	reg &= ~USB2_HOST_1_RESET_BIT;
+	iowrite32(reg, (void * __iomem)SCU_SYS_RESET_REG);
+	wmb();
+#endif
+
+#if defined(CONFIG_SPX_FEATURE_GLOBAL_UHCI2_USB_HOST_CONTROLLER) && !defined(CONFIG_SPX_FEATURE_GLOBAL_EHCI2_USB_HOST_CONTROLLER)
+	/* Set SCU94[13:14] to Enable USB2 HOST Port2 controller */
+	reg = ioread32((void * __iomem)SCU_FUN_PIN_CTRL6);
+	reg &= ~(3 << 13);
+	reg |= (2 << 13);
+	iowrite32(reg, (void * __iomem)SCU_FUN_PIN_CTRL6);
+	wmb();
+
+	/* Enable reset of USB20 */
+	reg = ioread32((void * __iomem)SCU_SYS_RESET_REG);
+	reg |= USB2_HOST_2_RESET_BIT; 
+	iowrite32(reg, (void * __iomem)SCU_SYS_RESET_REG);
+	wmb();
+
+	/* Enable USB20 clock */
+	reg = ioread32((void * __iomem)SCU_CLK_STOP_REG);
+	reg &= ~USB2_HOST_2_CLOCK_BIT;
+	iowrite32(reg, (void * __iomem)SCU_CLK_STOP_REG);
+
+	local_irq_restore(flags);
+	msleep(10);
+	local_irq_save(flags);
+
+	/* Disable reset of USB20 */
+	reg = ioread32((void * __iomem)SCU_SYS_RESET_REG);
+	reg &= ~USB2_HOST_2_RESET_BIT;
+	iowrite32(reg, (void * __iomem)SCU_SYS_RESET_REG);
+	wmb();
+#endif
+
+#ifdef CONFIG_SPX_FEATURE_GLOBAL_UHCI2_USB_HOST_CONTROLLER
+	/* Enable reset USB host controller */
+	reg = ioread32((void * __iomem)(SCU_SYS_RESET_REG));
+	reg |= USB11_HOST_RESET_BIT;
+	iowrite32(reg, (void * __iomem)(SCU_SYS_RESET_REG));
+
+	/* Enable clock of USB host controller */
+	reg = ioread32((void * __iomem)(SCU_CLK_STOP_REG));
+	reg &= ~(USB11_HOST_CLOCK_BIT);
+	iowrite32(reg, (void * __iomem)(SCU_CLK_STOP_REG));
+
+	mdelay(10);
+
+	/* Disable reset USB host controller */
+	reg = ioread32((void * __iomem)(SCU_SYS_RESET_REG));
+	reg &= ~(USB11_HOST_RESET_BIT);
+	iowrite32(reg, (void * __iomem)(SCU_SYS_RESET_REG));
+#endif
+
+	local_irq_restore(flags);
+	udelay(500);
+	/* Lock SCU */
+	iowrite32(0, (void * __iomem)(AST_SCU_VA_BASE + AST_SCU_LOCK_KEY)); /* Lock SCU */
+
+}
+
+static int uhci_ast_init(struct usb_hcd *hcd)
+{
+	struct uhci_hcd *uhci = hcd_to_uhci(hcd);
+
+	/* Get port count */
+	uhci->rh_numports = uhci_count_ports(hcd);
+
+	/* Set up pointers to generic functions */
+	uhci->reset_hc = uhci_generic_reset_hc;
+	uhci->check_and_reset_hc = uhci_generic_check_and_reset_hc;
+
+	/* No special actions need to be taken for the functions below */
+	uhci->configure_hc = NULL;
+	uhci->resume_detect_interrupts_are_broken = NULL;
+	uhci->global_suspend_mode_is_broken = NULL;
+
+	check_and_reset_hc(uhci);
+	return 0;
+}
+
+void uhci_ast_stop_hc(struct usb_hcd *hcd)
+{
+	struct uhci_hcd *uhci = hcd_to_uhci(hcd);
+
+	iowrite32(0, (uhci->regs + USBINTR)); // Disable all interrupts
+	iowrite32(USBCMD_MAXP, (uhci->regs + USBCMD)); // disable hc
+}
+
+static const struct hc_driver uhci_driver = {
+	.description =		hcd_name,
+	.product_desc =		"UHCI Host Controller",
+	.hcd_priv_size =	sizeof(struct uhci_hcd),
+
+	/* Generic hardware linkage */
+	.irq =			uhci_irq,
+	.flags =		HCD_USB11 | HCD_MEMORY,
+
+	/* Basic lifecycle operations */
+	.reset =		uhci_ast_init,
+	.start =		uhci_start,
+#ifdef CONFIG_PM
+	.pci_suspend =		uhci_pci_suspend,
+	.pci_resume =		uhci_pci_resume,
+	.bus_suspend =		uhci_rh_suspend,
+	.bus_resume =		uhci_rh_resume,
+#endif
+	.stop =			uhci_stop,
+
+	.urb_enqueue =		uhci_urb_enqueue,
+	.urb_dequeue =		uhci_urb_dequeue,
+
+	.endpoint_disable =	uhci_hcd_endpoint_disable,
+	.get_frame_number =	uhci_hcd_get_frame_number,
+
+	.hub_status_data =	uhci_hub_status_data,
+	.hub_control =		uhci_hub_control,
+};
+
+static int ast_uhci_hc_drv_probe(struct platform_device *pdev)
+{
+	struct usb_hcd *hcd = NULL;
+	struct uhci_hcd *uhci = NULL;
+	int retval;
+
+	if (usb_disabled())
+		return -ENODEV;
+
+	aspeed_usb_setup();
+
+	hcd = usb_create_hcd(&uhci_driver, &pdev->dev, hcd_name);
+	if (hcd == NULL) {
+		return -ENOMEM;
+	}
+
+	hcd->rsrc_start = pdev->resource[0].start;
+	hcd->rsrc_len = pdev->resource[0].end - pdev->resource[0].start + 1;
+
+	if (!request_mem_region(hcd->rsrc_start, hcd->rsrc_len, hcd_name)) {
+		dev_err(&pdev->dev, "request_mem_region failed\n");
+		retval = -EBUSY;
+		goto err_put;
+	}
+
+	hcd->regs = ioremap(hcd->rsrc_start, hcd->rsrc_len);
+	if (hcd->regs == NULL) {
+		dev_err(&pdev->dev, "ioremap failed\n");
+		retval = -ENOMEM;
+		goto err_mem;
+	}
+
+	uhci = hcd_to_uhci(hcd);
+	uhci->regs = hcd->regs;
+
+	retval = usb_add_hcd(hcd, pdev->resource[1].start, IRQF_SHARED);
+	if (retval != 0) {
+		pr_err("add UHCI to USB host controller list failed!\n");
+		goto err_ioremap;
+	}
+
+	IRQ_SET_HIGH_LEVEL(IRQ_USB11);
+	IRQ_SET_LEVEL_TRIGGER(IRQ_USB11);
+
+	iowrite32(0x01, hcd->regs + 0x94); //For frame timing setting
+
+	return 0;
+
+err_ioremap:
+	uhci_ast_stop_hc(hcd);
+	iounmap(hcd->regs);
+
+err_mem:
+	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
+
+err_put:
+	usb_put_hcd(hcd);
+
+	return retval;
+}
+
+static int ast_uhci_hc_drv_remove(struct platform_device *pdev)
+{
+	struct usb_hcd *hcd = platform_get_drvdata(pdev);
+
+	usb_remove_hcd(hcd);
+	uhci_ast_stop_hc(hcd);
+	iounmap(hcd->regs);
+	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
+	usb_put_hcd(hcd);
+
+	return 0;
+}
+
+static struct platform_driver uhci_aspeed_driver = {
+	.probe = ast_uhci_hc_drv_probe,
+	.remove = ast_uhci_hc_drv_remove,
+	.shutdown	= usb_hcd_platform_shutdown,
+	.driver = {
+		.owner = THIS_MODULE,
+		.name = "ast_uhci_hc",
+	},
+};
+
