--- uboot/common/miiphyutil.c	2006-11-02 09:15:01.000000000 -0500
+++ uboot.new/common/miiphyutil.c	2010-09-07 15:59:41.487733610 -0400
@@ -368,15 +368,15 @@
 
 	/* Check Basic Management Control Register first. */
 	if (miiphy_read (devname, addr, PHY_BMCR, &reg)) {
-		puts ("PHY speed read failed, assuming 10bT\n");
-		return (_10BASET);
+		puts ("PHY speed read failed, assuming 100bT\n");
+		return (_100BASET);
 	}
 	/* Check if auto-negotiation is on. */
 	if ((reg & PHY_BMCR_AUTON) != 0) {
 		/* Get auto-negotiation results. */
 		if (miiphy_read (devname, addr, PHY_ANLPAR, &reg)) {
-			puts ("PHY AN speed read failed, assuming 10bT\n");
-			return (_10BASET);
+			puts ("PHY AN speed read failed, assuming 100bT\n");
+			return (_100BASET);
 		}
 		if ((reg & PHY_ANLPAR_100) != 0) {
 			return (_100BASET);
@@ -419,15 +419,15 @@
 
 	/* Check Basic Management Control Register first. */
 	if (miiphy_read (devname, addr, PHY_BMCR, &reg)) {
-		puts ("PHY duplex read failed, assuming half duplex\n");
-		return (HALF);
+		puts ("PHY duplex read failed, assuming full duplex\n");
+		return (FULL);
 	}
 	/* Check if auto-negotiation is on. */
 	if ((reg & PHY_BMCR_AUTON) != 0) {
 		/* Get auto-negotiation results. */
 		if (miiphy_read (devname, addr, PHY_ANLPAR, &reg)) {
-			puts ("PHY AN duplex read failed, assuming half duplex\n");
-			return (HALF);
+			puts ("PHY AN duplex read failed, assuming full duplex\n");
+			return (FULL);
 		}
 
 		if ((reg & (PHY_ANLPAR_10FD | PHY_ANLPAR_TXFD)) != 0) {
