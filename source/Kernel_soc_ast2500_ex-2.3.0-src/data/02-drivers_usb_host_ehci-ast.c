--- linux/drivers/usb/host/ehci-ast.c	1970-01-01 08:00:00.000000000 +0800
+++ linux.new/drivers/usb/host/ehci-ast.c	2015-01-27 12:26:23.454529773 +0800
@@ -0,0 +1,325 @@
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
+#define AST_SCU_LOCK_KEY         0x00
+#define AST_SCU_UNLOCK_MAGIC     0x1688A8A8
+
+//SCU_SYS_RESET_REG
+#define USB2_HOST_1_RESET_BIT    (1 << 14)
+#define USB2_HOST_2_RESET_BIT    (1 << 3)
+
+//SCU_CLK_STOP_REG
+#define USB2_HOST_1_CLOCK_BIT    (1 << 14)
+#define USB2_HOST_2_CLOCK_BIT    (1 << 7)
+
+//SCU_FUN_PIN_CTRL5
+#define SCU_FUN_PIN_CTRL5		 (AST_SCU_VA_BASE + 0x90)
+#define USB2_HOST_1_PORTS        (1 << 29)
+
+//SCU_FUN_PIN_CTRL6
+#define SCU_FUN_PIN_CTRL6		 (AST_SCU_VA_BASE + 0x94)
+
+/*****************************************************************************
+ * EHCI
+ ****************************************************************************/
+static u64 ehci_dmamask = 0xffffffff;
+
+static struct resource aspeed_ehci1_resources[] = {
+	{
+		.start	= AST_USB20_1_BASE,
+		.end	= AST_USB20_1_BASE + SZ_4K - 1,
+		.flags	= IORESOURCE_MEM,
+	}, {
+		.start	= IRQ_USB20,
+		.end	= IRQ_USB20,
+		.flags	= IORESOURCE_IRQ,
+	},
+};
+
+static struct platform_device __maybe_unused aspeed_ehci1 = {
+	.name		= "aspeed-ehci",
+	.id		= 0,
+	.dev		= {
+		.dma_mask		= &ehci_dmamask,
+		.coherent_dma_mask	= 0xffffffff,
+	},
+	.resource	= aspeed_ehci1_resources,
+	.num_resources	= ARRAY_SIZE(aspeed_ehci1_resources),
+};
+
+static struct resource aspeed_ehci2_resources[] = {
+	{
+		.start	= AST_USB20_2_BASE,
+		.end	= AST_USB20_2_BASE + SZ_4K - 1,
+		.flags	= IORESOURCE_MEM,
+	}, {
+		.start	= IRQ_USB20_2,
+		.end	= IRQ_USB20_2,
+		.flags	= IORESOURCE_IRQ,
+	},
+};
+
+static struct platform_device __maybe_unused aspeed_ehci2 = {
+	.name		= "aspeed-ehci",
+	.id		= 1,
+	.dev		= {
+		.dma_mask		= &ehci_dmamask,
+		.coherent_dma_mask	= 0xffffffff,
+	},
+	.resource	= aspeed_ehci2_resources,
+	.num_resources	= ARRAY_SIZE(aspeed_ehci2_resources),
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
+	iowrite32(AST_SCU_UNLOCK_MAGIC, (void * __iomem)(AST_SCU_VA_BASE + AST_SCU_LOCK_KEY));
+	local_irq_save(flags);
+
+#ifdef CONFIG_SPX_FEATURE_GLOBAL_EHCI1_USB_HOST_CONTROLLER
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
+#ifdef CONFIG_SPX_FEATURE_GLOBAL_EHCI2_USB_HOST_CONTROLLER
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
+	local_irq_restore(flags);
+	udelay(500);
+
+	/* Lock SCU */
+	iowrite32(0, (void * __iomem)(AST_SCU_VA_BASE + AST_SCU_LOCK_KEY));
+}
+
+static int ehci_aspeed_setup(struct usb_hcd *hcd)
+{
+	struct ehci_hcd *ehci = hcd_to_ehci(hcd);
+	int retval;
+
+	retval = ehci_halt(ehci);
+	if (retval)
+		return retval;
+
+	/* Data structure init */
+	retval = ehci_init(hcd);
+	if (retval)
+		return retval;
+
+	hcd->has_tt = 0;
+
+	ehci_reset(ehci);
+	//ehci_port_power(ehci, 0);
+
+	return retval;
+}
+
+static const struct hc_driver ehci_aspeed_hc_driver = {
+	.description = hcd_name,
+	.product_desc = "ASPEED EHCI",
+	.hcd_priv_size = sizeof(struct ehci_hcd),
+
+	/* Generic hardware linkage */
+	.irq = ehci_irq,
+	.flags = HCD_MEMORY | HCD_USB2,
+
+	/* Basic lifecycle operations */
+	.reset = ehci_aspeed_setup,
+	.start = ehci_run,
+	.stop = ehci_stop,
+	.shutdown = ehci_shutdown,
+
+	/* Managing i/o requests and associated device resources */
+	.urb_enqueue = ehci_urb_enqueue,
+	.urb_dequeue = ehci_urb_dequeue,
+	.endpoint_disable = ehci_endpoint_disable,
+
+	/* Scheduling support */
+	.get_frame_number = ehci_get_frame,
+
+	/* Root hub support */
+	.hub_status_data = ehci_hub_status_data,
+	.hub_control = ehci_hub_control,
+#if defined(CONFIG_PM)
+	.bus_suspend = ehci_bus_suspend,
+	.bus_resume = ehci_bus_resume,
+#endif
+	.relinquish_port = ehci_relinquish_port,
+	.port_handed_over = ehci_port_handed_over,
+};
+
+
+static int ehci_aspeed_drv_probe(struct platform_device *pdev)
+{
+	struct resource *res;
+	struct usb_hcd *hcd;
+	struct ehci_hcd *ehci;
+	void __iomem *regs;
+	int irq, err;
+
+	if (usb_disabled())
+		return -ENODEV;
+
+	pr_info("Initializing ASPEED-SoC USB Host Controller\n");
+
+	/* Setup ASPEED USB controller */
+	aspeed_usb_setup();
+        
+	irq = platform_get_irq(pdev, 0);
+	if (irq <= 0) {
+		dev_err(&pdev->dev,
+			"Found HC with no IRQ. Check %s setup!\n",
+			dev_name(&pdev->dev));
+		err = -ENODEV;
+		goto err1;
+	}
+	IRQ_SET_HIGH_LEVEL (irq);
+	IRQ_SET_LEVEL_TRIGGER (irq);
+
+	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
+	if (!res) {
+		dev_err(&pdev->dev,
+			"Found HC with no register addr. Check %s setup!\n",
+			dev_name(&pdev->dev));
+		err = -ENODEV;
+		goto err1;
+	}
+
+	if (!request_mem_region(res->start, res->end - res->start + 1,
+				ehci_aspeed_hc_driver.description)) {
+		dev_dbg(&pdev->dev, "controller already in use\n");
+		err = -EBUSY;
+		goto err1;
+	}
+
+	regs = ioremap_nocache(res->start, res->end - res->start + 1);
+	if (regs == NULL) {
+		dev_dbg(&pdev->dev, "error mapping memory\n");
+		err = -EFAULT;
+		goto err2;
+	}
+
+	hcd = usb_create_hcd(&ehci_aspeed_hc_driver,
+			&pdev->dev, dev_name(&pdev->dev));
+       
+	if (!hcd) {
+		err = -ENOMEM;
+		goto err3;
+	}
+
+	hcd->rsrc_start = res->start;
+	hcd->rsrc_len = res->end - res->start + 1;
+	hcd->regs = regs;
+
+	ehci = hcd_to_ehci(hcd);
+	ehci->caps = hcd->regs;
+	ehci->regs = hcd->regs +
+	HC_LENGTH(ehci, ehci_readl(ehci, &ehci->caps->hc_capbase));
+
+	ehci->hcs_params = ehci_readl(ehci, &ehci->caps->hcs_params);
+	hcd->has_tt = 0;
+	ehci->sbrn = 0x20; //USB Bus revision number.
+
+	err = usb_add_hcd(hcd,irq, IRQF_DISABLED);
+	if (err)	
+		goto err4;
+	return 0;
+
+err4:
+	usb_put_hcd(hcd);
+err3:
+	iounmap(regs);
+err2:
+	release_mem_region(res->start, res->end - res->start + 1);
+err1:
+	dev_err(&pdev->dev, "init %s fail, %d\n",
+		dev_name(&pdev->dev), err);
+
+	return err;
+}
+
+static int __exit ehci_aspeed_drv_remove(struct platform_device *pdev)
+{
+	struct usb_hcd *hcd = platform_get_drvdata(pdev);
+
+	usb_remove_hcd(hcd);
+	iounmap(hcd->regs);
+	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
+	usb_put_hcd(hcd);
+
+	return 0;
+}
+
+MODULE_ALIAS("platform:aspeed-ehci");
+
+static struct platform_driver ehci_aspeed_driver = {
+	.probe		= ehci_aspeed_drv_probe,
+	.remove		= ehci_aspeed_drv_remove,
+	.driver.name	= "aspeed-ehci",
+};
+
