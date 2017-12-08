--- .pristine/libnetwork-1.58.0-src/data/nwcfg.h Wed Sep  7 19:36:38 2011
+++ source/libnetwork-1.58.0-src/data/nwcfg.h Wed Sep  7 20:05:07 2011
@@ -587,6 +587,14 @@
 int CheckIfcEntry(INT8U Index,INT8U IfcType);
 
 /*
+ * @fn CheckBondSlave
+ * @brief This function will check the given interfaces slave status
+ * @param EthIndex[in] interface's Ethindex value
+ * @returns 1 if the interface is a slave of any bond interface, otherwise 0
+ */
+int CheckBondSlave(INT8U EthIndex);
+
+/*
  * @fn CheckIfcLinkStatus
  * @brief This function will check the interface's Link health
  * @param Index [in] index value 
