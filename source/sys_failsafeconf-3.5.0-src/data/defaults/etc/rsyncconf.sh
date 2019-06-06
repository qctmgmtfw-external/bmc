#!/bin/sh

# This script will use inotifywait to mointor target directory for close write changes.
# And use rsync to synchronize target directory and backup directory.

EXCLUDE_FILE="/etc/rsync-excludes"
RSYNC_TMP="/tmp/rsync_tmp"


test -e $RSYNC_TMP && echo "folder exist" || mkdir $RSYNC_TMP
#sync first time when service is started
rsync -raq --temp-dir=$RSYNC_TMP  --delete --exclude-from $EXCLUDE_FILE $1/ $2/

while true
do
inotifywait -r -e close_write -e modify $1/ >/dev/null 2>&1
rsync -raq --temp-dir=$RSYNC_TMP  --delete --exclude-from $EXCLUDE_FILE $1/ $2/ >/dev/null 2>&1
		
done

