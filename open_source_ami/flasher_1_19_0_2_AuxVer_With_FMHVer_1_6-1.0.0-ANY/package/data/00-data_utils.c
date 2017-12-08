--- .pristine/flasher-1.19.0-src/data/utils.c Mon Sep 19 18:25:28 2011
+++ source/flasher-1.19.0-src/data/utils.c Mon Sep 19 18:42:17 2011
@@ -476,7 +476,10 @@
 	TDBG("VerifyImageIntegrity passed\n");
 		
 	ModulePtr = &(FMHPtr->Module_Info);
-	sprintf ((char *)ImgVeriData->NewImageVersion, "%d.%d.%.2s", ModulePtr->Module_Ver_Major,ModulePtr->Module_Ver_Minor,ModulePtr->Module_Ver_Aux);
+	if(FMH_MAJOR>=1 && FMH_MINOR>=6)
+		sprintf ((char *)ImgVeriData->NewImageVersion, "%d.%d.%.2s", ModulePtr->Module_Ver_Major,ModulePtr->Module_Ver_Minor,ModulePtr->Module_Ver_Aux);
+	else
+		sprintf ((char *)ImgVeriData->NewImageVersion, "%d.%d", ModulePtr->Module_Ver_Major,ModulePtr->Module_Ver_Minor);
 
 /**
  * Changed the FlashStart to UsedFlashStart
