#!/usr/bin/env python

#-----------------------------------------Import Rules Files -------------------------------------------
import PyRules
from   PyRules import PrjVars
from   PyRules import *


#--------------------------------------- Extra Pyhon modules -------------------------------------------
import os
from helper import ConvertString2Int
#-------------------------------------------------------------------------------------------------------

def Py_FileFromTemplate(indir,outfile,loopmacro,values,*extras):

	template="../spx/DATA/template/"+indir

	# Extra arguments should come in twos (macro,value)
	if len(extras) % 2  == 1 :
		print "ERROR: Invalid arguments for Py_FileFromTemplate()"
		return 1

	Py_Touch(outfile)	

	# Read head and write to output as is
	head=Py_ReadFile(template+"/head")
	Py_AppendFile(head,outfile)

	# read body and for every value modify the macro and append to outfile
	body=Py_ReadFile(template+"/body")
	for val in values:
		newbody=body
		
		if loopmacro != "":
			newbody=newbody.replace(loopmacro,str(val)) 

		# If any extra macros, replace them
		for ext in range(0,len(extras)/2):
			emacro=extras[(ext*2)]
			evalue=extras[(ext*2)+1] 
			newbody= newbody.replace(emacro,evalue)

		Py_AppendFile(newbody,outfile)
		
	# Read tail  and append to output as is
	tail=Py_ReadFile(template+"/tail");
	Py_AppendFile(tail,outfile)
	return 0
	

def Py_CreateInittab(indir,outfile):

	arch=PrjVars["ARCH"]

	template="../spx/DATA/template/"+indir

	Py_Touch(outfile)	

	# Read head and write to output as is
	head=Py_ReadFile(template+"/head")
	Py_AppendFile(head,outfile)

	# Read tail  and append to output as is
	if arch == "i686" or arch =="X86_64":
		tail=Py_ReadFile(template+"/tail.x86")   # Add ttyX
	else:
		tail=Py_ReadFile(template+"/tail")		 # Add console
#
	Py_AppendFile(tail,outfile)
	return 0
	


#-------------------------------------------------------------------------------------------------------
#			Rules for Installing Source to ImageTree
#-------------------------------------------------------------------------------------------------------
def build_install():

	ImageTree=PrjVars["IMAGE_TREE"]

	# Copy the data to Image
	retval=Py_CopyDir(".",ImageTree)
	if retval != 0:
		return retval

