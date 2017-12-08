--- .pristine/libipmi-2.38.0-src/data/sensor_helpers.c Fri Sep 16 23:41:45 2011
+++ source/libipmi-2.38.0-src/data/sensor_helpers.c Fri Sep 16 23:58:27 2011
@@ -804,4 +804,215 @@
     return( sensor_count );
 }
 
-
+/**
+ * @fn ipmi_conv_reading
+ * @brief Helper function to convert the Sensor related readings 
+ * @params SDRType[in],SensorReading,MinReading,MaxReading,SensorUnits1,Linear fn,M value,B Value, M tolerace, B accuracy,Rand B exponent
+ * @returns converted reading
+ */
+int ipmi_conv_reading(u8 SDRType, u8 raw_reading, float *converted_reading ,u8 Min,u8 Max,u8 Units1,u8 Lin,u8 Mval,u8 Bval,u8 M_Tol,u8 B_Acc,u8 R_B_Ex)
+{
+    uint8 L;
+    int M, B, K1, K2;
+    int raw_number;
+    int sensor_min, sensor_max;
+
+    if( SDRType != FULL_SDR_REC )
+    {
+        /* Not enough info to convert */
+        TDBG( "SDRs of type 0x%02x do not provide conversion information\n",SDRType );
+        *converted_reading = (float)raw_reading;
+        return( -1 );
+    }
+
+    /* Convert raw_reading and SensorMin/MaxReading to correct numeric */
+    /* formats before continuing                                       */
+    raw_number = (int)raw_reading;
+    sensor_min = (int)Min;
+    sensor_max = (int)Max;
+    
+    switch( Units1 >> 6 )
+    {
+        case SDR_READING_UNSIGNED:
+            TDBG( "Sensor reading number format is unsigned\n" );
+            TDBG( "Sensor min: %d\n", sensor_min );
+            TDBG( "Sensor max: %d\n", sensor_max );
+            break;
+
+        case SDR_READING_1SCOMP:
+            /* Do 1's comp conversion */
+            if( raw_number & (u8)0x80 )
+                raw_number = -( ~raw_number & 0xff );
+
+            if( sensor_min & (u8)0x80 )
+                sensor_min = -( ~sensor_min & 0xff );
+
+            if( sensor_max & (u8)0x80 )
+                sensor_max = -( ~sensor_max & 0xff );
+
+            TDBG( "Sensor reading number format is 1's complement: %d\n", raw_number );
+            TDBG( "Sensor min: %d\n", sensor_min );
+            TDBG( "Sensor max: %d\n", sensor_max );
+            break;
+
+        case SDR_READING_2SCOMP:
+            /* Do 2's comp conversion */
+            if( raw_number & (u8)0x80 )
+                raw_number = -( ( ~raw_number & 0xff ) + 1 );
+
+            if( sensor_min & (u8)0x80 )
+                sensor_min = -( ( ~sensor_min & 0xff ) + 1 );
+
+            if( sensor_max & (u8)0x80 )
+                sensor_max = -( ( ~sensor_max & 0xff ) + 1 );
+
+            TDBG( "Sensor reading number format is 2's complement: %d\n", raw_number );
+            TDBG( "Sensor min: %d\n", sensor_min );
+            TDBG( "Sensor max: %d\n", sensor_max );
+            break;
+
+        case SDR_READING_NONANALOG:
+        default:
+            TWARN( "Sensor reading number format is non-analog!\n" );
+            /* Can't convert this one */
+            *converted_reading = (float)raw_reading;
+            return( -1 );
+    }
+
+    /* This sensor is out of bounds.  No meaningful conversion can be done */
+    /* This frequently happens when a sensor is reporting some kind of     */
+    /* error, like a fan stopped/missing.                                  */
+    if( ( raw_number < sensor_min ) || ( raw_number > sensor_max ) )
+    {
+        *converted_reading = 0;
+        return( 0 );
+    }
+
+    /* Linearization type.  Most sensors are already linear, so this is   */
+    /* zero in most cases.  If not, it specifies a way to make the sensor */
+    /* response linear.  Only linear sensors are supported now.           */
+    L = Lin;
+
+    /* Convert M from split 10 bit 2s-complement to our int format */
+    M = (int)Mval;
+    if( ( M_Tol & (u8)0x80 ) != (u8)0 )
+    {
+        /* Number is negative */
+        M = M + ( ( (u16)M_Tol & (u16)0xC0 ) << 2 );
+        M = -( ( ~M & 0x1ff ) + 1 );
+    }
+    else
+    {
+        /* Number is positive */
+        M = M + ( ( (u16)M_Tol & (u16)0xC0 ) << 2 );
+    }
+
+    /* Convert B from split 10 bit 2s-complement to our int format */
+    B = (int)Bval;
+    if( ( B_Acc & (u8)0x80 ) != (u8)0 )
+    {
+        /* Number is negative */
+        B = B + ( ( (u16)B_Acc & (u16)0xC0 ) << 2 );
+        B = -( ( ~B & 0x1ff ) + 1 );
+    }
+    else
+    {
+        /* Number is positive */
+        B = B + ( ( (u16)B_Acc & (u16)0xC0 ) << 2 );
+    }
+
+    /* Convert K1 from 4 bit 2s-complement to our int format */
+    K1 = (int)( R_B_Ex & 0x0f );
+    if( K1 & 0x08 )
+    {
+        /* Number is negative */
+        K1 = -( ( ~K1 & 0x0f ) + 1 );
+    }
+
+    /* Convert K2 from 4 bit 2s-complement to our int format */
+    K2 = (int)( R_B_Ex >> 4 );
+    if( K2 & 0x08 )
+    {
+        /* Number is negative */
+        K2 = -( ( ~K2 & 0x0f ) + 1 );
+    }
+
+    return( ipmi_sensor_reading_conversion( raw_number, L, M, B, K1, K2, converted_reading ) );
+}
+
+/**
+ * @fn SensorState
+ * @brief Helper function to get the sensor state of threshold
+ * @params SensorReading,SensorState,SDRType[in],Linear fn,SensorUnits1,SensorEvtType,AssertionEvtbyte1,AssertionEvtbyte2,DeaasertionEvtbyte1,DeassertionEvtbyte2
+ * @returns Sensor state for FULL SDR Record
+ */
+int SensorState( INT8U sensor_reading, u16 *current_state,u8 SDRType,u8 Lin,u8 Units1,u8 EvtType,u8 AssertEvt1,u8 AssertEvt2,u8 DeassertEvt1,u8 DeassertEvt2)
+{
+    number_type reading_format = Unsigned;
+
+    *current_state = THRESH_UNINITIALIZED;
+
+    if( SDRType == FULL_SDR_REC )
+    {
+        /* For inverse linear sensors, a raw reading of 0 is off the chart. */
+        /* We want a low reading, because this corresponds to what most     */
+        /* people expect for most sensors.  We usually get a raw reading of */
+        /* 0 when the device (like a fan) is not connected, and so          */
+        /* (logically), it is below low thresholds, not above high ones.    */
+        /* So, for purposes of determining our state, use 0xff for the      */
+        /* reading instead of 0.                                            */
+
+        if((Lin == (u8)7 ) && ( sensor_reading == (u8)0))
+        {
+            sensor_reading = (u8)0xff;
+        }
+
+        /* Get the format of the sensor/threshold readings */
+        reading_format = (number_type)( Units1 >> 6 );
+
+        /* If the sensor is threshold based... */
+        if( EvtType == 0x01 )
+        {
+            /*Default Sensor State*/
+            *current_state = THRESH_NORMAL;
+
+            /*Do not Change the sequence of condition*/
+            if((AssertEvt1 & 0x80) == 0x80)
+            *current_state = THRESH_UP_NONCRIT;
+
+            if((AssertEvt2 & 0x02) == 0x02)
+            *current_state = THRESH_UP_CRITICAL;
+
+            if((AssertEvt2 & 0x08) == 0x08)
+            *current_state = THRESH_UP_NON_RECOV;
+
+            if((AssertEvt1 & 0x01 ) == 0x01)
+            *current_state = THRESH_LOW_NONCRIT;
+
+            if((AssertEvt1 & 0x04 ) == 0x04)
+            *current_state = THRESH_LOW_CRITICAL;
+
+            if((AssertEvt1 & 0x10) == 0x10)
+            *current_state = THRESH_LOW_NON_RECOV;
+        } /* if sensor is threshold based */
+        else
+        {
+            /* This sensor is discrete */
+            *current_state = (u16)sensor_reading;
+        }
+    }
+    else if( SDRType == COMPACT_SDR_REC )
+    {
+        if( EvtType > (u8)1 )
+        *current_state = (u16)sensor_reading;
+        else
+        *current_state = THRESH_UNINITIALIZED;
+    }
+    else
+    {
+        *current_state = THRESH_UNINITIALIZED;
+        return( -1 );
+    }
+
+    return( 0 );
+}
