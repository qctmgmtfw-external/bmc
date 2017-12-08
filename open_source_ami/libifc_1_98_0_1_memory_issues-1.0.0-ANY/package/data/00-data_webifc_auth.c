--- .pristine/libifc-1.98.0-src/data/webifc_auth.c Wed Sep 14 14:34:06 2011
+++ source/libifc-1.98.0-src/data/webifc_auth.c Wed Sep 14 17:06:49 2011
@@ -361,6 +361,7 @@
 	WEBPAGE_WRITE_BEGIN();
 	WEBPAGE_WRITE_JSON_BEGIN(GETADCONFIG);
 
+	memset((char *)&adcfg, 0, sizeof(AD_Config_T));
 	retval=GetADConfig (&adcfg);
 	if(retval != 0)
 	{
@@ -402,6 +403,7 @@
 	AD_DOMAINSRVR2 = WP_GET_VAR_STR(AD_DOMAINSRVR2);
 	AD_DOMAINSRVR3 = WP_GET_VAR_STR(AD_DOMAINSRVR3);
 
+	memset((char *)&adcfg, 0, sizeof(AD_Config_T));
 	retval=GetADConfig (&adcfg);
 	if(retval != 0)
 	{
