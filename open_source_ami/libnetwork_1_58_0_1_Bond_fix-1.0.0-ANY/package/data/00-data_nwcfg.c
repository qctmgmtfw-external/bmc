--- .pristine/libnetwork-1.58.0-src/data/nwcfg.c Wed Sep  7 19:36:38 2011
+++ source/libnetwork-1.58.0-src/data/nwcfg.c Wed Sep  7 20:04:42 2011
@@ -1517,6 +1517,42 @@
 }
 
 /*
+ * @fn CheckBondSlave
+ * @brief This function will check the given interfaces slave status
+ * @param EthIndex[in] interface's Ethindex value
+ * @returns 1 if the interface is a slave of any bond interface, otherwise 0
+ */
+int CheckBondSlave(INT8U EthIndex)
+{
+    int i;
+
+    if(nwGetBondCfg() != 0)
+    {
+        TCRIT("Error in getting slave interface's configuration\n");
+        return -1;
+    }
+
+    for(i = 0; i < sizeof(Ifcnametable)/sizeof(IfcName_T); i++)
+    {
+        if(Ifcnametable[i].Index == EthIndex)
+        {
+            if(m_NwCfgInfo[Ifcnametable[i].Index].Slave == 1)
+            {
+                TDBG("Given interface is slave of bond interface index %d\n",m_NwCfgInfo[Ifcnametable[i].Index].BondIndex);
+                return 1;
+            }
+            else
+            {
+                TDBG("Given interface is not a slave of any bond interface\n");
+                return 0;
+            }
+        }
+    }
+
+    return 0;
+}
+
+/*
  * @fn UpdateBondInterface
  * @brief This function will update the interface file based on bonding Configuration
  * @param Enable [in] Enable/Disable
@@ -1833,15 +1869,21 @@
     {
 
         /*up all the interfaces*/
-        if( m_NwCfgInfo[Ethindex].VLANID != 0)
-        {
-            sprintf(IfupdownStr,"%s%d",VLAN_NETWORK_CONFIG_FILE,Ethindex);
-        }
-        else
-        {
-            sprintf(IfupdownStr,"%s bond%d",IfUp_Ifc,BondCfg->BondIndex);
-        }
-        safe_system(IfupdownStr);
+        for(i = 0; i < sizeof(Ifcnametable)/sizeof(IfcName_T); i++)
+        {
+            if(Ifcnametable[i].Enabled != 1)
+                continue;
+
+            if( m_NwCfgInfo[Ifcnametable[i].Index].VLANID != 0)
+            {
+                sprintf(IfupdownStr,"%s%d",VLAN_NETWORK_CONFIG_FILE,Ifcnametable[i].Index);
+            }
+            else
+            {
+                sprintf(IfupdownStr,"%s %s",IfUp_Ifc,Ifcnametable[i].Ifcname);
+            }
+            safe_system(IfupdownStr);
+        }
 
         /*detach the slaves*/
         sprintf(EnslaveIface,"%s -d bond%d %s",IfEnslave,BondCfg->BondIndex,BondSlaves);
