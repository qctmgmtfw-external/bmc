--- u-boot-2013.07/common/cmd_flash.c	2013-12-13 13:49:25.700958498 -0500
+++ uboot.new/common/cmd_flash.c	2013-12-05 12:17:55.223503948 -0500
@@ -302,9 +302,12 @@
 #ifndef CONFIG_SYS_NO_FLASH
 	if (argc == 1) {	/* print info for all FLASH banks */
 		for (bank=0; bank <CONFIG_SYS_MAX_FLASH_BANKS; ++bank) {
-			printf ("\nBank # %ld: ", bank+1);
+			if (flash_info[bank].sector_count != 0)   
+			{
+				printf ("\nBank # %ld: ", bank+1);
 
-			flash_print_info (&flash_info[bank]);
+				flash_print_info (&flash_info[bank]);
+			}
 		}
 		return 0;
 	}
@@ -315,8 +318,11 @@
 			CONFIG_SYS_MAX_FLASH_BANKS);
 		return 1;
 	}
-	printf ("\nBank # %ld: ", bank);
-	flash_print_info (&flash_info[bank-1]);
+	if (flash_info[bank-1].sector_count != 0)   
+	{
+		printf ("\nBank # %ld: ", bank);
+		flash_print_info (&flash_info[bank-1]);
+	}
 #endif /* CONFIG_SYS_NO_FLASH */
 	return 0;
 }
@@ -454,7 +460,7 @@
 }
 #endif /* CONFIG_SYS_NO_FLASH */
 
-static int do_protect(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
+int do_protect(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
 {
 	int rcode = 0;
 #ifndef CONFIG_SYS_NO_FLASH
