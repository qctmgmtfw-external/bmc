--- .pristine/libifc-1.98.0-src/data/webifc_KVMConsole.c	Tue Jul 10 13:51:34 2012
+++ source/libifc-1.98.0-src/data/webifc_KVMConsole.c	Wed Jul 11 12:34:02 2012
@@ -51,10 +51,14 @@
 	int kvmstatus;
 	int vmediastatus;
 	VMediaCfg_T vmedia_cfg;
+	AdviserCfg_T adviser_cfg;
 
 	WEBPAGE_DECLARE_JSON_RECORD(GETREMOTESESSIONCFG)
 	{
 		JSONFIELD(KVMENCRYPTION, JSONFIELD_TYPE_INT),
+#ifdef CONFIG_SPX_FEATURE_KB_LANG_SELECT_SUPPORT		
+		JSONFIELD(KEYBOARDLANG, JSONFIELD_TYPE_STR),
+#endif			
 		JSONFIELD(MEDIAENCRYPTION, JSONFIELD_TYPE_INT),
 		JSONFIELD(VMEDIAATTACH,JSONFIELD_TYPE_INT),
 		JSONFIELD(LMEDIAENABLE,JSONFIELD_TYPE_INT8U)
@@ -62,6 +66,18 @@
 	WEBPAGE_WRITE_BEGIN();
 	WEBPAGE_WRITE_JSON_BEGIN(GETREMOTESESSIONCFG);
 
+	memset(&adviser_cfg, 0, sizeof(AdviserCfg_T));
+
+#ifdef CONFIG_SPX_FEATURE_KB_LANG_SELECT_SUPPORT
+	retval = GetAdviserCfg(&adviser_cfg);
+	if(retval != RPC_HAPI_SUCCESS)
+	{
+		TCRIT("Error in getting Keyboard language configuration::%d\n", retval);
+		goto error_out;
+	}
+#endif
+	
+
 	kvmstatus = GetAdviserSecureStatus();
 	if ((kvmstatus != 0) && (kvmstatus != 1))
 	{
@@ -87,6 +103,9 @@
 
 	WEBPAGE_WRITE_JSON_RECORD(GETREMOTESESSIONCFG,
 									kvmstatus,
+#ifdef CONFIG_SPX_FEATURE_KB_LANG_SELECT_SUPPORT															
+									adviser_cfg.keyboard_layout,
+#endif																	
 									vmediastatus,
 									vmedia_cfg.attach_fd,
 									vmedia_cfg.lmedia_enable
@@ -104,12 +123,24 @@
 	VMediaCfg_T vmedia_cfg;
 
 	WP_VAR_DECLARE(KVMENCRYPTION, int);
+#ifdef CONFIG_SPX_FEATURE_KB_LANG_SELECT_SUPPORT	
+	WP_VAR_DECLARE(KEYBOARDLANG, VARTYPE_STR);
+#endif	
 	WP_VAR_DECLARE(MEDIAENCRYPTION, int);
 	WP_VAR_DECLARE(VMEDIAATTACH, int);
 
 	WEBPAGE_WRITE_BEGIN();
 	WEBPAGE_WRITE_JSON_BEGIN(SETREMOTESESSIONCFG);
 
+#ifdef CONFIG_SPX_FEATURE_KB_LANG_SELECT_SUPPORT
+	KEYBOARDLANG = WP_GET_VAR_STR(KEYBOARDLANG);
+	retval = SetKeyLayoutCfg(KEYBOARDLANG);
+	if(retval != RPC_HAPI_SUCCESS)
+	{
+		TCRIT("Error setting keyboard language:%d\n", retval);
+		goto error_out;
+	}
+#endif
 	KVMENCRYPTION = WP_GET_VAR_INT(KVMENCRYPTION);
 	MEDIAENCRYPTION = WP_GET_VAR_INT(MEDIAENCRYPTION);
 	VMEDIAATTACH = WP_GET_VAR_INT(VMEDIAATTACH);
@@ -120,10 +151,7 @@
 		TCRIT("Error setting Adviser Secure Status\n");
 		goto error_out;
 	}
-	else
-	{
-		system(ADVISER_RESTART);	
-	}
+
 
 	if( SetVmediaSecureStatus(MEDIAENCRYPTION) != 0 )
 	{
@@ -131,10 +159,7 @@
 		TCRIT("Error setting VMedia Secure Status\n");
 		goto error_out;
 	}
-	else
-	{
-		system(ADVISER_RESTART);	
-	}
+
 
 	if( GetVMediaCfg(&vmedia_cfg) != 0 )
 	{
@@ -437,36 +462,20 @@
 WEBIFC_HAPIFN_DEFINE(SetMouseMode)
 {
 	int retVal = 0;
-	AdviserCfg_T adviser_cfg;
+
 
 	WP_VAR_DECLARE(SET_MOUSE_MODE, int);
 	WEBPAGE_WRITE_BEGIN();
 	WEBPAGE_WRITE_JSON_BEGIN(SETMOUSEMODE);
 	SET_MOUSE_MODE = WP_GET_VAR_INT(SET_MOUSE_MODE);
 
-	retVal = GetAdviserCfg(&adviser_cfg);
-	if (retVal != 0)
-	{
-		TCRIT("Error in getting Mouse Mode.\n");
-		goto error_out;
-	}
-
-	TDBG("Got the mouse mode as  %d - %d\n", SET_MOUSE_MODE,adviser_cfg.mouse_mode);
-	//already in the required mode. Don't change mode
-	if (SET_MOUSE_MODE != adviser_cfg.mouse_mode)
-	{
-		adviser_cfg.mouse_mode = SET_MOUSE_MODE;
-		retVal = SetAdviserCfg(&adviser_cfg);
-		if (retVal != 0)
-		{
-			TCRIT("Error in setting Mouse mode to %d \n", SET_MOUSE_MODE );
-			goto error_out;
-		}
-	}
-	else
-		TINFO("Mouse Mode same as earlier. Will not Reboot Card.\n");
-
-error_out:
+	retVal = SetMouseModeCfg(SET_MOUSE_MODE);
+	if (retVal != RPC_HAPI_SUCCESS)
+	{
+		TCRIT("Error in setting Mouse mode to %d \n", retVal);
+	}
+
+
 	WEBPAGE_WRITE_JSON_END(SETMOUSEMODE, retVal);
 	WEBPAGE_WRITE_END();
 
@@ -480,6 +489,7 @@
 	unsigned int user_privileges=0;
 	char  UserName[TOKENCFG_MAX_USERNAME_LEN];
 	char client_ip[TOKENCFG_MAX_CLIENTIP_LEN];
+	memset(session_token,0,MAX_TOKEN_LEN+1);
 
 	WEBPAGE_DECLARE_JSON_RECORD( GETSESSIONTOKEN )
 	{
@@ -565,6 +575,11 @@
 	TDBG ("Filename::%s\n",filename);
 
 	value = (char *)malloc(MAX_FILELINE_LENGTH);
+	if(value == NULL)
+	{
+		retval = -1;
+		goto error_out;
+	}
 	fp = fopen(filename, "r");
 	if (fp != NULL)
 	{
@@ -601,10 +616,10 @@
 	fclose(fp);
 
 error_out:
+	if (value != NULL)
+		free(value);
 	WEBPAGE_WRITE_JSON_END(GETACTIVEXCFG, retval);
 	WEBPAGE_WRITE_END();
-	if (value)
-		free(value);
 	return 0;
 }
 
