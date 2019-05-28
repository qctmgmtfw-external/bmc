--- u-boot-2013.07/common/main.c	2013-12-13 13:49:25.700958498 -0500
+++ uboot.new/common/main.c	2013-12-13 10:09:07.240960075 -0500
@@ -40,6 +40,10 @@
 
 DECLARE_GLOBAL_DATA_PTR;
 
+#ifdef CONFIG_SIMULATE_ACPI
+void SimulateACPI(void);	
+#endif 
+
 /*
  * Board-specific Platform code can reimplement show_boot_progress () if needed
  */
@@ -243,13 +247,19 @@
 				break;
 			}
 			udelay(10000);
-		} while (!abort && get_timer(ts) < 1000);
+		} while (!abort && get_timer(ts) < CONFIG_SYS_HZ);
 
 		printf("\b\b\b%2d ", bootdelay);
 	}
 
 	putc('\n');
 
+#ifdef CONFIG_SIMULATE_ACPI
+		if (!abort)
+			SimulateACPI();	
+#endif 
+
+
 #ifdef CONFIG_SILENT_CONSOLE
 	if (abort)
 		gd->flags &= ~GD_FLG_SILENT;
@@ -407,6 +417,12 @@
 
 	debug ("### main_loop: bootcmd=\"%s\"\n", s ? s : "<UNDEFINED>");
 
+#ifdef CONFIG_DRAM_ECC
+	printf("DRAM ECC enabled\n");
+#else
+	printf("DRAM ECC disabled\n");
+#endif
+
 	if (bootdelay != -1 && s && !abortboot(bootdelay)) {
 #ifdef CONFIG_AUTOBOOT_KEYED
 		int prev = disable_ctrlc(1);	/* disable Control C checking */
@@ -429,6 +445,65 @@
 }
 #endif /* CONFIG_BOOTDELAY */
 
+#ifdef CONFIG_MEMTEST_ENABLE
+int do_mtest(vu_long *start,vu_long *end,ulong pattern)
+{
+	vu_long *addr;
+	ulong incr=0;
+	int rcode = 0,i;
+	ulong   val;
+	ulong   readback;
+
+	incr = 1;
+	/*Allow only 4 Iteration now*/
+	for (i=0;i<4;i++) {
+		if (ctrlc()) {
+			putc ('\n');
+			return 1;
+		}
+
+		printf ("\rPattern %08lX  Writing..."
+					"%12s"
+					"\b\b\b\b\b\b\b\b\b\b",
+					pattern, "");
+
+		for (addr=start,val=pattern; addr<end; addr++) {
+			*addr = val;
+			val  += incr;
+		}
+
+		puts ("Reading...");
+
+		for (addr=start,val=pattern; addr<end; addr++) {
+			readback = *addr;
+			if (readback != val) {
+				printf ("\nMem error @ 0x%08X: "
+						"found %08lX, expected %08lX\n",
+						(uint)addr, readback, val);
+				rcode = 1;
+			}
+			val += incr;
+		}
+
+		/*
+		* Flip the pattern each time to make lots of zeros and
+		* then, the next time, lots of ones.  We decrement
+		* the "negative" patterns and increment the "positive"
+		* patterns to preserve this feature.
+		*/
+		if(pattern & 0x80000000) {
+			pattern = -pattern; /* complement & increment */
+		}
+		else {
+			pattern = ~pattern;
+		}
+		incr = -incr;
+	}
+	return rcode;
+
+}
+#endif
+
 void main_loop(void)
 {
 #ifndef CONFIG_SYS_HUSH_PARSER
@@ -441,6 +516,13 @@
 	char *p;
 #endif
 
+#ifdef CONFIG_MEMTEST_ENABLE
+	char *mtest;
+	int mRet;
+	vu_long *start,*end;
+	ulong pattern;
+#endif
+
 	bootstage_mark_name(BOOTSTAGE_ID_MAIN_LOOP, "main_loop");
 
 #ifdef CONFIG_MODEM_SUPPORT
@@ -454,6 +536,41 @@
 	}
 #endif  /* CONFIG_MODEM_SUPPORT */
 
+#ifdef CONFIG_MEMTEST_ENABLE
+	/*Doing Memory Test*/
+	mtest = getenv("do_memtest");
+	if(strcmp(mtest ,"1") == 0)
+	{
+		start = (ulong *)CONFIG_SYS_MEMTEST_START;
+		end = (ulong *)CONFIG_SYS_MEMTEST_END;
+		pattern = 0;    /*As of now set the pattern as 0*/
+		mRet = do_mtest(start,end,pattern);
+		if(mRet == 0)
+		{
+			setenv("memtest_pass","yes");
+			/*Save the new value into spi*/
+			saveenv();
+		}
+		else
+		{
+			setenv("memtest_pass","no");
+			/*Save the new value into spi*/
+			saveenv();
+		}
+	}
+	else
+	{
+		mtest=getenv("memtest_pass");
+		if(strcmp(mtest,"idle") != 0)
+		{
+			/*Change the Memory Test staus as idle if the memtest is disabled*/
+			setenv("memtest_pass","idle");
+			/*Save the new value into spi*/
+			saveenv();
+		}
+	}
+#endif
+
 #ifdef CONFIG_VERSION_VARIABLE
 	{
 		setenv("ver", version_string);  /* set version variable */
