--- linux/drivers/usb/host/ehci-hcd.c	2014-08-14 09:38:34.000000000 +0800
+++ linux.new/drivers/usb/host/ehci-hcd.c	2015-01-26 14:28:53.849395507 +0800
@@ -1300,6 +1300,11 @@
 #define	PLATFORM_DRIVER		ehci_hcd_sead3_driver
 #endif
 
+#if defined(CONFIG_SPX_FEATURE_GLOBAL_EHCI1_USB_HOST_CONTROLLER) || defined(CONFIG_SPX_FEATURE_GLOBAL_EHCI2_USB_HOST_CONTROLLER)
+#include "ehci-ast.c"
+#define	PLATFORM_DRIVER		ehci_aspeed_driver
+#endif
+
 static int __init ehci_hcd_init(void)
 {
 	int retval = 0;
@@ -1328,6 +1333,21 @@
 #endif
 
 #ifdef PLATFORM_DRIVER
+
+#ifdef CONFIG_SPX_FEATURE_GLOBAL_EHCI1_USB_HOST_CONTROLLER
+	retval = platform_device_register(&aspeed_ehci1);
+	if (retval < 0) {
+		printk(KERN_INFO "Unable to register USB EHCI device: %s.\n", aspeed_ehci1.name);
+		goto AST_EHCI1;
+	}
+#endif 
+#ifdef CONFIG_SPX_FEATURE_GLOBAL_EHCI2_USB_HOST_CONTROLLER
+	retval = platform_device_register(&aspeed_ehci2);
+	if (retval < 0) {
+		printk(KERN_INFO "Unable to register USB EHCI device: %s.\n", aspeed_ehci2.name);
+		goto AST_EHCI2;
+	}
+#endif 
 	retval = platform_driver_register(&PLATFORM_DRIVER);
 	if (retval < 0)
 		goto clean0;
@@ -1364,10 +1384,21 @@
 	ps3_ehci_driver_unregister(&PS3_SYSTEM_BUS_DRIVER);
 clean2:
 #endif
+
 #ifdef PLATFORM_DRIVER
 	platform_driver_unregister(&PLATFORM_DRIVER);
 clean0:
+#ifdef CONFIG_SPX_FEATURE_GLOBAL_EHCI2_USB_HOST_CONTROLLER
+	platform_device_unregister(&aspeed_ehci2);
+AST_EHCI2:
+#endif 
+#ifdef CONFIG_SPX_FEATURE_GLOBAL_EHCI1_USB_HOST_CONTROLLER
+	platform_device_unregister(&aspeed_ehci1);
+AST_EHCI1:
+#endif 
 #endif
+
+
 #ifdef CONFIG_DYNAMIC_DEBUG
 	debugfs_remove(ehci_debug_root);
 	ehci_debug_root = NULL;
@@ -1388,6 +1419,12 @@
 #endif
 #ifdef PLATFORM_DRIVER
 	platform_driver_unregister(&PLATFORM_DRIVER);
+#ifdef CONFIG_SPX_FEATURE_GLOBAL_EHCI2_USB_HOST_CONTROLLER
+	platform_device_unregister(&aspeed_ehci2);
+#endif 
+#ifdef CONFIG_SPX_FEATURE_GLOBAL_EHCI1_USB_HOST_CONTROLLER
+	platform_device_unregister(&aspeed_ehci1);
+#endif 
 #endif
 #ifdef PS3_SYSTEM_BUS_DRIVER
 	ps3_ehci_driver_unregister(&PS3_SYSTEM_BUS_DRIVER);
