--- .pristine/ipmi_dev-2.100.0-src/data/hal_hw.h Wed Dec 21 23:00:13 2011
+++ source/ipmi_dev-2.100.0-src/data/hal_hw.h Wed Dec 21 23:07:57 2011
@@ -158,9 +158,10 @@
 extern int fan_speed_set (hal_t *phal); 
 extern int fan_set_connect (hal_t *phal);
 extern int peci_read_temp(hal_t *phal); 
+extern int lpcuart_route_set(hal_t *phal);
 #ifdef CONFIG_SPX_FEATURE_INTEL_INTELLIGENT_POWER_NODE_MANAGER
 extern int nm_register_sensor(hal_t *phal);
+#endif
 
-#endif
 #endif //#define HAL_HW_H
 
