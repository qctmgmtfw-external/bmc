--- u-boot-2013.07/common/board_f.c	2013-12-13 13:49:25.700958498 -0500
+++ uboot.new/common/board_f.c	2013-12-05 12:17:55.223503948 -0500
@@ -713,20 +713,21 @@
 #ifdef CONFIG_SYS_EXTBDINFO
 static int setup_board_extra(void)
 {
-	bd_t *bd = gd->bd;
 
+#if defined(CONFIG_405GP) || defined(CONFIG_405EP) || \
+		defined(CONFIG_440EP) || defined(CONFIG_440GR) || \
+		defined(CONFIG_440EPX) || defined(CONFIG_440GRX)
+	bd_t *bd = gd->bd;
 	strncpy((char *) bd->bi_s_version, "1.2", sizeof(bd->bi_s_version));
 	strncpy((char *) bd->bi_r_version, U_BOOT_VERSION,
 		sizeof(bd->bi_r_version));
 
 	bd->bi_procfreq = gd->cpu_clk;	/* Processor Speed, In Hz */
 	bd->bi_plb_busfreq = gd->bus_clk;
-#if defined(CONFIG_405GP) || defined(CONFIG_405EP) || \
-		defined(CONFIG_440EP) || defined(CONFIG_440GR) || \
-		defined(CONFIG_440EPX) || defined(CONFIG_440GRX)
 	bd->bi_pci_busfreq = get_PCI_freq();
 	bd->bi_opbfreq = get_OPB_freq();
 #elif defined(CONFIG_XILINX_405)
+	bd_t *bd = gd->bd;
 	bd->bi_pci_busfreq = get_PCI_freq();
 #endif
 
