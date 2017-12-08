--- .pristine/libsyslogconf-1.4.0-src/data/syslogconf.c	Tue May 13 16:25:00 2014
+++ source/libsyslogconf-1.4.0-src/data/syslogconf.c	Wed May 14 10:57:55 2014
@@ -357,7 +357,7 @@
 			"\tsize=%lu\n"
 			"\tnocompress\n"
 			"\tpostrotate\n"
-			"\t\tkill -HUP `cat /var/run/syslogd.pid`\n"
+			"\t\tkill -HUP `cat /var/run/rsyslogd.pid`\n"
 			"\tendscript\n"
 			"}";
 
