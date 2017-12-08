#!/bin/sh

# This script will use inotifywait to mointor target directory for close write changes.
# And use rsync to synchronize target directory and backup directory.

DIR="/conf/"
BACKUP_DIR="/bkupconf/"
EXCLUDE_FILE="/etc/rsync-excludes"
RSYNC_TMP="/tmp/rsync_tmp"


test -e $RSYNC_TMP && echo "folder exist" || mkdir $RSYNC_TMP
inotifywait -mr -e close_write $DIR | while read date time dir file; do

       rsync -raq --temp-dir=$RSYNC_TMP  --delete --exclude-from $EXCLUDE_FILE $DIR $BACKUP_DIR

done


