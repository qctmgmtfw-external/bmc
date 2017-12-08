--- .pristine/usb1_hw-1.6.0-ARM-AST-src/data/ast_usbhub.c	Mon Feb  9 10:33:43 2015
+++ source/usb1_hw-1.6.0-ARM-AST-src/data/ast_usbhub.c	Thu Feb 12 10:26:55 2015
@@ -1753,6 +1753,7 @@
 	uint32_t irq_status;
 	uint32_t reg;
 	uint8_t setup_data[8];
+	uint8_t port;
 	struct usb_ctrlrequest *ctrl_req;
 	int length, i;
 
@@ -1781,6 +1782,8 @@
 			ast_usbhub_port_status[i].wPortChange = 0;
 			ast_usbhub_dev_run_speed[i] = SUPPORT_FULL_SPEED;
 
+			port = ast_usbhub_convert_dev_to_port(ast_usbhub_dev_cfg[i].dev_num);
+			ast_usbhub_clear_bitmap(port);
 			if (ast_usbhub_device_connect_to_port[i]) {
 				usb_core_module.CoreUsbBusReset(ast_usbhub_dev_cfg[i].dev_num);
 			}
