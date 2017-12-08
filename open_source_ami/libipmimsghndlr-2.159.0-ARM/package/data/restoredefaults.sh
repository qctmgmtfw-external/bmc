#####################################################
##########      Restore to defaults     #############
#####################################################


case "$1" in
        restore)
        if [ -f /var/flasher.initcomplete ]
        then
                echo -n "Restoring to default configuration...  "
                rm -rf /conf/*
                cp -Rp /etc/defconfig/* /conf
                if [ $? != 0 ]
                then
                        echo "Failed."
                        exit 1
                else
                        echo "Done."
                fi
                echo -n "Stopping adviserd... "
                /etc/init.d/adviserd.sh stop
                if [ $? != 0 ]
                then
                		echo "Failed."
                else
                		echo "Done."
                fi 
        fi
        ;;
        *)
        echo "Usage: $0 start"
        ;;
esac