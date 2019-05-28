#!/usr/bin/env python

#-----------------------------------------Import Rules Files -------------------------------------------
import PyRules
from   PyRules import PrjVars
from   PyRules import *
#-------------------------------------------------------------------------------------------------------

#--------------------------------------- Extra Pyhon modules -------------------------------------------
from MergeConfig import MergeConfig
#-------------------------------------------------------------------------------------------------------


#-------------------------------------------------------------------------------------------------------
#			Rules for Extracting Source
#-------------------------------------------------------------------------------------------------------
def extract_source():

	return 0

#-------------------------------------------------------------------------------------------------------
#			Rules for Clean Source Directory
#-------------------------------------------------------------------------------------------------------
def clean_source():

	BusyboxSrc=PrjVars["BUSYBOX_SRC"]

	retval = Py_Cwd(BusyboxSrc+"/busybox")
	if retval != 0:
		return retval 

	retval =Py_RunMake("clean");
	if retval != 0:
		return retval 

	retval =Py_RunMake("distclean");
	if retval != 0:
		return retval 

	return 0

#-------------------------------------------------------------------------------------------------------
#			Rules for Building Source
#-------------------------------------------------------------------------------------------------------
def build_source():

	BusyboxSrc=PrjVars["BUSYBOX_SRC"]
	BusyboxCfg=PrjVars["BUSYBOX_CFG"]

	retval = MergeConfig(BusyboxCfg, BusyboxSrc+"/busybox/.config")
	if retval != 0:
		return retval 

	retval = Py_Cwd(BusyboxSrc+"/busybox")
	if retval != 0:
		return retval 

	retval =Py_RunMake("oldconfig");
	if retval != 0:
		return retval 

	retval =Py_RunMake("all");
	if retval != 0:
		return retval 

	return 0



#-------------------------------------------------------------------------------------------------------
#			 Rules for Creating (Packing) Binary Packge
#-------------------------------------------------------------------------------------------------------
def build_package_Busybox_ex():

	BusyboxSrc=PrjVars["BUSYBOX_SRC"]
	
	retval = Py_Cwd(BusyboxSrc+"/busybox")
	if retval != 0:
		return retval 

	retval = Py_MkdirClean(BusyboxSrc+"/busybox/OUTPUT")
	if retval != 0:
		return retval 

	retval =Py_RunMake("install CONFIG_PREFIX=%s/busybox/OUTPUT"%(BusyboxSrc))
	if retval != 0:
		return retval 
	

	return Py_PackSPX(".", BusyboxSrc+"/busybox/OUTPUT")


#-------------------------------------------------------------------------------------------------------


