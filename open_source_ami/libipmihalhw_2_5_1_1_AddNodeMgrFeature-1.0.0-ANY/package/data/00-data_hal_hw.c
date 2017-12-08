--- .pristine/libipmihalhw-2.5.1-src/data/hal_hw.c Wed Dec 21 04:29:32 2011
+++ source/libipmihalhw-2.5.1-src/data/hal_hw.c Wed Dec 21 04:31:33 2011
@@ -100,6 +100,14 @@
     {HAL_LPCUART_ROUTE_COM_TO_UART,"route_com_to_uart"}, 
     {HAL_LPCUART_ROUTE_UART_TO_UART,"route_uart_to_uart"} 
 }; 
+/*HAL Handles for Node Manager Sensors*/
+#ifdef CONFIG_SPX_FEATURE_INTEL_INTELLIGENT_POWER_NODE_MANAGER
+phalfunc g_HALNMHandle[MAX_NM_HANDLE];
+HAL_Init g_HALNMInit [] =
+{
+    {HAL_NM_SENSOR_REGISTER, "NM_RegisterOneSensor"}
+};
+#endif
 
 /*HAL Handles for PECI*/
 phalfunc g_HALPECIHandle[MAX_PECI_HANDLE];
@@ -179,6 +187,9 @@
     InitHALHandles(g_HALFanPwmHandle,MAX_FANPWM_HANDLE);
     InitHALHandles(g_HALPECIHandle,MAX_PECI_HANDLE);
     InitHALHandles(g_HALLPCUARTHandle,MAX_LPCUART_HANDLE);
+    #ifdef CONFIG_SPX_FEATURE_INTEL_INTELLIGENT_POWER_NODE_MANAGER
+    InitHALHandles(g_HALNMHandle,MAX_NM_HANDLE);
+    #endif
 
     GetHALHandles(I2C_LIB,g_HALI2CInit,g_HALI2CHandle,sizeof(g_HALI2CInit)/sizeof(g_HALI2CInit[0]));
     GetHALHandles(GPIO_LIB,g_HALGPIOInit,g_HALGPIOHandle,sizeof(g_HALGPIOInit)/sizeof(g_HALGPIOInit[0]));
@@ -186,6 +197,9 @@
     GetHALHandles(FANPWM_LIB,g_HALFanInit,g_HALFanPwmHandle,sizeof(g_HALFanInit)/sizeof(g_HALFanInit[0]));
     GetHALHandles(PECI_LIB,g_HALPECIInit,g_HALPECIHandle,sizeof(g_HALPECIInit)/sizeof(g_HALPECIInit[0]));
     GetHALHandles(LPCUART_LIB,g_HALLPCUARTInit,g_HALLPCUARTHandle,sizeof(g_HALLPCUARTInit)/sizeof(g_HALLPCUARTInit[0]));
+    #ifdef CONFIG_SPX_FEATURE_INTEL_INTELLIGENT_POWER_NODE_MANAGER
+    GetHALHandles(NM_LIB,g_HALNMInit,g_HALNMHandle,sizeof(g_HALNMInit)/sizeof(g_HALNMInit[0]));
+    #endif
 
     return 0;
 }
@@ -867,3 +881,37 @@
 
     return 0; 
 } 
+
+#ifdef CONFIG_SPX_FEATURE_INTEL_INTELLIGENT_POWER_NODE_MANAGER
+/**
+ * @fn nm_register_sensor
+ * @brief
+ *
+**/
+int
+nm_register_sensor(hal_t *phal)
+{
+     int ret_val = -1;
+    if(g_HALNMHandle[HAL_NM_SENSOR_REGISTER] != NULL)
+    {
+
+        TCRIT("g_HALNMHandle[HAL_NM_SENSOR_REGISTER found\n");
+
+        ret_val = ((int(*)(hal_t *))g_HALNMHandle[HAL_NM_SENSOR_REGISTER]) (phal);
+    }
+    else
+    {
+        TCRIT("g_HALNMHandle[HAL_NM_SENSOR_REGISTER] is NULL\n");
+    }
+
+    /* Check if NM issue command operation is success or not */
+    if (ret_val == -1)
+    {
+        TCRIT("Hal_hw.c: nm_register_sensor failed\n" );
+        phal->status_code = HAL_ERR_READ;
+        return -1;
+    }
+
+      return 0;
+}
+#endif
