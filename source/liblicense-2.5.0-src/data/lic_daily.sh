#!/bin/sh

COUNT_DAYS_FILE=/conf/license/count_days

#increment days count to handle trial licenses
if [ -f $COUNT_DAYS_FILE ]
then
    read count <$COUNT_DAYS_FILE
    echo $count
    if [ $count -ne 0 ]
    then
        let count=$count+1
        echo $count >$COUNT_DAYS_FILE
    fi
fi

#start lecenseapp to stop license expired applications
/etc/init.d/licenseapp.sh manual

