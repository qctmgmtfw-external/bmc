--- .pristine/cdserverapp-1.16.0-src/data/cdserver.c	Tue Jun  3 12:13:10 2014
+++ source/cdserverapp-1.16.0-src/data/cdserver.c	Wed Jun  4 16:13:57 2014
@@ -1314,7 +1314,7 @@
 		{
 			TWARN ("Local CD Instance connection closure detected\n");
 			LocalCDConnected = 0;
-			if ((isOtherVMEDIAInUse == 0) && IsStatusAutoAttach ())
+			if ((Checkanyinstancerunning(RunCdChild, gNumCdInstances) == 0) && IsStatusAutoAttach())
 			{
 				ModifiedData = (u8*) AddAuthInfo (USB_DEVICE_DISCONNECT, NULL, Instance);
 				if( ioctl(usbfd, USB_DEVICE_DISCONNECT, ModifiedData) < 0 )
@@ -1347,7 +1347,7 @@
 		if ((isOtherVMEDIAInUse == 0 ) || (iUSBDevInfo[Instance].DevInfo.DeviceType != IUSB_CDROM_FLOPPY_COMBO))
 		{
 			// If status is auto attach, need to disconnect usb device, if floppy is not active
-			if ((IsStatusAutoAttach ()) && (LocalCDConnected == 0))
+			if ((Checkanyinstancerunning(RunCdChild, gNumCdInstances) == 0) && IsStatusAutoAttach())
 			{
 				ModifiedData = (u8*) AddAuthInfo (USB_DEVICE_DISCONNECT, NULL, Instance);
 				if( ioctl(usbfd, USB_DEVICE_DISCONNECT, ModifiedData) < 0 )
