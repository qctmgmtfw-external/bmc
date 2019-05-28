--- u-boot-2013.07/common/miiphyutil.c	2013-12-13 13:49:25.700958498 -0500
+++ uboot.new/common/miiphyutil.c	2013-12-05 12:17:55.223503948 -0500
@@ -374,7 +374,7 @@
 
 	debug("MII_PHYSID2 @ 0x%x = 0x%04x\n", addr, reg);
 
-	if (reg == 0xFFFF) {
+	if (reg == 0xFFFF || reg == 0x0000) {
 		/* No physical device present at this address */
 		return -1;
 	}
@@ -490,8 +490,8 @@
 	return (bmcr & BMCR_SPEED100) ? _100BASET : _10BASET;
 
 miiphy_read_failed:
-	printf(" read failed, assuming 10BASE-T\n");
-	return _10BASET;
+	printf(" read failed, assuming 100BASE-T\n");		/* Possibly NC-SI */
+	return _100BASET;
 }
 
 /*****************************************************************************
@@ -549,8 +549,8 @@
 	return (bmcr & BMCR_FULLDPLX) ? FULL : HALF;
 
 miiphy_read_failed:
-	printf(" read failed, assuming half duplex\n");
-	return HALF;
+	printf(" read failed, assuming full duplex\n");				/* Possibly NC-SI */
+	return FULL;
 }
 
 /*****************************************************************************
