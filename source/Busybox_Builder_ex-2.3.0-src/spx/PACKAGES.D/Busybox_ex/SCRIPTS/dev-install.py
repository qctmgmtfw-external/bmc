#!/usr/bin/env python

#-----------------------------------------Import Rules Files -------------------------------------------
import PyRules
from   PyRules import PrjVars
from   PyRules import *


#--------------------------------------- Extra Pyhon modules -------------------------------------------
#
#-------------------------------------------------------------------------------------------------------


#-------------------------------------------------------------------------------------------------------
#		  	      Rules for Installing to ImageTree
#-------------------------------------------------------------------------------------------------------
def build_install():

	ImageTree=PrjVars["IMAGE_TREE"]
	
	retval = Py_CopyFile("../spx/DATA/dhcp6.script", ImageTree+"/usr/sbin/dhcp6.script")
	if retval != 0:
		return retval

	retval = Py_CopyFile("../spx/DATA/dhcp4.script", ImageTree+"/usr/sbin/dhcp4.script")
	if retval != 0:
		return retval

	# Copy the data to Image
	return Py_CopyDir(".",ImageTree)

#-------------------------------------------------------------------------------------------------------
#				Rules for Debug Install
#-------------------------------------------------------------------------------------------------------
def debug_install():

	return 0



#-------------------------------------------------------------------------------------------------------
