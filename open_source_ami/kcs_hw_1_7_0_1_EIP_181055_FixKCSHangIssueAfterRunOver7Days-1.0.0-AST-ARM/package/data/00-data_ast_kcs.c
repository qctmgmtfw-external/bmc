--- .pristine/kcs_hw-1.7.0-ARM-AST-src/data/ast_kcs.c Wed Jan 28 13:39:35 2015
+++ source/kcs_hw-1.7.0-ARM-AST-src/data/ast_kcs.c Wed Jan 28 14:22:20 2015
@@ -247,6 +247,18 @@
 	ast_kcs_disable_interrupt();
 }
 
+static void ast_kcs_set_obf_status(u8 channel)
+{
+    /*Setting obf bit based on the platform*/
+#if 0
+    u8 status=0;
+    
+    ast_kcs_read_status (channel, &status);
+    status=status | 0x01;
+    ast_kcs_write_status (channel, status);
+#endif
+}
+
 static kcs_hal_operations_t ast_kcs_hw_ops = {
 	.num_kcs_ch = ast_kcs_num_ch,
 	.enable_kcs_interrupt = ast_kcs_enable_interrupt,
@@ -257,7 +269,8 @@
 	.read_kcs_data_in = ast_kcs_read_data,
 	.write_kcs_data_out = ast_kcs_write_data,
 	.kcs_interrupt_enable_user = ast_kcs_interrupt_enable_user,
-	.kcs_interrupt_disable_user = ast_kcs_interrupt_disable_user
+	.kcs_interrupt_disable_user = ast_kcs_interrupt_disable_user,
+	.kcs_set_obf_status = ast_kcs_set_obf_status
 };
 
 static hw_hal_t ast_kcs_hw_hal = {
