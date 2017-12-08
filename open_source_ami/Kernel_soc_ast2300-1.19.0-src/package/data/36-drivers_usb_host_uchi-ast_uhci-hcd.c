--- linux-2.6.28.10/drivers/usb/host/uhci-ast/uhci-hcd.c	1970-01-01 08:00:00.000000000 +0800
+++ linux-2.6.28.10-ami/drivers/usb/host/uhci-ast/uhci-hcd.c	2011-03-17 16:37:44.000000000 +0800
@@ -0,0 +1,1010 @@
+/*
+ * Universal Host Controller Interface driver for USB.
+ *
+ * Maintainer: Alan Stern <stern@rowland.harvard.edu>
+ *
+ * (C) Copyright 1999 Linus Torvalds
+ * (C) Copyright 1999-2002 Johannes Erdfelt, johannes@erdfelt.com
+ * (C) Copyright 1999 Randy Dunlap
+ * (C) Copyright 1999 Georg Acher, acher@in.tum.de
+ * (C) Copyright 1999 Deti Fliegl, deti@fliegl.de
+ * (C) Copyright 1999 Thomas Sailer, sailer@ife.ee.ethz.ch
+ * (C) Copyright 1999 Roman Weissgaerber, weissg@vienna.at
+ * (C) Copyright 2000 Yggdrasil Computing, Inc. (port of new PCI interface
+ *               support from usb-ohci.c by Adam Richter, adam@yggdrasil.com).
+ * (C) Copyright 1999 Gregory P. Smith (from usb-ohci.c)
+ * (C) Copyright 2004-2007 Alan Stern, stern@rowland.harvard.edu
+ *
+ * Intel documents this fairly well, and as far as I know there
+ * are no royalties or anything like that, but even so there are
+ * people who decided that they want to do the same thing in a
+ * completely different way.
+ *
+ */
+
+#include <linux/module.h>
+#include <linux/pci.h>
+#include <linux/kernel.h>
+#include <linux/init.h>
+#include <linux/delay.h>
+#include <linux/ioport.h>
+#include <linux/slab.h>
+#include <linux/errno.h>
+#include <linux/unistd.h>
+#include <linux/interrupt.h>
+#include <linux/spinlock.h>
+#include <linux/debugfs.h>
+#include <linux/pm.h>
+#include <linux/dmapool.h>
+#include <linux/dma-mapping.h>
+#include <linux/usb.h>
+#include <linux/bitops.h>
+#include <linux/dmi.h>
+#include <linux/platform_device.h>
+
+#include <asm/uaccess.h>
+#include <asm/io.h>
+#include <asm/irq.h>
+#include <asm/system.h>
+
+#include <mach/platform.h>
+#include "../../core/hcd.h"
+#include "uhci-hcd.h"
+
+/*
+ * Version Information
+ */
+#define DRIVER_AUTHOR "Linus 'Frodo Rabbit' Torvalds, Johannes Erdfelt, \
+Randy Dunlap, Georg Acher, Deti Fliegl, Thomas Sailer, Roman Weissgaerber, \
+Alan Stern"
+#define DRIVER_DESC "USB Universal Host Controller Interface driver"
+
+/* for flakey hardware, ignore overcurrent indicators */
+static int ignore_oc;
+module_param(ignore_oc, bool, S_IRUGO);
+MODULE_PARM_DESC(ignore_oc, "ignore hardware overcurrent indications");
+
+/*
+ * debug = 0, no debugging messages
+ * debug = 1, dump failed URBs except for stalls
+ * debug = 2, dump all failed URBs (including stalls)
+ *            show all queues in /debug/uhci/[pci_addr]
+ * debug = 3, show all TDs in URBs when dumping
+ */
+#ifdef DEBUG
+#define DEBUG_CONFIGURED	1
+static int debug = 1;
+module_param(debug, int, S_IRUGO | S_IWUSR);
+MODULE_PARM_DESC(debug, "Debug level");
+
+#else
+#define DEBUG_CONFIGURED	0
+#define debug			0
+#endif
+
+static char *errbuf;
+#define ERRBUF_LEN    (32 * 1024)
+
+#define AST_UHC_REG_ADDR		0x1E6B0000
+#define AST_UHC_REG_SZ			SZ_4K
+#define AST_UHC_IRQ				14
+#define USB11HOST_RESET_BIT		(1 << 15)
+#define USB11HOST_CLOCK_BIT		(1 << 9)
+#define USB_HOST_2_PORTS		(1 << 3)
+
+static struct kmem_cache *uhci_up_cachep;	/* urb_priv */
+
+static void suspend_rh(struct uhci_hcd *uhci, enum uhci_rh_state new_state);
+static void wakeup_rh(struct uhci_hcd *uhci);
+static void uhci_get_current_frame_number(struct uhci_hcd *uhci);
+
+/*
+ * Calculate the link pointer DMA value for the first Skeleton QH in a frame.
+ */
+static __le32 uhci_frame_skel_link(struct uhci_hcd *uhci, int frame)
+{
+	int skelnum;
+
+	/*
+	 * The interrupt queues will be interleaved as evenly as possible.
+	 * There's not much to be done about period-1 interrupts; they have
+	 * to occur in every frame.  But we can schedule period-2 interrupts
+	 * in odd-numbered frames, period-4 interrupts in frames congruent
+	 * to 2 (mod 4), and so on.  This way each frame only has two
+	 * interrupt QHs, which will help spread out bandwidth utilization.
+	 *
+	 * ffs (Find First bit Set) does exactly what we need:
+	 * 1,3,5,...  => ffs = 0 => use period-2 QH = skelqh[8],
+	 * 2,6,10,... => ffs = 1 => use period-4 QH = skelqh[7], etc.
+	 * ffs >= 7 => not on any high-period queue, so use
+	 *	period-1 QH = skelqh[9].
+	 * Add in UHCI_NUMFRAMES to insure at least one bit is set.
+	 */
+	skelnum = 8 - (int) __ffs(frame | UHCI_NUMFRAMES);
+	if (skelnum <= 1)
+		skelnum = 9;
+	return LINK_TO_QH(uhci->skelqh[skelnum]);
+}
+
+#include "uhci-debug.c"
+#include "uhci-q.c"
+#include "uhci-hub.c"
+
+/*
+ * Finish up a host controller reset and update the recorded state.
+ */
+static void finish_reset(struct uhci_hcd *uhci)
+{
+	int port;
+
+	/* HCRESET doesn't affect the Suspend, Reset, and Resume Detect
+	 * bits in the port status and control registers.
+	 * We have to clear them by hand.
+	 */
+	for (port = 0; port < uhci->rh_numports; ++port)
+		iowrite32(0, uhci->io_addr + USBPORTSC1 + (port * 4));
+
+	uhci->port_c_suspend = uhci->resuming_ports = 0;
+	uhci->rh_state = UHCI_RH_RESET;
+	uhci->is_stopped = UHCI_IS_STOPPED;
+	uhci_to_hcd(uhci)->state = HC_STATE_HALT;
+	uhci_to_hcd(uhci)->poll_rh = 0;
+
+	uhci->dead = 0;		/* Full reset resurrects the controller */
+}
+
+void uhci_reset_hc(struct uhci_hcd *uhci)
+{
+	/* Reset the HC - this will force us to get a
+	 * new notification of any already connected
+	 * ports due to the virtual disconnect that it
+	 * implies.
+	 */
+    iowrite32(0, (uhci->io_addr + USBINTR));
+    iowrite32(USBCMD_GRESET, (uhci->io_addr + USBCMD));
+
+    mdelay(50);
+
+    iowrite32(0, (uhci->io_addr + USBCMD));
+    mdelay(10);
+}
+
+int uhci_check_and_reset_hc(struct uhci_hcd *uhci)
+{
+    unsigned int cmd, intr;
+
+    cmd = ioread32((uhci->io_addr + USBCMD));
+	if ((cmd & USBCMD_RS) || !(cmd & USBCMD_CF) ||
+			!(cmd & USBCMD_EGSM)) {
+		printk("%s: cmd = 0x%04x\n", __FUNCTION__, cmd);
+		goto reset_needed;
+	}
+
+	intr = ioread32((uhci->io_addr + USBINTR));
+	if (intr & (~USBINTR_RESUME)) {
+		printk("%s: intr = 0x%04x\n", __FUNCTION__, intr);
+		goto reset_needed;
+	}
+	return 0;
+
+reset_needed:
+	printk("Performing full reset\n");
+	uhci_reset_hc(uhci);
+	return 1;
+}
+
+void uhci_stop_hc(struct usb_hcd *hcd)
+{
+	struct uhci_hcd *uhci = hcd_to_uhci(hcd);
+
+	iowrite32(0, (uhci->io_addr + USBINTR)); // Disable all interrupts
+	iowrite32(USBCMD_MAXP, (uhci->io_addr + USBCMD)); // disable hc
+}
+
+/*
+ * Last rites for a defunct/nonfunctional controller
+ * or one we don't want to use any more.
+ */
+static void uhci_hc_died(struct uhci_hcd *uhci)
+{
+	uhci_get_current_frame_number(uhci);
+	uhci_reset_hc(uhci);
+	finish_reset(uhci);
+	uhci->dead = 1;
+
+	/* The current frame may already be partway finished */
+	++uhci->frame_number;
+}
+
+/*
+ * Initialize a controller that was newly discovered or has lost power
+ * or otherwise been reset while it was suspended.  In none of these cases
+ * can we be sure of its previous state.
+ */
+static void check_and_reset_hc(struct uhci_hcd *uhci)
+{
+	if (uhci_check_and_reset_hc(uhci))
+		finish_reset(uhci);
+}
+
+/*
+ * Store the basic register settings needed by the controller.
+ */
+static void configure_hc(struct uhci_hcd *uhci)
+{
+	/* Set the frame length to the default: 1 ms exactly */
+	iowrite32(USBSOF_DEFAULT, uhci->io_addr + USBSOF);
+
+	/* Store the frame list base address */
+	iowrite32(uhci->frame_dma_handle, uhci->io_addr + USBFLBASEADD);
+
+	/* Set the current frame number */
+	iowrite32(uhci->frame_number & UHCI_MAX_SOF_NUMBER,
+			uhci->io_addr + USBFRNUM);
+
+	/* Mark controller as not halted before we enable interrupts */
+	uhci_to_hcd(uhci)->state = HC_STATE_SUSPENDED;
+	mb();
+}
+
+static void suspend_rh(struct uhci_hcd *uhci, enum uhci_rh_state new_state)
+__releases(uhci->lock)
+__acquires(uhci->lock)
+{
+	int auto_stop;
+	int int_enable, egsm_enable, wakeup_enable;
+	struct usb_device *rhdev = uhci_to_hcd(uhci)->self.root_hub;
+
+	auto_stop = (new_state == UHCI_RH_AUTO_STOPPED);
+	dev_dbg(&rhdev->dev, "%s%s\n", __func__,
+			(auto_stop ? " (auto-stop)" : ""));
+
+	/* Start off by assuming Resume-Detect interrupts and EGSM work
+	 * and that remote wakeups should be enabled.
+	 */
+	egsm_enable = USBCMD_EGSM;
+	uhci->RD_enable = 1;
+	int_enable = USBINTR_RESUME;
+	wakeup_enable = 1;
+
+	iowrite32(int_enable, uhci->io_addr + USBINTR);
+	iowrite32(USBCMD_EGSM | USBCMD_CF, uhci->io_addr + USBCMD);
+	mb();
+	udelay(5);
+
+	/* If we're auto-stopping then no devices have been attached
+	 * for a while, so there shouldn't be any active URBs and the
+	 * controller should stop after a few microseconds.  Otherwise
+	 * we will give the controller one frame to stop.
+	 */
+	if (!auto_stop && !(ioread32(uhci->io_addr + USBSTS) & USBSTS_HCH)) {
+		uhci->rh_state = UHCI_RH_SUSPENDING;
+		spin_unlock_irq(&uhci->lock);
+		msleep(1);
+		spin_lock_irq(&uhci->lock);
+		if (uhci->dead)
+			return;
+	}
+//	if (!(ioread32(uhci->io_addr + USBSTS) & USBSTS_HCH))
+//		dev_warn(uhci_dev(uhci), "Controller not stopped yet!\n");
+
+	uhci_get_current_frame_number(uhci);
+
+	uhci->rh_state = new_state;
+	uhci->is_stopped = UHCI_IS_STOPPED;
+
+	/* If interrupts don't work and remote wakeup is enabled then
+	 * the suspended root hub needs to be polled.
+	 */
+	uhci_to_hcd(uhci)->poll_rh = !int_enable;
+
+	uhci_scan_schedule(uhci);
+	uhci_fsbr_off(uhci);
+}
+
+static void start_rh(struct uhci_hcd *uhci)
+{
+	uhci_to_hcd(uhci)->state = HC_STATE_RUNNING;
+	uhci->is_stopped = 0;
+	/* Mark it configured and running with a 64-byte max packet.
+	 * All interrupts are enabled, even though RESUME won't do anything.
+	 */
+	iowrite32(USBCMD_RS | USBCMD_CF | USBCMD_MAXP, uhci->io_addr + USBCMD);
+	iowrite32(USBINTR_TIMEOUT | USBINTR_RESUME | USBINTR_IOC | USBINTR_SP,
+			uhci->io_addr + USBINTR);
+	mb();
+	uhci->rh_state = UHCI_RH_RUNNING;
+	uhci_to_hcd(uhci)->poll_rh = 1;
+}
+
+static void wakeup_rh(struct uhci_hcd *uhci)
+__releases(uhci->lock)
+__acquires(uhci->lock)
+{
+	dev_dbg(&uhci_to_hcd(uhci)->self.root_hub->dev,
+			"%s%s\n", __func__,
+			uhci->rh_state == UHCI_RH_AUTO_STOPPED ?
+				" (auto-start)" : "");
+
+	/* If we are auto-stopped then no devices are attached so there's
+	 * no need for wakeup signals.  Otherwise we send Global Resume
+	 * for 20 ms.
+	 */
+	if (uhci->rh_state == UHCI_RH_SUSPENDED) {
+		uhci->rh_state = UHCI_RH_RESUMING;
+	}
+
+	start_rh(uhci);
+
+	/* Restart root hub polling */
+	mod_timer(&uhci_to_hcd(uhci)->rh_timer, jiffies);
+}
+
+static irqreturn_t uhci_irq(struct usb_hcd *hcd)
+{
+	struct uhci_hcd *uhci = hcd_to_uhci(hcd);
+	unsigned short status;
+
+	/*
+	 * Read the interrupt status, and write it back to clear the
+	 * interrupt cause.  Contrary to the UHCI specification, the
+	 * "HC Halted" status bit is persistent: it is RO, not R/WC.
+	 */
+	status = ioread32(uhci->io_addr + USBSTS);
+	if (!(status & ~USBSTS_HCH))	/* shared interrupt, not mine */
+		return IRQ_NONE;
+	iowrite32(status, uhci->io_addr + USBSTS);		/* Clear it */
+
+	if (status & ~(USBSTS_USBINT | USBSTS_ERROR | USBSTS_RD)) {
+		if (status & USBSTS_HSE)
+			dev_err(uhci_dev(uhci), "host system error, "
+					"PCI problems?\n");
+		if (status & USBSTS_HCPE)
+			dev_err(uhci_dev(uhci), "host controller process "
+					"error, something bad happened!\n");
+		if (status & USBSTS_HCH) {
+			spin_lock(&uhci->lock);
+			if (uhci->rh_state >= UHCI_RH_RUNNING) {
+				dev_err(uhci_dev(uhci),
+					"host controller halted, "
+					"very bad!\n");
+				if (debug > 1 && errbuf) {
+					/* Print the schedule for debugging */
+					uhci_sprint_schedule(uhci,
+							errbuf, ERRBUF_LEN);
+					lprintk(errbuf);
+				}
+				uhci_hc_died(uhci);
+
+				/* Force a callback in case there are
+				 * pending unlinks */
+				mod_timer(&hcd->rh_timer, jiffies);
+			}
+			spin_unlock(&uhci->lock);
+		}
+	}
+
+	if (status & USBSTS_RD)
+		usb_hcd_poll_rh_status(hcd);
+	else {
+		spin_lock(&uhci->lock);
+		uhci_scan_schedule(uhci);
+		spin_unlock(&uhci->lock);
+	}
+
+	return IRQ_HANDLED;
+}
+
+/*
+ * Store the current frame number in uhci->frame_number if the controller
+ * is runnning.  Expand from 11 bits (of which we use only 10) to a
+ * full-sized integer.
+ *
+ * Like many other parts of the driver, this code relies on being polled
+ * more than once per second as long as the controller is running.
+ */
+static void uhci_get_current_frame_number(struct uhci_hcd *uhci)
+{
+	if (!uhci->is_stopped) {
+		unsigned delta;
+
+		delta = (ioread32(uhci->io_addr + USBFRNUM) - uhci->frame_number) &
+				(UHCI_NUMFRAMES - 1);
+		uhci->frame_number += delta;
+	}
+}
+
+/*
+ * De-allocate all resources
+ */
+static void release_uhci(struct uhci_hcd *uhci)
+{
+	int i;
+
+	if (DEBUG_CONFIGURED) {
+		spin_lock_irq(&uhci->lock);
+		uhci->is_initialized = 0;
+		spin_unlock_irq(&uhci->lock);
+
+		debugfs_remove(uhci->dentry);
+	}
+
+	for (i = 0; i < UHCI_NUM_SKELQH; i++)
+		uhci_free_qh(uhci, uhci->skelqh[i]);
+
+	uhci_free_td(uhci, uhci->term_td);
+
+	dma_pool_destroy(uhci->qh_pool);
+
+	dma_pool_destroy(uhci->td_pool);
+
+	kfree(uhci->frame_cpu);
+
+	dma_free_coherent(uhci_dev(uhci),
+			UHCI_NUMFRAMES * sizeof(*uhci->frame),
+			uhci->frame, uhci->frame_dma_handle);
+}
+
+static int uhci_init(struct usb_hcd *hcd)
+{
+	struct uhci_hcd *uhci = hcd_to_uhci(hcd);
+
+	uhci->io_addr = hcd->regs;
+
+	uhci->rh_numports = 1;
+
+	check_and_reset_hc(uhci);
+	return 0;
+}
+
+/*
+ * Allocate a frame list, and then setup the skeleton
+ *
+ * The hardware doesn't really know any difference
+ * in the queues, but the order does matter for the
+ * protocols higher up.  The order in which the queues
+ * are encountered by the hardware is:
+ *
+ *  - All isochronous events are handled before any
+ *    of the queues. We don't do that here, because
+ *    we'll create the actual TD entries on demand.
+ *  - The first queue is the high-period interrupt queue.
+ *  - The second queue is the period-1 interrupt and async
+ *    (low-speed control, full-speed control, then bulk) queue.
+ *  - The third queue is the terminating bandwidth reclamation queue,
+ *    which contains no members, loops back to itself, and is present
+ *    only when FSBR is on and there are no full-speed control or bulk QHs.
+ */
+static int uhci_start(struct usb_hcd *hcd)
+{
+	struct uhci_hcd *uhci = hcd_to_uhci(hcd);
+	int retval = -EBUSY;
+	int i;
+	struct dentry *dentry;
+
+	hcd->uses_new_polling = 1;
+
+	spin_lock_init(&uhci->lock);
+	setup_timer(&uhci->fsbr_timer, uhci_fsbr_timeout,
+			(unsigned long) uhci);
+	INIT_LIST_HEAD(&uhci->idle_qh_list);
+	init_waitqueue_head(&uhci->waitqh);
+
+	if (DEBUG_CONFIGURED) {
+		dentry = debugfs_create_file(hcd->self.bus_name,
+				S_IFREG|S_IRUGO|S_IWUSR, uhci_debugfs_root,
+				uhci, &uhci_debug_operations);
+		if (!dentry) {
+			dev_err(uhci_dev(uhci), "couldn't create uhci "
+					"debugfs entry\n");
+			retval = -ENOMEM;
+			goto err_create_debug_entry;
+		}
+		uhci->dentry = dentry;
+	}
+
+	uhci->frame = dma_alloc_coherent(uhci_dev(uhci),
+			UHCI_NUMFRAMES * sizeof(*uhci->frame),
+			&uhci->frame_dma_handle, 0);
+	if (!uhci->frame) {
+		dev_err(uhci_dev(uhci), "unable to allocate "
+				"consistent memory for frame list\n");
+		goto err_alloc_frame;
+	}
+	memset(uhci->frame, 0, UHCI_NUMFRAMES * sizeof(*uhci->frame));
+
+	uhci->frame_cpu = kcalloc(UHCI_NUMFRAMES, sizeof(*uhci->frame_cpu),
+			GFP_KERNEL);
+	if (!uhci->frame_cpu) {
+		dev_err(uhci_dev(uhci), "unable to allocate "
+				"memory for frame pointers\n");
+		goto err_alloc_frame_cpu;
+	}
+
+	uhci->td_pool = dma_pool_create("uhci_td", uhci_dev(uhci),
+			sizeof(struct uhci_td), 16, 0);
+	if (!uhci->td_pool) {
+		dev_err(uhci_dev(uhci), "unable to create td dma_pool\n");
+		goto err_create_td_pool;
+	}
+
+	uhci->qh_pool = dma_pool_create("uhci_qh", uhci_dev(uhci),
+			sizeof(struct uhci_qh), 16, 0);
+	if (!uhci->qh_pool) {
+		dev_err(uhci_dev(uhci), "unable to create qh dma_pool\n");
+		goto err_create_qh_pool;
+	}
+
+	uhci->term_td = uhci_alloc_td(uhci);
+	if (!uhci->term_td) {
+		dev_err(uhci_dev(uhci), "unable to allocate terminating TD\n");
+		goto err_alloc_term_td;
+	}
+
+	for (i = 0; i < UHCI_NUM_SKELQH; i++) {
+		uhci->skelqh[i] = uhci_alloc_qh(uhci, NULL, NULL);
+		if (!uhci->skelqh[i]) {
+			dev_err(uhci_dev(uhci), "unable to allocate QH\n");
+			goto err_alloc_skelqh;
+		}
+	}
+
+	/*
+	 * 8 Interrupt queues; link all higher int queues to int1 = async
+	 */
+	for (i = SKEL_ISO + 1; i < SKEL_ASYNC; ++i)
+		uhci->skelqh[i]->link = LINK_TO_QH(uhci->skel_async_qh);
+	uhci->skel_async_qh->link = UHCI_PTR_TERM;
+	uhci->skel_term_qh->link = LINK_TO_QH(uhci->skel_term_qh);
+
+	/* This dummy TD is to work around a bug in Intel PIIX controllers */
+	uhci_fill_td(uhci->term_td, 0, uhci_explen(0) |
+			(0x7f << TD_TOKEN_DEVADDR_SHIFT) | USB_PID_IN, 0);
+	uhci->term_td->link = UHCI_PTR_TERM;
+	uhci->skel_async_qh->element = uhci->skel_term_qh->element =
+			LINK_TO_TD(uhci->term_td);
+
+	/*
+	 * Fill the frame list: make all entries point to the proper
+	 * interrupt queue.
+	 */
+	for (i = 0; i < UHCI_NUMFRAMES; i++) {
+
+		/* Only place we don't use the frame list routines */
+		uhci->frame[i] = uhci_frame_skel_link(uhci, i);
+	}
+
+	/*
+	 * Some architectures require a full mb() to enforce completion of
+	 * the memory writes above before the I/O transfers in configure_hc().
+	 */
+	mb();
+
+	configure_hc(uhci);
+	uhci->is_initialized = 1;
+	start_rh(uhci);
+	return 0;
+
+/*
+ * error exits:
+ */
+err_alloc_skelqh:
+	for (i = 0; i < UHCI_NUM_SKELQH; i++) {
+		if (uhci->skelqh[i])
+			uhci_free_qh(uhci, uhci->skelqh[i]);
+	}
+
+	uhci_free_td(uhci, uhci->term_td);
+
+err_alloc_term_td:
+	dma_pool_destroy(uhci->qh_pool);
+
+err_create_qh_pool:
+	dma_pool_destroy(uhci->td_pool);
+
+err_create_td_pool:
+	kfree(uhci->frame_cpu);
+
+err_alloc_frame_cpu:
+	dma_free_coherent(uhci_dev(uhci),
+			UHCI_NUMFRAMES * sizeof(*uhci->frame),
+			uhci->frame, uhci->frame_dma_handle);
+
+err_alloc_frame:
+	debugfs_remove(uhci->dentry);
+
+err_create_debug_entry:
+	return retval;
+}
+
+static void uhci_stop(struct usb_hcd *hcd)
+{
+	struct uhci_hcd *uhci = hcd_to_uhci(hcd);
+
+	spin_lock_irq(&uhci->lock);
+	if (test_bit(HCD_FLAG_HW_ACCESSIBLE, &hcd->flags) && !uhci->dead)
+		uhci_hc_died(uhci);
+	uhci_scan_schedule(uhci);
+	spin_unlock_irq(&uhci->lock);
+
+	del_timer_sync(&uhci->fsbr_timer);
+	release_uhci(uhci);
+}
+
+#ifdef CONFIG_PM
+static int uhci_rh_suspend(struct usb_hcd *hcd)
+{
+	struct uhci_hcd *uhci = hcd_to_uhci(hcd);
+	int rc = 0;
+
+	spin_lock_irq(&uhci->lock);
+	if (!test_bit(HCD_FLAG_HW_ACCESSIBLE, &hcd->flags))
+		rc = -ESHUTDOWN;
+	else if (!uhci->dead)
+		suspend_rh(uhci, UHCI_RH_SUSPENDED);
+	spin_unlock_irq(&uhci->lock);
+	return rc;
+}
+
+static int uhci_rh_resume(struct usb_hcd *hcd)
+{
+	struct uhci_hcd *uhci = hcd_to_uhci(hcd);
+	int rc = 0;
+
+	spin_lock_irq(&uhci->lock);
+	if (!test_bit(HCD_FLAG_HW_ACCESSIBLE, &hcd->flags))
+		rc = -ESHUTDOWN;
+	else if (!uhci->dead)
+		wakeup_rh(uhci);
+	spin_unlock_irq(&uhci->lock);
+	return rc;
+}
+
+static int uhci_pci_suspend(struct usb_hcd *hcd, pm_message_t message)
+{
+	struct uhci_hcd *uhci = hcd_to_uhci(hcd);
+	int rc = 0;
+
+	dev_dbg(uhci_dev(uhci), "%s\n", __func__);
+
+	spin_lock_irq(&uhci->lock);
+	if (!test_bit(HCD_FLAG_HW_ACCESSIBLE, &hcd->flags) || uhci->dead)
+		goto done_okay;		/* Already suspended or dead */
+
+	if (uhci->rh_state > UHCI_RH_SUSPENDED) {
+		dev_warn(uhci_dev(uhci), "Root hub isn't suspended!\n");
+		rc = -EBUSY;
+		goto done;
+	};
+
+	/* All PCI host controllers are required to disable IRQ generation
+	 * at the source, so we must turn off PIRQ.
+	 */
+	pci_write_config_word(to_pci_dev(uhci_dev(uhci)), USBLEGSUP, 0);
+	mb();
+	hcd->poll_rh = 0;
+
+	/* FIXME: Enable non-PME# remote wakeup? */
+
+	/* make sure snapshot being resumed re-enumerates everything */
+	if (message.event == PM_EVENT_PRETHAW)
+		uhci_hc_died(uhci);
+
+done_okay:
+	clear_bit(HCD_FLAG_HW_ACCESSIBLE, &hcd->flags);
+done:
+	spin_unlock_irq(&uhci->lock);
+	return rc;
+}
+
+static int uhci_pci_resume(struct usb_hcd *hcd)
+{
+	struct uhci_hcd *uhci = hcd_to_uhci(hcd);
+
+	dev_dbg(uhci_dev(uhci), "%s\n", __func__);
+
+	/* Since we aren't in D3 any more, it's safe to set this flag
+	 * even if the controller was dead.
+	 */
+	set_bit(HCD_FLAG_HW_ACCESSIBLE, &hcd->flags);
+	mb();
+
+	spin_lock_irq(&uhci->lock);
+
+	/* FIXME: Disable non-PME# remote wakeup? */
+
+	/* The firmware or a boot kernel may have changed the controller
+	 * settings during a system wakeup.  Check it and reconfigure
+	 * to avoid problems.
+	 */
+	check_and_reset_hc(uhci);
+
+	/* If the controller was dead before, it's back alive now */
+	configure_hc(uhci);
+
+	if (uhci->rh_state == UHCI_RH_RESET) {
+
+		/* The controller had to be reset */
+		usb_root_hub_lost_power(hcd->self.root_hub);
+		suspend_rh(uhci, UHCI_RH_SUSPENDED);
+	}
+
+	spin_unlock_irq(&uhci->lock);
+
+	/* If interrupts don't work and remote wakeup is enabled then
+	 * the suspended root hub needs to be polled.
+	 */
+	if (!uhci->RD_enable && hcd->self.root_hub->do_remote_wakeup) {
+		hcd->poll_rh = 1;
+		usb_hcd_poll_rh_status(hcd);
+	}
+	return 0;
+}
+#endif
+
+/* Wait until a particular device/endpoint's QH is idle, and free it */
+static void uhci_hcd_endpoint_disable(struct usb_hcd *hcd,
+		struct usb_host_endpoint *hep)
+{
+	struct uhci_hcd *uhci = hcd_to_uhci(hcd);
+	struct uhci_qh *qh;
+
+	spin_lock_irq(&uhci->lock);
+	qh = (struct uhci_qh *) hep->hcpriv;
+	if (qh == NULL)
+		goto done;
+
+	while (qh->state != QH_STATE_IDLE) {
+		++uhci->num_waiting;
+		spin_unlock_irq(&uhci->lock);
+		wait_event_interruptible(uhci->waitqh,
+				qh->state == QH_STATE_IDLE);
+		spin_lock_irq(&uhci->lock);
+		--uhci->num_waiting;
+	}
+
+	uhci_free_qh(uhci, qh);
+done:
+	spin_unlock_irq(&uhci->lock);
+}
+
+static int uhci_hcd_get_frame_number(struct usb_hcd *hcd)
+{
+	struct uhci_hcd *uhci = hcd_to_uhci(hcd);
+	unsigned frame_number;
+	unsigned delta;
+
+	/* Minimize latency by avoiding the spinlock */
+	frame_number = uhci->frame_number;
+	barrier();
+	delta = (ioread32(uhci->io_addr + USBFRNUM) - frame_number) &
+			(UHCI_NUMFRAMES - 1);
+	return frame_number + delta;
+}
+
+static const char hcd_name[] = "uhci_hcd";
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
+	.reset =		uhci_init,
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
+#define AST_SCU_LOCK_KEY 0x00
+#define AST_SCU_UNLOCK_MAGIC 0x1688A8A8
+
+static int ast_uhci_hc_drv_probe(struct platform_device *pdev)
+{
+	struct usb_hcd *hcd = NULL;
+	uint32_t reg;
+	int retval;
+
+	if (usb_disabled())
+		return -ENODEV;
+
+	/* enable clock and reset USB1.1 host controller */
+	iowrite32(AST_SCU_UNLOCK_MAGIC, AST_SCU_VA_BASE + AST_SCU_LOCK_KEY); /* unlock SCU */
+
+	/* enable clock of USB host controller */
+	reg = ioread32(SCU_CLK_STOP_REG);
+	reg &= ~(USB11HOST_CLOCK_BIT);
+	iowrite32(reg, SCU_CLK_STOP_REG);
+
+	mdelay(10);
+
+	/* reset USB host controller */
+	reg = ioread32(SCU_SYS_RESET_REG);
+	reg &= ~(USB11HOST_RESET_BIT);
+	iowrite32(reg, SCU_SYS_RESET_REG);
+
+	/* we only use one port */
+	reg = ioread32(AST_SCU_VA_BASE + 0x90);
+	reg &= ~USB_HOST_2_PORTS;
+	iowrite32(reg, AST_SCU_VA_BASE + 0x90);
+
+	iowrite32(0, AST_SCU_VA_BASE + AST_SCU_LOCK_KEY); /* lock SCU */
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
+	retval = usb_add_hcd(hcd, pdev->resource[1].start, IRQF_SHARED);
+	if (retval != 0) {
+		printk("add UHCI to USB host controller list failed!\n");
+		goto err_ioremap;
+	}
+
+	IRQ_SET_HIGH_LEVEL(AST_UHC_IRQ);
+	IRQ_SET_LEVEL_TRIGGER(AST_UHC_IRQ);
+
+	iowrite32(0x01, hcd->regs + 0x94); // for frame timing setting
+
+	return 0;
+
+err_ioremap:
+	uhci_stop_hc(hcd);
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
+	uhci_stop_hc(hcd);
+	iounmap(hcd->regs);
+	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
+	usb_put_hcd(hcd);
+
+	return 0;
+}
+
+static struct resource ast_uhci_hc_resource[] = {
+	[0] = {
+		.start = AST_UHC_REG_ADDR,
+		.end   = AST_UHC_REG_ADDR + AST_UHC_REG_SZ - 1,
+		.flags = IORESOURCE_MEM,
+	},
+	[1] = {
+		.start = AST_UHC_IRQ,
+		.end   = AST_UHC_IRQ,
+		.flags = IORESOURCE_IRQ,
+	}
+};
+
+static struct platform_driver ast_uhci_hc_driver = {
+	.probe = ast_uhci_hc_drv_probe,
+	.remove = ast_uhci_hc_drv_remove,
+	.shutdown	= usb_hcd_platform_shutdown,
+	.driver = {
+		.owner = THIS_MODULE,
+		.name = "ast_uhci_hc",
+	},
+};
+
+static struct platform_device ast_uhci_hc_device = {
+	.name = "ast_uhci_hc",
+	.num_resources = ARRAY_SIZE(ast_uhci_hc_resource),
+	.resource = ast_uhci_hc_resource,
+	.dev = {
+		.dma_mask = (u64 *) 0xffffffffUL,
+		.coherent_dma_mask = 0xffffffffUL
+	}
+};
+
+static int __init uhci_hcd_init(void)
+{
+	int retval = -ENOMEM;
+
+	if (usb_disabled())
+		return -ENODEV;
+
+	printk(KERN_INFO "uhci_hcd: " DRIVER_DESC "%s\n",
+			ignore_oc ? ", overcurrent ignored" : "");
+	set_bit(USB_UHCI_LOADED, &usb_hcds_loaded);
+
+	if (DEBUG_CONFIGURED) {
+		errbuf = kmalloc(ERRBUF_LEN, GFP_KERNEL);
+		if (!errbuf)
+			goto errbuf_failed;
+		uhci_debugfs_root = debugfs_create_dir("uhci", NULL);
+		if (!uhci_debugfs_root)
+			goto debug_failed;
+	}
+
+	uhci_up_cachep = kmem_cache_create("uhci_urb_priv",
+		sizeof(struct urb_priv), 0, 0, NULL);
+	if (!uhci_up_cachep)
+		goto up_failed;
+
+	retval = platform_driver_register(&ast_uhci_hc_driver);
+	if (retval == 0) {
+		retval = platform_device_register(&ast_uhci_hc_device);
+		if (retval) {
+			platform_driver_unregister(&ast_uhci_hc_driver);
+			goto init_failed;
+		}
+	}
+
+	return 0;
+
+init_failed:
+	kmem_cache_destroy(uhci_up_cachep);
+
+up_failed:
+	debugfs_remove(uhci_debugfs_root);
+
+debug_failed:
+	kfree(errbuf);
+
+errbuf_failed:
+
+	clear_bit(USB_UHCI_LOADED, &usb_hcds_loaded);
+	return retval;
+}
+
+static void __exit uhci_hcd_cleanup(void) 
+{
+	platform_device_unregister(&ast_uhci_hc_device);
+	platform_driver_unregister(&ast_uhci_hc_driver);
+	kmem_cache_destroy(uhci_up_cachep);
+	debugfs_remove(uhci_debugfs_root);
+	kfree(errbuf);
+	clear_bit(USB_UHCI_LOADED, &usb_hcds_loaded);
+}
+
+module_init(uhci_hcd_init);
+module_exit(uhci_hcd_cleanup);
+
+MODULE_AUTHOR(DRIVER_AUTHOR);
+MODULE_DESCRIPTION(DRIVER_DESC);
+MODULE_LICENSE("GPL");
