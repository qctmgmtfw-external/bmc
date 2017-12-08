--- .pristine/libipmi-2.38.0-src/data/sensor_helpers.h Fri Sep 16 23:41:45 2011
+++ source/libipmi-2.38.0-src/data/sensor_helpers.h Fri Sep 16 23:58:36 2011
@@ -30,5 +30,6 @@
 					  int timeout );
 int get_sdr_sensor_state( u8 sensor_reading, u8 *sdr_buffer, u16 *current_state );
 int GetSensorState(IPMI20_SESSION_T *pSession, INT8U sensor_reading, u8 *sdr_buffer, u16 *current_state, int timeout  );
-
+int ipmi_conv_reading(u8 SDRType, u8 raw_reading, float *converted_reading ,u8 Min,u8 Max,u8 Units1,u8 Lin,u8 Mval,u8 Bval,u8 M_Tol,u8 B_Acc,u8 R_B_Ex);
+int SensorState( INT8U sensor_reading, u16 *current_state,u8 SDRType,u8 Lin,u8 Units1,u8 EvtType,u8 AssertEvt1,u8 AssertEvt2,u8 DeassertEvt1,u8 DeassertEvt2);
 #endif
