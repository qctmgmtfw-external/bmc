--- u-boot-2013.07/common/cmd_mmc.c	2013-12-13 13:49:25.704958498 -0500
+++ uboot.new/common/cmd_mmc.c	2013-12-05 12:17:55.227503948 -0500
@@ -131,7 +131,11 @@
 	mmc = find_mmc_device(curr_device);
 
 	if (mmc) {
-		mmc_init(mmc);
+		if (mmc_init(mmc))
+		{
+			printf("no mmc device at slot %x\n", curr_device);
+			return 1;
+		}
 
 		print_mmcinfo(mmc);
 		return 0;
