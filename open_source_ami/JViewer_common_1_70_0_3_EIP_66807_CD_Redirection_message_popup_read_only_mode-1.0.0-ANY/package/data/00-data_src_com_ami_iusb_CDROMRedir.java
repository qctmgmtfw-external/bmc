--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/iusb/CDROMRedir.java Mon Sep  5 17:06:11 2011
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/iusb/CDROMRedir.java Thu Sep  8 17:09:34 2011
@@ -17,6 +17,7 @@
 import java.io.UnsupportedEncodingException;
 import java.nio.ByteBuffer;
 import java.nio.ByteOrder;
+import javax.swing.JOptionPane;
 
 import com.ami.iusb.protocol.CDROMProtocol;
 import com.ami.iusb.protocol.IUSBHeader;
@@ -250,6 +251,9 @@
         	vMThread.startCDROMMonitor();
         }
 		running = true;
+		//CD Redirection will always occur in read only mode, irrespective of whether its Physical device or ISO image
+		//that is being redirected.
+		JOptionPane.showMessageDialog(JViewerApp.getInstance().getMainWindow(), "Device redirected in Read Only Mode");	
 		return (true);
 	}
 