#	# Create Static devices - (No more static device nodes in /dev - deprecated) . Just here for backward compatability
#	if os.path.exists("../spx/DATA/device.list") == True:
#		retval=Py_CreateDevTree("../spx/DATA/device.list")
#		if retval != 0:
#			return retval
#
	# TTY defconfig
	retval=Py_WriteFile(PrjVars["CONFIG_SPX_FEATURE_GLOBAL_CONSOLE_TTY"], ImageTree+"/etc/defconfig/console_tty")
	if retval !=0:
		return retval
	retval=Py_CopyFile(ImageTree+"/etc/defconfig/console_tty", ImageTree+"/conf/console_tty")
	if retval != 0:
		return retval
	

	# SHELL defconfig 
	retval=Py_WriteFile("[defaultshell]", ImageTree+"/etc/defconfig/default_sh")
	if retval != 0:
		return retval
	retval=Py_AppendFile("default_shell=\""+PrjVars["CONFIG_SPX_FEATURE_GLOBAL_DEFAULT_SHELL"]+"\"", ImageTree+"/etc/defconfig/default_sh")
	if retval != 0:
		return retval
	retval=Py_CopyFile(ImageTree+"/etc/defconfig/default_sh", ImageTree+"/conf/default_sh");
	if retval != 0:
		return retval


	# Create core features and macros if not exist in Image
	if os.path.exists(ImageTree+"/etc/core_features") == False:
		retval=Py_CopyFile("./etc/core_features", ImageTree+"/etc/core_features");
		if retval != 0:
			return retval
	if os.path.exists(ImageTree+"/etc/core_macros") == False:
		retval=Py_CopyFile("./etc/core_macros", ImageTree+"/etc/core_macros");
		if retval != 0:
			return retval

	# Add necessary macros and features
	retval=Py_AddToMacros("CONFIG_SPX_FEATURE_GLOBAL_CONSOLE_TTY")
	if retval != 0:
		return retval
	retval=Py_AddToMacros("CONFIG_SPX_FEATURE_GLOBAL_DEFAULT_SHELL")
	if retval != 0:
		return retval
	retval=Py_AddToMacros("CONFIG_SPX_FEATURE_GLOBAL_HOST_PREFIX")
	if retval != 0:
		return retval
	retval=Py_AddToMacros("CONFIG_SPX_FEATURE_GLOBAL_DEFAULT_DOMAINNAME")
	if retval != 0:
		return retval
	retval=Py_AddToMacros("CONFIG_SPX_FEATURE_FASTER_BOOT_SUPPORT")
	if retval != 0:
		return retval
	retval=Py_AddToMacros("CONFIG_SPX_FEATURE_MMC_BOOT_PARTITION")
	if retval != 0:
		return retval
	retval=Py_AddToMacros("CONFIG_SPX_FEATURE_MMC_IMAGE_NAME")
	if retval != 0:
		return retval
	retval=Py_AddToMacros("CONFIG_SPX_FEATURE_FIRMWARE_TAR_NAME")
	if retval != 0:
		return retval

	retval=Py_AddToFeatures("CONFIG_SPX_FEATURE_ADVANCED_IP_ROUTING_SUPPORT")
	if retval != 0:
		return retval
	retval=Py_AddToFeatures("CONFIG_SPX_FEATURE_OPENSSL_1_0_1")
	if retval != 0:
		return retval
	retval=Py_AddToFeatures("CONFIG_SPX_FEATURE_OPENSSL_0_9_8")
	if retval != 0:
		return retval
	retval=Py_AddToFeatures("CONFIG_SPX_FEATURE_DISABLE_PING_SUPPORT")
	if retval != 0:
		return retval
	retval=Py_AddToFeatures("CONFIG_SPX_FEATURE_FASTER_BOOT_SUPPORT")
	if retval != 0:
		return retval
	retval=Py_AddToFeatures("CONFIG_SPX_FEATURE_OPENSSL_HW_ENC")
	if retval != 0:
		return retval
	retval=Py_AddToFeatures("CONFIG_SPX_FEATURE_MMC_BOOT")
	if retval != 0:
		return retval
	retval=Py_AddToFeatures("CONFIG_SPX_FEATURE_FIRMWARE_TAR")
	if retval != 0:
		return retval
		

	#  Get Number of Network Interfaces
	NicStr=PrjVars["CONFIG_SPX_FEATURE_GLOBAL_NIC_COUNT"]
	NicCount=ConvertString2Int(NicStr)
	if "CONFIG_SPX_FEATURE_GLOBAL_DEFAULT_NETWORK_DHCP" in PrjVars:
		NicConfig="dhcp"
	else:
		NicConfig="manual"	

	# Create interfaces file 
	retval=Py_FileFromTemplate("interfaces",ImageTree+"/etc/defconfig/interfaces","$IFNUM", range(0,NicCount),"$TYPE",NicConfig)
	if retval != 0:
		return retval
	retval=Py_CopyFile(ImageTree+"/etc/defconfig/interfaces", ImageTree+"/conf/interfaces")
	if retval != 0:
		return retval

	# Create dns.conf file
	retval=Py_FileFromTemplate("dns.conf",ImageTree+"/etc/defconfig/dns.conf","$IFNUM", range(0,NicCount))
	if retval != 0:
		return retval
	retval =Py_CopyFile(ImageTree+"/etc/defconfig/dns.conf", ImageTree+"/conf/dns.conf")
	if retval != 0:
		return retval

	#Create Inittab
	retval=Py_CreateInittab("inittab",ImageTree+"/etc/inittab");
	if retval != 0:
		return retval

	#Add SystemLogSocketName support for Kernel Version 2.6.28+New Target
	if  "CONFIG_SPX_FEATURE_GLOBAL_KERNEL_VER2_PLUS_NEW_TARGET" in PrjVars:
		retval = Py_AppendFile("$SystemLogSocketName /var/syslogsock", ImageTree+"/etc/defconfig/rsyslog.conf")
                if retval != 0:
   	  		return retval

	 #Add mDNS Support only if enabled
        if  "CONFIG_SPX_FEATURE_MDNS_SUPPORT" in PrjVars:
                retval = Py_AppendFile("avahi:x:104:111:Avahi mDNS daemon,,,:/var/run/avahi-daemon:/bin/false", ImageTree+"/etc/defconfig/passwd")
                if retval != 0:
                        return retval

	return retval

#-------------------------------------------------------------------------------------------------------
#				Rules for Debug Install
#-------------------------------------------------------------------------------------------------------
def debug_install():

	return 0



#-------------------------------------------------------------------------------------------------------
