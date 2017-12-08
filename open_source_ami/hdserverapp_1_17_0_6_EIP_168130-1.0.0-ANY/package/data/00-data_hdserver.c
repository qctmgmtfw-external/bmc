--- .pristine/hdserverapp-1.17.0-src/data/hdserver.c	Tue Jun  3 12:13:13 2014
+++ source/hdserverapp-1.17.0-src/data/hdserver.c	Wed Jun  4 16:08:23 2014
@@ -1360,7 +1360,7 @@
 		{
 			TWARN ("Local HD Instance connection closure detected\n");
 			LocalHDConnected = 0;
-			if ((isOtherVMEDIAInUse == 0) && IsStatusAutoAttach ())
+			if ((Checkanyinstancerunning(RunHdChild, gNumHdInstances) == 0) && IsStatusAutoAttach())
 			{
 				ModifiedData = (u8*) AddAuthInfo (USB_DEVICE_DISCONNECT, NULL, Instance);
 				if( ioctl(usbhd, USB_DEVICE_DISCONNECT, ModifiedData) < 0 )
@@ -1394,7 +1394,7 @@
 		if( (isOtherVMEDIAInUse == 0 ) || (iUSBDevInfo[Instance].DevInfo.DeviceType != IUSB_CDROM_FLOPPY_COMBO))
 		{
 			// If status is auto attach, need to disconnect usb device, if cdrom is not active
-			if ((IsStatusAutoAttach()) && (LocalHDConnected == 0))
+			if ((Checkanyinstancerunning(RunHdChild, gNumHdInstances) == 0) && IsStatusAutoAttach())
 			{
 				ModifiedData = AddAuthInfo (USB_DEVICE_DISCONNECT, NULL, Instance);
 				if( ioctl(usbhd, USB_DEVICE_DISCONNECT, ModifiedData) < 0 )
