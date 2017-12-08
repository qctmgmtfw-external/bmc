--- .pristine/libifc-1.98.0-src/data/webifc_XportDevice.c Thu Feb 23 11:48:47 2012
+++ source/libifc-1.98.0-src/data/webifc_XportDevice.c Wed Mar  7 20:15:38 2012
@@ -1995,14 +1995,17 @@
 	
 	BondIface bondCfg;
 
-	WP_VAR_DECLARE(BOND_ENABLE, int);
-	WP_VAR_DECLARE(BOND_IFC, int);
+	WP_VAR_DECLARE(BOND_ENABLE, VARTYPE_INT);
+	WP_VAR_DECLARE(BOND_IFC, VARTYPE_INT);
+	WP_VAR_DECLARE(AUTO_CONF, VARTYPE_INT);
 
 	WEBPAGE_WRITE_BEGIN();
 	WEBPAGE_WRITE_JSON_BEGIN(SETNWBONDCFG);
 
 	BOND_ENABLE = WP_GET_VAR_INT(BOND_ENABLE);
 	BOND_IFC = WP_GET_VAR_INT(BOND_IFC);
+	AUTO_CONF = WP_GET_VAR_INT(AUTO_CONF);
+
 	if (BOND_IFC == 0x3)
 	{
 		BOND_IFC = 0xFF;
@@ -2023,6 +2026,7 @@
 		bondCfg.MiiInterval = DEFAULT_MII_INTERVAL;
 		bondCfg.BondMode = BOND_ROUND_ROBIN;
 		bondCfg.Slaves = 0x3;		//At present only 2 interfaces is available for bonding (eth0 and eth1) so 0000 0011 -> 0x3
+		bondCfg.AutoConf = AUTO_CONF;
 
 		retval = LIBIPMI_HL_SetBondEnable(&((wc_get_ipmi_session(wp))->IPMISession),
 			(INT8U *)&bondCfg, DEFAULT_IPMITIMEOUT);
