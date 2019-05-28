--- u-boot-2013.07/net/tftp.c	2013-12-13 13:49:25.236958498 -0500
+++ uboot.new/net/tftp.c	2013-12-05 12:17:54.975503948 -0500
@@ -165,10 +165,14 @@
 	int i, rc = 0;
 
 	for (i = 0; i < CONFIG_SYS_MAX_FLASH_BANKS; i++) {
+		if (flash_info[i].size == 0)
+			continue;
 		/* start address in flash? */
 		if (flash_info[i].flash_id == FLASH_UNKNOWN)
 			continue;
-		if (load_addr + offset >= flash_info[i].start[0]) {
+		//BugFix: In cases where Flash is located at lower range than SDRAM, it tftp direct flash will fail.
+		//if (load_addr + offset >= flash_info[i].start[0]) {
+		if ((load_addr + offset >= flash_info[i].start[0]) && (load_addr + offset <= ((flash_info[i].size-1) + flash_info[i].start[0]))) {
 			rc = 1;
 			break;
 		}
@@ -303,8 +307,9 @@
 	time_start = get_timer(time_start);
 	if (time_start > 0) {
 		puts("\n\t ");	/* Line up with "Loading: " */
-		print_size(NetBootFileXferSize /
-			time_start * 1000, "/s");
+//		print_size(NetBootFileXferSize /
+//			time_start * 1000, "/s");
+		print_size(NetBootFileXferSize, "");
 	}
 	puts("\ndone\n");
 	net_set_state(NETLOOP_SUCCESS);
