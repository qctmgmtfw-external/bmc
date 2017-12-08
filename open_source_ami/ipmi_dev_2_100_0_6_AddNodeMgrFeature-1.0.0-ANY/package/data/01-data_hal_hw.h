--- .pristine/ipmi_dev-2.100.0-src/data/hal_hw.h Tue Dec 20 22:01:30 2011
+++ source/ipmi_dev-2.100.0-src/data/hal_hw.h Wed Dec 21 00:40:13 2011
@@ -32,6 +32,7 @@
 #define FANPWM_LIB "/usr/local/lib/libpwmtach.so"
 #define PECI_LIB "/usr/local/lib/libpeci.so"
 #define LPCUART_LIB "/usr/local/lib/liblpcuart.so"
+#define NM_LIB "/usr/local/lib/libipminmsupport.so"
 #define MAX_HAL_HANDLE_NAME 256
 #define FAN_MAX_NO 16
 
@@ -111,7 +112,13 @@
     HAL_LPCUART_ROUTE_UART_TO_UART, 
     MAX_LPCUART_HANDLE 
 }HAL_LPCUART_HANDLE; 
-
+#ifdef CONFIG_SPX_FEATURE_INTEL_INTELLIGENT_POWER_NODE_MANAGER
+typedef enum
+{
+    HAL_NM_SENSOR_REGISTER = 0,
+    MAX_NM_HANDLE
+}HAL_NM_HANDLE;
+#endif
 typedef struct
 {
     INT16U HALHandleNum;
@@ -151,7 +158,9 @@
 extern int fan_speed_set (hal_t *phal); 
 extern int fan_set_connect (hal_t *phal);
 extern int peci_read_temp(hal_t *phal); 
-extern int lpcuart_route_set(hal_t *phal);
+#ifdef CONFIG_SPX_FEATURE_INTEL_INTELLIGENT_POWER_NODE_MANAGER
+extern int nm_register_sensor(hal_t *phal);
 
+#endif
 #endif //#define HAL_HW_H
 
