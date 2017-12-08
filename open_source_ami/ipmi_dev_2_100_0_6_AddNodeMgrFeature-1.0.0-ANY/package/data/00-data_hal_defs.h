--- .pristine/ipmi_dev-2.100.0-src/data/hal_defs.h Tue Dec 20 22:01:30 2011
+++ source/ipmi_dev-2.100.0-src/data/hal_defs.h Wed Dec 21 00:41:55 2011
@@ -26,6 +26,7 @@
 #include "SensorMonitor.h"
 
 #define MAX_INT_HNDLR_NAME  25
+#define MAX_NM_HNDLR_NAME  25
 
 /**
  * @brief HAL Error code definitions
@@ -194,6 +195,20 @@
     unsigned char int_input_data;
 }hal_intsensor_t;
 
+#ifdef CONFIG_SPX_FEATURE_INTEL_INTELLIGENT_POWER_NODE_MANAGER
+typedef struct
+{
+    unsigned char IsNMconfig;
+    unsigned char Sensor_Num;
+    unsigned char SensorType;
+    unsigned char NetFnLUN;
+    unsigned char cmd;
+    u16 NM_SensorID;
+    unsigned char res_par;
+    char FillHndlrName[MAX_NM_HNDLR_NAME];
+    char ParseHndlrName[MAX_NM_HNDLR_NAME];
+}hal_nmsensor_t;
+#endif
 /**
  * @brief This structure necessary hardware information to access any device
 **/
@@ -229,6 +244,10 @@
     hal_peci_t  peci;            /* Hold PECI information Added by winston for peci */
     hal_lpcuart_t lpcuart;            /* Hold LPCUART information Added by jcchiu for lpcuart */
     hal_intsensor_t intsensor;     /* Hold Interrupt Sensor information */
+
+#ifdef CONFIG_SPX_FEATURE_INTEL_INTELLIGENT_POWER_NODE_MANAGER
+    hal_nmsensor_t nmsensor;     /* Hold Node Manager Sensor information */
+#endif
 } hal_t;
 
 
