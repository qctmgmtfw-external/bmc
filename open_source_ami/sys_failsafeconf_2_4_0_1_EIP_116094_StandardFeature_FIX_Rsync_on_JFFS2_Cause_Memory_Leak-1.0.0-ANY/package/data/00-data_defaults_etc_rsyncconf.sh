--- .pristine/sys_failsafeconf-2.4.0-src/data/defaults/etc/rsyncconf.sh Mon Apr  8 15:29:43 2013
+++ source/sys_failsafeconf-2.4.0-src/data/defaults/etc/rsyncconf.sh Mon Apr  8 15:34:27 2013
@@ -6,11 +6,14 @@
 DIR="/conf/"
 BACKUP_DIR="/bkupconf/"
 EXCLUDE_FILE="/etc/rsync-excludes"
+RSYNC_TMP="/tmp/rsync_tmp"
 
 
+test -e $RSYNC_TMP && echo "folder exist" || mkdir $RSYNC_TMP
 inotifywait -mr -e close_write $DIR | while read date time dir file; do
 
-       rsync -raq --delete --exclude-from $EXCLUDE_FILE $DIR $BACKUP_DIR 
+       rsync -raq --temp-dir=$RSYNC_TMP  --delete --exclude-from $EXCLUDE_FILE $DIR $BACKUP_DIR
 
 done
 
+
