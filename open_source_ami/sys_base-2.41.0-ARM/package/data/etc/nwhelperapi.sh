#!/bin/sh

#This file can be included in your script by adding the line "source ./thisfile" in your script

# API to add DNSserverip
# Parses /conf/resolv.conf file to see if the dnsserverip already exists. 
# if the Ip already exists, it will ignore
# if the Ip doesnot exist, it will the ip at the end.
# will work for both IPV4 and IPV6
AddDNSServerIP()
{
    API_RESOLV_CONF_FILE=/conf/resolv.conf
    API_DNSIP_ALREADY_EXISTS=0
    if [ -f $API_RESOLV_CONF_FILE ]; then
	    API_CURRENT_DNS_SERVERS=`cat $API_RESOLV_CONF_FILE | grep "nameserver" | awk '{print $2}'`
	    for j in $API_CURRENT_DNS_SERVERS; do
		    if [ "$1" == "$j" ]; then
			    API_DNSIP_ALREADY_EXISTS=1	
		    fi
	    done
    fi
    
    if [ $API_DNSIP_ALREADY_EXISTS == 0 ]; then
	    echo "nameserver " $1 >> $API_RESOLV_CONF_FILE
    fi
}

# API to add DNS Domain name
# Parses /conf/resolv.conf file to see if the domainname already exists. 
# if the domainname already exists, it will ignore
# if the domainname doesnot exist, it will add the domainname at the end.
# will work for both IPV4 and IPV6
AddDNSDomainName()
{
    API_RESOLV_CONF_FILE=/conf/resolv.conf
    API_DNSNAME_ALREADY_EXISTS=0
    if [ -f $API_RESOLV_CONF_FILE ]; then
	    API_CURRENT_DNS_NAME=`cat $API_RESOLV_CONF_FILE | grep "search" | awk '{print $2}'`
	    for j in $API_CURRENT_DNS_NAME; do
		    if [ "$1" == "$j" ]; then
			    API_DNSNAME_ALREADY_EXISTS=1	
		    fi
	    done
    fi
    
    if [ $API_DNSNAME_ALREADY_EXISTS == 0 ]; then
	    echo "search " $1 >> $API_RESOLV_CONF_FILE
    fi
}

# API to add NTP server name
# Parses /conf/ntp.conf file to see if the servername already exists. 
# if the servername already exists, it will ignore
# if the servername doesnot exist, it will add the servername at the end.
# will work for both IPV4 and IPV6
AddNTPServerName()
{
    echo "AddNTPServerName called.........!"
    API_NTP_CONF_FILE=/conf/ntp.conf
    API_NTPSERVER_ALREADY_EXISTS=0
    if [ -f $API_NTP_CONF_FILE ]; then
	    API_CURRENT_SERVER_NAME=`cat $API_NTP_CONF_FILE | grep "server" | awk '{print $2}'`
	    #echo "API_CURRENT_SERVER_NAME= $API_CURRENT_SERVER_NAME"
	    for j in $API_CURRENT_SERVER_NAME; do
		    if [ "$1" == "$j" ]; then
			    API_NTPSERVER_ALREADY_EXISTS=1	
			    echo "NTP server Already exist......!"
		    fi
	    done
    else
	#echo "No ntp.conf found........!"
	API_NTPSERVER_ALREADY_EXISTS=1
    fi
    
    if [ $API_NTPSERVER_ALREADY_EXISTS == 0 ]; then
	    echo "server " $1 >> $API_NTP_CONF_FILE
	    #echo "NTP server added in ntp.conf......!"
    fi
}


# API to add/Replace IPv4/IPv6 DNSserver addresses
# Parses /conf/resolv.conf file to add/replace/append the dnsserver IP. 
# if the entry present, then just replace it.
# if the DNS entry doesnot exist, it will append the entry at the end.
# will work for both IPV4 and IPV6
AddDNSServerIP_New()
{
    API_RESOLV_CONF_FILE=/conf/resolv.conf
    API_DNSIP_ALREADY_EXISTS=0
    . /conf/dns.conf
    if [ -f $API_RESOLV_CONF_FILE ]; then

	#find out whether the given IP address is IPv4 or IPv6 address
	#then select the ARG value according to that for IPv4 it is (.) and for IPv6 it is (:)
	#According to the given IP, select the nameserver values (only IPv4 or IPv6 addresses) from resolv.conf file.
	#then replace the new values for the existing entries. i.e. DNS1 address in first place and DNS2 at second place.
	#the count variable and the arguement 2 is used for this.

            ARG=`echo $1 | grep "\."`
            #echo $ARG
            if  [ -z "$ARG" ]; then
                ARG="\:"
            else
                ARG="\."
            fi
            #echo "ARG - $ARG"

            #count used to select the appropriate nameserver (DNS1 or DNS2).
            COUNT=0
            ONE=1

            #select the list of IPv4/IPv6 DNS servers available in resolv.conf file.
            API_CURRENT_DNS_SERVERS=`cat $API_RESOLV_CONF_FILE | grep "nameserver" | awk '{print $2}' | grep $ARG`

            for j in $API_CURRENT_DNS_SERVERS; do
                COUNT=$(( $COUNT+$ONE ))
                #echo "j - $j"
                #echo "COUNT - $COUNT"
                
		    	#check whether count reached to correct nameserver.
		    	if [ $COUNT == $2 ]; then
                	TMP=`echo $j | grep "$ARG"`
                	#echo "TMP - $TMP"
			  		
                    #check whether the IPv4/IPv6 entries are not present in the file. If null the entries will be appended.
                	if ! [ -z "$TMP" ]; then
                    	#echo "\$j - $j"
                    	#echo "\$1 - $1"
                        
                        #find out which line of DNS nameserver entries that we want to replaced with the new value
                        VAL_LINE=""
                        VAL_LINE=`grep -n nameserver $API_RESOLV_CONF_FILE | awk 'NR == '"$2"' {print $0}' | cut -d: -f1`

                        #replace the existing DNS entry with new values
                        #echo "sed -i 's/$j/$1/' /conf/resolv.conf"
                        sed -i "$VAL_LINE"s/$j/$1/ $API_RESOLV_CONF_FILE
                        #exit 0
						return
                	else
                    	#if TMP is NULL then there are no entries already present. so just continye. In the end we will append the values
                        #echo "NO.Entry is not already present. so append the new enrty."
                        COUNT=0
                        break
                	fi
            	else
                    #echo "Just continue if the count is not reached to the desired entry..."
                    continue
            	fi
            
            done
    
	    if [ $2 -gt $COUNT ]; then
                #echo "AddDNSserverIP_New - There are less DNS entries already present. so just insert your new entry here."
                if [ "$SET_IPV6_PRIORITY" == 'yes' -o "$SET_IPV6_PRIORITY" == 'y' ]; then
					TMP=`grep nameserver $API_RESOLV_CONF_FILE | grep "\."`
					if ! [ -z "$TMP" ]; then
						sed '/'"$TMP"'/ i\nameserver '$1 $API_RESOLV_CONF_FILE >> /var/tmp/resolv.conf
						mv /var/tmp/resolv.conf $API_RESOLV_CONF_FILE
					else
						echo "nameserver $1" >> $API_RESOLV_CONF_FILE
					fi
				else
	                echo "nameserver $1" >> $API_RESOLV_CONF_FILE
				fi
            fi
    fi
}
