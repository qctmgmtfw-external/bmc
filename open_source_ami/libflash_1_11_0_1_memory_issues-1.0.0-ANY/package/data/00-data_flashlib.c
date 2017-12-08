--- .pristine/libflash-1.11.0-src/data/flashlib.c Wed Sep 14 14:28:17 2011
+++ source/libflash-1.11.0-src/data/flashlib.c Wed Sep 14 15:14:46 2011
@@ -794,11 +794,17 @@
 	unsigned long EraseBlkSize = ActualEraseBlockSize();
 
 	OneEBlock = malloc(EraseBlkSize);
+	if(OneEBlock == NULL)
+	{
+		TCRIT("Error in allocating memory \n");
+		return;
+	}
 
 	/*  Read EraseBlockSize bytes at a time */
 	if (0 != ReadBlock((fp->UsedFlashSize - EraseBlkSize), (char *)OneEBlock))
 	{
 		TCRIT ("Error reading device %s\n",strerror(errno));
+		free(OneEBlock);
 		return;
 	}
 
@@ -807,6 +813,7 @@
 	if (FMHPtr == NULL)
 	{
 		TCRIT ("Can not find FMH\n");
+		free(OneEBlock);
 		return;
 	}
 	/* FMH module found!!    */
