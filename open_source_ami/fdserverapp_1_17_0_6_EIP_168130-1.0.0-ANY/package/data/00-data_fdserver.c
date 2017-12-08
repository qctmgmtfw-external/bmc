--- .pristine/fdserverapp-1.17.0-src/data/fdserver.c	Tue Jun  3 12:13:11 2014
+++ source/fdserverapp-1.17.0-src/data/fdserver.c	Wed Jun  4 16:13:46 2014
@@ -1290,7 +1290,7 @@
 		{
 			TWARN ("Local FD Instance connection closure detected\n");
 			LocalFDConnected = 0;
-			if ((isOtherVMEDIAInUse == 0) && IsStatusAutoAttach ())
+			if ((Checkanyinstancerunning(RunFdChild, gNumFdInstances) == 0) && IsStatusAutoAttach())
 			{
 				ModifiedData = (u8*) AddAuthInfo (USB_DEVICE_DISCONNECT, NULL, Instance);
 				if( ioctl(usbfd, USB_DEVICE_DISCONNECT, ModifiedData) < 0 )
@@ -1324,7 +1324,7 @@
 		if ((isOtherVMEDIAInUse == 0 ) || (iUSBDevInfo[Instance].DevInfo.DeviceType != IUSB_CDROM_FLOPPY_COMBO))
 		{
 			// If status is auto attach, need to disconnect usb device, if cdrom is not active
-			if ((IsStatusAutoAttach()) && (LocalFDConnected == 0))
+			if ((Checkanyinstancerunning(RunFdChild, gNumFdInstances) == 0) && IsStatusAutoAttach())
 			{
 					ModifiedData = AddAuthInfo (USB_DEVICE_DISCONNECT, NULL, Instance);
 					if( ioctl(usbfd, USB_DEVICE_DISCONNECT, ModifiedData) < 0 )
