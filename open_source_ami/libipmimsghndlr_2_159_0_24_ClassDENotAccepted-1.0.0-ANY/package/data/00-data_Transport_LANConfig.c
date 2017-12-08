--- .pristine/libipmimsghndlr-2.159.0-src/data/Transport/LANConfig.c Fri Jun 29 11:36:01 2012
+++ source/libipmimsghndlr-2.159.0-src/data/Transport/LANConfig.c Fri Jun 29 11:46:58 2012
@@ -1439,6 +1439,10 @@
 int IPAddrCheck(INT8U *Addr,int params)
 {
     int i,maskcount=0,bitmask =0,j,bitcheck=0;
+    
+    if(params != LAN_PARAM_SUBNET_MASK)
+		if(Addr[0] == 0 || Addr[0] == 127 || Addr[0] == 224 || Addr[0] == 240)
+			return 1;
 
     for(i=0;i< IP_ADDR_LEN;i++)
     {
