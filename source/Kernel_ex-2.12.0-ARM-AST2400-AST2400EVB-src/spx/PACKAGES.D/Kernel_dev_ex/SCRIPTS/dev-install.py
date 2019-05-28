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

	retval = Py_MkdirClean("%s/linux"%(PrjVars["SPXINC"]))
	if retval != 0:
		return retval

	return Py_CopyDir("./include","%s/linux"%(PrjVars["SPXINC"]))



#-------------------------------------------------------------------------------------------------------
#				Rules for Debug Install
def debug_install():
	return 0



#-------------------------------------------------------------------------------------------------------
