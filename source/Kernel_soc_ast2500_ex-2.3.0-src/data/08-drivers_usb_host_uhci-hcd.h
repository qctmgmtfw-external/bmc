--- linux/drivers/usb/host/uhci-hcd.h	2014-08-14 09:38:34.000000000 +0800
+++ linux.new/drivers/usb/host/uhci-hcd.h	2015-01-21 12:23:50.911649014 +0800
@@ -12,6 +12,13 @@
  * Universal Host Controller Interface data structures and defines
  */
 
+/* Port offset */
+#if defined(CONFIG_SOC_AST2500)
+	#define USB_PORT_OFFSET 4	/* Registers offset is in multiple of 4 */
+#else
+	#define USB_PORT_OFFSET 2	/* Registers offset is in multiple of 2 */
+#endif
+
 /* Command register */
 #define USBCMD		0
 #define   USBCMD_RS		0x0001	/* Run/Stop */
@@ -24,7 +31,11 @@
 #define   USBCMD_MAXP		0x0080	/* Max Packet (0 = 32, 1 = 64) */
 
 /* Status register */
-#define USBSTS		2
+#if defined(CONFIG_SOC_AST2500)
+	#define USBSTS		0x04
+#else
+	#define USBSTS		2
+#endif
 #define   USBSTS_USBINT		0x0001	/* Interrupt due to IOC */
 #define   USBSTS_ERROR		0x0002	/* Interrupt due to error */
 #define   USBSTS_RD		0x0004	/* Resume Detect */
@@ -34,20 +45,35 @@
 #define   USBSTS_HCH		0x0020	/* HC Halted */
 
 /* Interrupt enable register */
-#define USBINTR		4
+#if defined(CONFIG_SOC_AST2500)
+	#define USBINTR		0x08
+#else
+	#define USBINTR		4
+#endif
 #define   USBINTR_TIMEOUT	0x0001	/* Timeout/CRC error enable */
 #define   USBINTR_RESUME	0x0002	/* Resume interrupt enable */
 #define   USBINTR_IOC		0x0004	/* Interrupt On Complete enable */
 #define   USBINTR_SP		0x0008	/* Short packet interrupt enable */
 
-#define USBFRNUM	6
-#define USBFLBASEADD	8
-#define USBSOF		12
+#if defined(CONFIG_SOC_AST2500)
+	#define USBFRNUM	0x80
+	#define USBFLBASEADD	0x0C
+	#define USBSOF		0x84
+#else
+	#define USBFRNUM	6
+	#define USBFLBASEADD	8
+	#define USBSOF		12
+#endif
 #define   USBSOF_DEFAULT	64	/* Frame length is exactly 1 ms */
 
 /* USB port status and control registers */
-#define USBPORTSC1	16
-#define USBPORTSC2	18
+#if defined(CONFIG_SOC_AST2500)
+	#define USBPORTSC1	0x88
+	#define USBPORTSC2	0x8C
+#else
+	#define USBPORTSC1	16
+	#define USBPORTSC2	18
+#endif
 #define   USBPORTSC_CCS		0x0001	/* Current Connect Status
 					 * ("device present") */
 #define   USBPORTSC_CSC		0x0002	/* Connect Status Change */
