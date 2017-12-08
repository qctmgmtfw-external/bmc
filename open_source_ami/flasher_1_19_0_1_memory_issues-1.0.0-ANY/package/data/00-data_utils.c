--- .pristine/flasher-1.19.0-src/data/utils.c Wed Sep 14 14:28:40 2011
+++ source/flasher-1.19.0-src/data/utils.c Wed Sep 14 16:20:09 2011
@@ -96,9 +96,11 @@
 int GetSysCtlUlong (const char *TagName, unsigned long *SysVal)
 {
     unsigned long RetVal;
+    if (!SysVal)
+        return 1;
     FILE* SysFile = fopen (TagName, "r");
 
-    if ((!SysFile) || (!SysVal))
+    if (!SysFile)
         return 1;
 
     fscanf (SysFile, "%lu", &RetVal);
@@ -344,6 +346,8 @@
 	unsigned long ImgChksum = Mod->Module_Checksum;
 	unsigned char theBlock = 0;
 
+	if(TmpBuff == NULL)
+		return EIMGREAD;
 	//printf("imgesz = %x\n",ImgSize);
 	BeginCRC32 (&CRC32Val);
 	
@@ -366,6 +370,7 @@
 		if (ReadCount == -1)
 		{
 			TCRIT ("Error reading device for calculating checksum\n");
+			if(TmpBuff) free(TmpBuff);
 			return EIMGREAD;
 		}
 		//printf("ReadCount = %d\n",ReadCount);
@@ -860,10 +865,21 @@
 	int ReadImg=0, ReadMtd=0;
 	unsigned int BlockNum = 1;
 	char Msg[100];
-	unsigned char *TmpBuff = malloc(EraseBlockSize);
-	unsigned char *MtdBuff = malloc(EraseBlockSize);
+	unsigned char *TmpBuff = NULL;
+	unsigned char *MtdBuff = NULL;
 	int TotalBlocks = ImgSize/EraseBlockSize;
 
+	TmpBuff = malloc(EraseBlockSize);
+	if (TmpBuff == NULL){
+		TCRIT ("Error allocating memory.\n");
+		return 1;
+	}
+	MtdBuff = malloc(EraseBlockSize);
+	if (MtdBuff == NULL){
+		TCRIT ("Error allocating memory.\n");
+		free(TmpBuff);
+		return 1;
+	}
 
 #ifdef CONFIG_SPX_FEATURE_GLOBAL_DUAL_IMAGE_SUPPORT
        /* if two images are exists in same bank*/
