--- .pristine/kcs-1.10.0-src/data/kcs.h Wed Jan 28 13:39:19 2015
+++ source/kcs-1.10.0-src/data/kcs.h Wed Jan 28 14:12:35 2015
@@ -132,6 +132,7 @@
 	void (*write_kcs_data_out) (u8 ch_num, u8 data_value);
 	void (*kcs_interrupt_enable_user) (void); 
 	void (*kcs_interrupt_disable_user) (void);
+    void (*kcs_set_obf_status)(u8 ch_num);
 } kcs_hal_operations_t;
 
 typedef struct
