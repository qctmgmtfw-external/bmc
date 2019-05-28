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
	IMAGETREE=PrjVars["IMAGE_TREE"]
	retval = Py_CopyDir("./",IMAGETREE)
	if retval != 0:
		return retval
	retval = Py_Delete(IMAGETREE+"/lib/modules/generic/modules.dep")
	if retval != 0:
		return retval
	retval = Py_Delete(IMAGETREE+"/lib/modules/generic/modules.alias")
	if retval != 0:
		return retval
	retval= Py_Delete(IMAGETREE+"/lib/modules/generic/modules.symbols")
	if retval != 0:
		return retval
	retval = Py_Mkdir(IMAGETREE+"/var/run")
	if retval != 0:
		return retval
	retval = Py_Touch(IMAGETREE+"/var/run/modules.dep")
	if retval != 0:
		return retval
	retval = Py_Touch(IMAGETREE+"/var/run/modules.alias")
	if retval != 0:
		return retval
	retval = Py_Touch(IMAGETREE+"/var/run/modules.symbols")
	if retval != 0:
		return retval
	retval = Py_Cwd(IMAGETREE+"/lib/modules/generic")
	if retval != 0:
		return retval
	retval = Py_SoftLink("../../../var/run/modules.dep","modules.dep")
	if retval != 0:
		return retval
	retval = Py_SoftLink("../../../var/run/modules.alias","modules.alias")
	if retval != 0:
		return retval
	return Py_SoftLink("../../../var/run/modules.symbols","modules.symbols")

#-------------------------------------------------------------------------------------------------------
#				Rules for Debug Install
def debug_install():
	return 0



#-------------------------------------------------------------------------------------------------------


