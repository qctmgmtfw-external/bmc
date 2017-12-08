--- .pristine/libipmi-2.38.0-src/data/sdr_cache.c Wed Sep 14 14:28:18 2011
+++ source/libipmi-2.38.0-src/data/sdr_cache.c Wed Sep 14 15:27:59 2011
@@ -205,6 +205,7 @@
 	{
 		TCRIT("Null buffer passed to ReadSDRCache\n");
 		*buff_size = sdr_only_size;
+        fclose(sdrfile);
 		return ERR_SDRCACHE_INVALID_ARGS;
 	}
 
