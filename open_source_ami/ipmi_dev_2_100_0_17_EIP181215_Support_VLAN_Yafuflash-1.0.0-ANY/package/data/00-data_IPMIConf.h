--- .pristine/ipmi_dev-2.100.0-src/data/IPMIConf.h	Thu Aug 14 14:33:27 2014
+++ source/ipmi_dev-2.100.0-src/data/IPMIConf.h	Thu Aug 14 14:59:23 2014
@@ -385,6 +385,7 @@
     INT8U               TCPChannel[MAX_LAN_CHANNELS];
     INT8U               VLANChannel[MAX_LAN_CHANNELS];
     SOCKET		VLANUDPSocket[MAX_LAN_CHANNELS];
+    SOCKET              VLANTCPSocket[MAX_LAN_CHANNELS];
     INT16U		VLANID[MAX_LAN_CHANNELS];
     int			VLANIFcheckFlag[MAX_LAN_CHANNELS];
     int                     LANIFcheckFlag[MAX_LAN_CHANNELS];
