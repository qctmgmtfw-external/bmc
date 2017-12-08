#Running at RCS.d
#Runlevel : S = S37

# Set the Hostname 
echo -n "Hostname: "

# We dont want all machines to be named localhost. 
# if the hostname is set to localhost, create a 
# unique hostname using the product and MAC address 
if [ -x /usr/local/bin/defaulthost ]
then
	/usr/local/bin/defaulthost
else
	echo "ERROR: Executable defaulthost is missing. Cannot set default hostname"
fi

# Display the hostname
echo $(hostname)"."
