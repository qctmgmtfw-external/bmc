--- .pristine/webui-1.122.0-src/data/MDS-Workspace/www/impl/add_mod_pef_imp.js	Mon Oct  1 12:55:37 2012
+++ source/webui-1.122.0-src/data/MDS-Workspace/www/impl/add_mod_pef_imp.js	Mon Oct  8 14:46:48 2012
@@ -195,7 +195,7 @@
 }
 
 /* This function loads dynamic checkboxes based on sensor type, sensor name and event options choosen */
-function loadSensorEvents()
+/*function loadSensorEvents()
 {
 	if (eventOpt.value == 1) 		//Get the sensor type and load corresponding events as checkbox 
 	{
@@ -229,6 +229,53 @@
 		sensorEvent.innerHTML = "";
 		sensorEventRow.style.display = "none";
 	}
+}*/
+
+function loadSensorEvents()
+{
+	if (eventOpt.value != 1)
+	{
+		sensorEvent.innerHTML = "";
+		sensorEventRow.style.display = "none";
+		return;
+	}
+
+	var tmp_sensortype = 0xFF;
+	var tmpSensorEventType = 0x00;
+
+	if (sensorType.value != 0xFF)
+	{
+		tmp_sensortype = sensorType.value;
+
+		if (sensorName.value != 0xFF)
+			tmpSensorEventType = GetSensorEventReadingType(sensorName.value);
+	}
+	else
+	{
+		if (sensorName.value != 0xFF)
+		{
+			tmp_sensortype = getSensorType(sensorName.value);
+			tmpSensorEventType = GetSensorEventReadingType(sensorName.value);
+		}
+	}
+
+	if (tmp_sensortype == 0xFF || 
+		(tmp_sensortype != 0xFF && tmpSensorEventType == 0x00)) return;
+
+	sensorEventRow.style.display = "";
+	sensorEvent.innerHTML = "";
+
+	if (tmp_sensortype > 0 && tmp_sensortype < 5)		//Threshold sensors range is 0x1 to 0x4
+		sensorEvent.innerHTML = fillThresholdEvents(tmpSensorEventType);
+	else if (tmp_sensortype > 4 && tmp_sensortype < 45)		//Other sensors range is 0x5 to 0x2C
+	{
+		if (tmpSensorEventType == 0x6F)
+			sensorEvent.innerHTML = fillDiscreteEvents(tmp_sensortype);
+		else if (tmpSensorEventType >= 0x01 && tmpSensorEventType <= 0x0C)
+			sensorEvent.innerHTML = fillThresholdEvents(tmpSensorEventType);
+	}
+	else				//OEM Sensors, get the Sensor event strings using PDK Hooks
+		sensorEventRow.style.display = "none";
 }
 
 function initPowerAction()
@@ -286,6 +333,22 @@
 	return sensortype;
 }
 
+function GetSensorEventReadingType(sensorNum)
+{
+	var eventReadingType = 0;
+
+	for (var i = 0; i < SENSORINFO_DATA.length; i++)
+	{
+		if (SENSORINFO_DATA[i].SensorNumber == sensorNum)
+		{
+			eventReadingType = SENSORINFO_DATA[i].SensorEventType;
+			break;
+		}
+	}
+	
+	return eventReadingType;
+}
+
 /* This function fills the available Sensor types in SensorType selectbox */
 function fillSensorType()
 {
@@ -333,13 +396,13 @@
 	var index = 0;
 	eventOpt.innerHTML = '';
 	eventOpt.add(new Option(eLang.getString('common','STR_PEF_ALL_EVENTS'), 0),isIE?index++:null);
-	if (sensorType.value != 0xFF || sensorName.value != 0xFF)
+	if (sensorType.value != 0xFF && sensorName.value != 0xFF)
 		eventOpt.add(new Option(eLang.getString('common','STR_PEF_SENSOR_EVENTS'), 1),isIE?index++:null);
 	//loadSensorEvents();
 }
 
 /* This function loads 12checkboxes for threshold sensors */
-function fillThresholdEvents()
+/*function fillThresholdEvents()
 {
 	var sensor_spec = eLang.getString('common','STR_SENSOR_THRESHOLD');
 	var sensorThreshold = "<table>"; 
@@ -351,6 +414,31 @@
 			"<td><input type='checkbox' id='_thres_bit" + (i+1) + "'/>" +
 			"<label for='_thres_bit" + (i+1) + "'>" + eLang.getString('common','STR_PEF_EVENT_HIGH') + "</label></td></tr>";
 	}
+	sensorThreshold += "</table>";
+	return (sensorThreshold);
+}*/
+
+function fillThresholdEvents(sensorEventType)
+{
+	var sensor_spec = eLang.getString('event', sensorEventType);
+	var sensorThreshold = "<table>"; 
+
+	for (var i = 0; i < sensor_spec.length; i+=2)
+	{
+		sensorThreshold += "<tr><td><input type='checkbox' id='_thres_bit" + i + "'/>" + 
+						   "<label for='_thres_bit" + i + "'>" + sensor_spec[i] + "</label></td>";
+
+		j = i + 1;
+
+		if (j != sensor_spec.length)
+		{
+			sensorThreshold += "<td><input type='checkbox' id='_thres_bit" + j + "'/>" +
+							   "<label for='_thres_bit" + j + "'>" + sensor_spec[j] + "</label></td>";
+		}
+
+		sensorThreshold += "</tr>";
+	}
+
 	sensorThreshold += "</table>";
 	return (sensorThreshold);
 }
