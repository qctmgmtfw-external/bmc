#!/bin/sh

#This is the DDNS update script
#args for this script are as follows
# 1 - option can be 'dereg' or 'reg'
# 2 - the name for dereg or reg
# 3 - The domain name for eg or dereg
# 3 - the IP address to use in case of register
reverse_ipv6()
{
    # An extra dot (".") will be present at the end of the reversed IPv6.
    # Use it accordingly
    REV_IP6=`echo $1 |awk '
    {
        c=split($0, s, ":");
        for(n=0; n<=(8-c); n++)
        {
            zz=zz":0";
        }

        zz=zz":";
        sub ("::", zz, $0);

        c=split($0, s, ":");
        ip6str="";
        for(i=1; i<=8; i++)
        {
            s[i]=substr("000"s[i], length(s[i]));
            ip6str=ip6str s[i];
        }

        revstr = "";
        for (i=1; i<=length(ip6str); i++)
        {
            revstr = substr(ip6str, i, 1) "." revstr;
        }
        {print revstr};
	}'`
}

#echo "We are in nsupdate : "

if [ -z "$2" ] || [ -z "$3" ]; then
    echo "Need IP and Name to Register/Deregister"
    exit
else
    IPv6=`echo $3 | grep :`

    if [ $IPv6 ]; then
            RECORD_TYPE="AAAA"
    else
            RECORD_TYPE="A"
    fi
fi

NSUPDATE_TEMP_FILE="/tmp/nsupdate_temp"
NSUPDATE_TIMEOUT=10
NSUPDATE_BINARY=/usr/local/bin/nsupdate
[ -x $NSUPDATE_BINARY ] || exit 0


case $1 in
      deregister)
      	if [ -z "$2" ] || [ -z "$3" ]; then
        	echo "Need IP and Name for deregistering"
			exit 0
	  	fi
#      	echo "update delete $2 A" > $NSUPDATE_TEMP_FILE
        echo "update delete $2 $RECORD_TYPE $3" > $NSUPDATE_TEMP_FILE
      	echo "send" >> $NSUPDATE_TEMP_FILE
      	#we have to also delete the reverse lookup here of the form update delete rever.ip.x.y.index2event-addr.arpa PTR
        # Deregistration of reverse lookup
        if [ $IPv6 ]; then
            # Delete the reverse lookup for IPv6 address
            # Format : update delete reversed_ip6.ip6.arpa PTR

            reverse_ipv6 $3
            #echo "Reversed IPv6 is " $REV_IP6
            echo "update delete $REV_IP6""ip6.arpa PTR" >> $NSUPDATE_TEMP_FILE
            echo "send" >> $NSUPDATE_TEMP_FILE
            
        else
            # Delete the reverse lookup for IPv4 address
            # Format : update delete reversed_ip.in-addr.arpa PTR

      	reversedip=`echo $3 | awk 'BEGIN { FS = "."; OFS = "." } ; { print $4, $3, $2, $1 }'`
#     	echo "reversed ip is " $reversedip
        #echo "Reversed IPv4 is " $reversedip
      	echo "update delete $reversedip.in-addr.arpa PTR" >> $NSUPDATE_TEMP_FILE
      	echo "send" >> $NSUPDATE_TEMP_FILE
        fi	
      	$NSUPDATE_BINARY  -v $NSUPDATE_TEMP_FILE
		;;
      register)
      	if [ -z "$2" ] || [ -z "$3" ]; then
    	    echo "Need IP and Name for registering"
			exit 0
		fi
		if [ -z "$4" ]; then
#    		echo -n "Using default TTL of one day"
			TTL=86400
	   	else
    		TTL=$4
	   	fi
#		echo "update delete $2 A" > $NSUPDATE_TEMP_FILE
        echo "DNS Registering $2 $3 $RECORD_TYPE $TTL"
        echo "update delete $2 $RECORD_TYPE $3" > $NSUPDATE_TEMP_FILE
       	echo "send" >> $NSUPDATE_TEMP_FILE
       	#Dont need to delete reverse lookup here since it is already deleted in deregister and anyways how do we get the old ip?
       	#we may not always have the old ip either
#       	echo "update add $2 $TTL A $3" >> $NSUPDATE_TEMP_FILE
        echo "update add $2 $TTL $RECORD_TYPE $3" >> $NSUPDATE_TEMP_FILE
       	echo "send" >> $NSUPDATE_TEMP_FILE
       	#now add the reverse lookup entry
        if [ $IPv6 ]; then
            reverse_ipv6 $3
            #echo "Reversed IPv6 is " $REV_IP6
            echo "update add $REV_IP6""ip6.arpa $TTL PTR $2" >> $NSUPDATE_TEMP_FILE
            echo "send" >> $NSUPDATE_TEMP_FILE
        else
		reversedip=`echo $3 | awk 'BEGIN { FS = "."; OFS = "." } ; { print $4, $3, $2, $1 }'`
#		echo "reversed ip is " $reversedip
       	echo "update add $reversedip.in-addr.arpa $TTL PTR $2" >> $NSUPDATE_TEMP_FILE
       	echo "send" >> $NSUPDATE_TEMP_FILE
        fi
	    $NSUPDATE_BINARY  -v $NSUPDATE_TEMP_FILE
		;;
      default)
  	 	echo "NSUPDATE:please enter enough args"
	 	;;
esac

exit 0



 

