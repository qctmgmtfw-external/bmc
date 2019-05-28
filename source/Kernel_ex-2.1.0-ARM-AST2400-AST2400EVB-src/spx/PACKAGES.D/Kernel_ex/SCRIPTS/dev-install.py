#!/usr/bin/env python

#-----------------------------------------Import Rules Files -------------------------------------------
import PyRules
from   PyRules import PrjVars
from   PyRules import *


#--------------------------------------- Extra Pyhon modules -------------------------------------------
#
#-------------------------------------------------------------------------------------------------------


KernelVersion="3.14.17-ami"

#-------------------------------------------------------------------------------------------------------
#		  	      Rules for Installing to ImageTree
#-------------------------------------------------------------------------------------------------------
def build_install():

	Py_AddMacro("SPX_KERNEL_VERSION",KernelVersion)

	retval=Py_SetValue("SPX_KERNEL_VERSION",KernelVersion)
	if retval != 0:
		return retval


	retval = Py_MkdirClean(PrjVars["KERNEL_BIN"])
	if retval != 0:
		return retval

	# Copy the data to Image
	retval =Py_CopyFile("./boot/kernel/uImage",PrjVars["KERNEL_BIN"])
	if retval != 0:
		return retval

	
	return Py_SoftLink("generic","%s/lib/modules/%s"
						%(PrjVars["IMAGE_TREE"],PrjVars["SPX_KERNEL_VERSION"]))



#-------------------------------------------------------------------------------------------------------
#				Rules for Debug Install
def debug_install():

	return Py_CopyFile("./boot/kernel/uImage",PrjVars["TFTPBOOT"])


#-------------------------------------------------------------------------------------------------------

