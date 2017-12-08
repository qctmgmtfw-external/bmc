--- .pristine/libadvisercfg-1.8.0-src/data/adviser_cfg.c	Tue Jul 10 13:48:34 2012
+++ source/libadvisercfg-1.8.0-src/data/adviser_cfg.c	Wed Jul 11 12:27:31 2012
@@ -54,19 +54,26 @@
 	// If an entry already exists, then we will remove the entry 
 	if ((ret == 0) && (secure == 1))
 	{
-		system(ADVISER_RESTART);
-		AddStunnelEntry(KVM_SERVICE_NAME, 1);
+		ret=AddStunnelEntry(KVM_SERVICE_NAME, 1);
+		if(ret!=0)
+		{
+			return -1;
+		}
 	}
 	else if ((ret == 1) && (secure == 0))
 	{
-		system(ADVISER_RESTART);
-		RemoveStunnelEntry(KVM_SERVICE_NAME, 1);
+		
+		ret=RemoveStunnelEntry(KVM_SERVICE_NAME, 1);
+		if(ret!=0)
+		{
+			return -1;
+		}
 	}
 	else
 	{
 		return 0;
 	}
-
+	system(ADVISER_RESTART);
 	return 0;
 }
 
@@ -75,7 +82,9 @@
 {
     dictionary *d = NULL;
     int err_value = 0xFFFFFF;
-    char	temp[64];
+    char *err_str ="";
+    char	temp[64]={0};
+    memset(pAdviserCfg,0,sizeof(AdviserCfg_T));
 
     d = iniparser_load(ADVISER_CFG_FILE);
 
@@ -87,8 +96,10 @@
 
     sprintf(temp, "%s:%s", ADVISER_SECTION_NAME, STR_MOUSE_MODE);
     pAdviserCfg->mouse_mode = dictionary_getint(d, temp, err_value);
+    sprintf(temp, "%s:%s", ADVISER_SECTION_NAME, STR_KEYBOARD_LAYOUT);
+    strncpy(pAdviserCfg->keyboard_layout,dictionary_get(d,temp,err_str),KEYBOARD_LANG_SIZE-1);
 
-    if( pAdviserCfg->mouse_mode == err_value )
+    if( pAdviserCfg->mouse_mode == err_value || strcmp(pAdviserCfg->keyboard_layout,err_str)==0)
     {
         TEMERG("Unable to read the values. %s may be corrupt", ADVISER_CFG_FILE);
         iniparser_freedict(d);
@@ -112,9 +123,10 @@
 		TEMERG("Unable to load Configuration file for writing: %s", ADVISER_CFG_FILE);
 		return -2;
 	}
-	
+	pAdviserCfg->keyboard_layout[KEYBOARD_LANG_SIZE-1]='\0';
 	fprintf(fp, "[%s]\n", ADVISER_SECTION_NAME);
 	fprintf(fp, "%s=%d\n", STR_MOUSE_MODE, pAdviserCfg->mouse_mode);
+    fprintf(fp, "%s=%s\n", STR_KEYBOARD_LAYOUT,pAdviserCfg->keyboard_layout);
 	fclose(fp);
 	
 	system(ADVISER_RESTART_CONF);
@@ -122,6 +134,27 @@
 	return 0;
 }
 
+int SetMouseModeCfg(unsigned int mouse_mode)
+{
+	
+    AdviserCfg_T AdviserCfg;
+    GetAdviserCfg( &AdviserCfg);
+    AdviserCfg.mouse_mode=mouse_mode;
+    SetAdviserCfg(&AdviserCfg);
+    return 0;
+}
+int SetKeyLayoutCfg(char *keyboard_layout)
+{
+	AdviserCfg_T AdviserCfg;
+    GetAdviserCfg(&AdviserCfg);
+    /*Not allowing same language to be set*/
+    if(strncmp(AdviserCfg.keyboard_layout,keyboard_layout,KEYBOARD_LANG_SIZE-1)!=0)
+    {
+    strncpy(AdviserCfg.keyboard_layout,keyboard_layout,KEYBOARD_LANG_SIZE-1);
+    SetAdviserCfg(&AdviserCfg);
+    }
+	return 0;
+}
 
 
 
