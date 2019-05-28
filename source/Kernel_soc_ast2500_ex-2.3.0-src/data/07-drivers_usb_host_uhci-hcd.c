--- linux/drivers/usb/host/uhci-hcd.c	2014-08-14 09:38:34.000000000 +0800
+++ linux.new/drivers/usb/host/uhci-hcd.c	2015-01-26 14:29:17.937395548 +0800
@@ -88,7 +88,7 @@
 
 static struct kmem_cache *uhci_up_cachep;	/* urb_priv */
 
-static void suspend_rh(struct uhci_hcd *uhci, enum uhci_rh_state new_state);
+//static void suspend_rh(struct uhci_hcd *uhci, enum uhci_rh_state new_state);
 static void wakeup_rh(struct uhci_hcd *uhci);
 static void uhci_get_current_frame_number(struct uhci_hcd *uhci);
 
@@ -136,7 +136,7 @@
 	 * We have to clear them by hand.
 	 */
 	for (port = 0; port < uhci->rh_numports; ++port)
-		uhci_writew(uhci, 0, USBPORTSC1 + (port * 2));
+		uhci_writew(uhci, 0, USBPORTSC1 + (port * USB_PORT_OFFSET));
 
 	uhci->port_c_suspend = uhci->resuming_ports = 0;
 	uhci->rh_state = UHCI_RH_RESET;
@@ -263,6 +263,7 @@
 		uhci->configure_hc(uhci);
 }
 
+#if 0
 static int resume_detect_interrupts_are_broken(struct uhci_hcd *uhci)
 {
 	/* If we have to ignore overcurrent events then almost by definition
@@ -379,6 +380,7 @@
 	uhci_scan_schedule(uhci);
 	uhci_fsbr_off(uhci);
 }
+#endif
 
 static void start_rh(struct uhci_hcd *uhci)
 {
@@ -818,10 +820,10 @@
 	 * a nonexistent register is addressed is to return all ones, so
 	 * we test for that also.
 	 */
-	for (port = 0; port < (io_size - USBPORTSC1) / 2; port++) {
+	for (port = 0; port < (io_size - USBPORTSC1) / USB_PORT_OFFSET; port++) {
 		unsigned int portstatus;
 
-		portstatus = uhci_readw(uhci, USBPORTSC1 + (port * 2));
+		portstatus = uhci_readw(uhci, USBPORTSC1 + (port * USB_PORT_OFFSET));
 		if (!(portstatus & 0x0080) || portstatus == 0xffff)
 			break;
 	}
@@ -855,6 +857,11 @@
 #define PLATFORM_DRIVER		uhci_platform_driver
 #endif
 
+#if defined(CONFIG_SPX_FEATURE_GLOBAL_UHCI1_USB_HOST_CONTROLLER) || defined(CONFIG_SPX_FEATURE_GLOBAL_UHCI2_USB_HOST_CONTROLLER)
+#include "uhci-ast.c"
+#define PLATFORM_DRIVER		uhci_aspeed_driver
+#endif
+
 #if !defined(PCI_DRIVER) && !defined(PLATFORM_DRIVER)
 #error "missing bus glue for uhci-hcd"
 #endif
@@ -885,9 +892,14 @@
 		goto up_failed;
 
 #ifdef PLATFORM_DRIVER
-	retval = platform_driver_register(&PLATFORM_DRIVER);
-	if (retval < 0)
-		goto clean0;
+	retval = platform_device_register(&aspeed_uhci);
+	if (retval == 0) {
+		retval = platform_driver_register(&PLATFORM_DRIVER);
+		if (retval) {
+			platform_device_unregister(&aspeed_uhci);
+			goto clean0;
+		}
+	}
 #endif
 
 #ifdef PCI_DRIVER
@@ -924,6 +936,7 @@
 static void __exit uhci_hcd_cleanup(void) 
 {
 #ifdef PLATFORM_DRIVER
+	platform_device_unregister(&aspeed_uhci);
 	platform_driver_unregister(&PLATFORM_DRIVER);
 #endif
 #ifdef PCI_DRIVER
