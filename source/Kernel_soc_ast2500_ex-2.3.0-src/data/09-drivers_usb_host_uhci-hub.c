--- linux/drivers/usb/host/uhci-hub.c	2014-08-14 09:38:34.000000000 +0800
+++ linux.new/drivers/usb/host/uhci-hub.c	2015-01-21 12:23:50.819649014 +0800
@@ -44,7 +44,7 @@
 	int port;
 
 	for (port = 0; port < uhci->rh_numports; ++port) {
-		if ((uhci_readw(uhci, USBPORTSC1 + port * 2) &
+		if ((uhci_readw(uhci, USBPORTSC1 + port * USB_PORT_OFFSET) &
 				(USBPORTSC_CCS | RWC_BITS)) ||
 				test_bit(port, &uhci->port_c_suspend))
 			return 1;
@@ -68,7 +68,7 @@
 
 	*buf = 0;
 	for (port = 0; port < uhci->rh_numports; ++port) {
-		if ((uhci_readw(uhci, USBPORTSC1 + port * 2) & mask) ||
+		if ((uhci_readw(uhci, USBPORTSC1 + port * USB_PORT_OFFSET) & mask) ||
 				test_bit(port, &uhci->port_c_suspend))
 			*buf |= (1 << (port + 1));
 	}
@@ -139,7 +139,7 @@
 	int status;
 
 	for (port = 0; port < uhci->rh_numports; ++port) {
-		port_addr = USBPORTSC1 + 2 * port;
+		port_addr = USBPORTSC1 + USB_PORT_OFFSET * port;
 		status = uhci_readw(uhci, port_addr);
 		if (unlikely(status & USBPORTSC_PR)) {
 			if (time_after_eq(jiffies, uhci->ports_timeout)) {
@@ -223,9 +223,9 @@
 		/* auto-stop if nothing connected for 1 second */
 		if (any_ports_active(uhci))
 			uhci->rh_state = UHCI_RH_RUNNING;
-		else if (time_after_eq(jiffies, uhci->auto_stop_time) &&
-				!uhci->wait_for_hp)
-			suspend_rh(uhci, UHCI_RH_AUTO_STOPPED);
+		//else if (time_after_eq(jiffies, uhci->auto_stop_time) &&
+		//		!uhci->wait_for_hp)
+		//	suspend_rh(uhci, UHCI_RH_AUTO_STOPPED);
 		break;
 
 	    default:
@@ -244,7 +244,7 @@
 	struct uhci_hcd *uhci = hcd_to_uhci(hcd);
 	int status, lstatus, retval = 0;
 	unsigned int port = wIndex - 1;
-	unsigned long port_addr = USBPORTSC1 + 2 * port;
+	unsigned long port_addr = USBPORTSC1 + USB_PORT_OFFSET * port;
 	u16 wPortChange, wPortStatus;
 	unsigned long flags;
 
