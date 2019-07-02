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
	DestDir="%s/linux/initramfs"%(PrjVars["KERNEL_SRC"])

	retval = Py_MkdirClean(DestDir)
	if retval != 0:
		return retval 

	return Py_CopyDir("./",DestDir)


#-------------------------------------------------------------------------------------------------------
#				Rules for Debug Install
def debug_install():
	return 0



#-------------------------------------------------------------------------------------------------------

