#!/usr/bin/env python

#-----------------------------------------Import Rules Files -------------------------------------------
import PyRules
from   PyRules import PrjVars
from   PyRules import *
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


	retval = Py_Cwd(PrjVars["BOOT_SRC"]+"/uboot")
	if retval != 0:
		return retval 

	retval=Py_SetEnv("ARCH","arm")
	if retval != 0:
		return retval 

	retval=Py_SetEnv("CROSS_COMPILE",PrjVars["TOOLDIR"]+"/arm-linux/arm-2011.09/bin/arm-none-linux-gnueabi-")
	if retval != 0:
		return retval 

	retval =Py_RunMake("clean");
	if retval != 0:
		return retval 

	retval =Py_RunMake("distclean");
	if retval != 0:
		return retval 

	retval=Py_SetEnv("ARCH","ARM")
	if retval != 0:
		return retval 

	return 0

#-------------------------------------------------------------------------------------------------------
#			Rules for Building Source
#-------------------------------------------------------------------------------------------------------
def build_source():

	
	SrcFile="%s/%s/spx/DEFCONFIG"%(PrjVars["BUILD"],PrjVars["PACKAGE"])
	DestFile="%s/uboot/include/configs/ast2500evb.h"%(PrjVars["BOOT_SRC"])

	retval = Py_CopyFile(SrcFile,DestFile)
	if retval != 0:
		return retval 

	retval = Py_Cwd(PrjVars["BOOT_SRC"]+"/uboot")
	if retval != 0:
		return retval 

	retval=Py_SetEnv("ARCH","arm")
	if retval != 0:
		return retval 

	retval =Py_RunMake("ast2500evb_config");
	if retval != 0:
		return retval 

	retval =Py_RunSilentMake("");
	if retval != 0:
		return retval 

	retval=Py_SetEnv("ARCH","ARM")
	if retval != 0:
		return retval 

	return 0



#-------------------------------------------------------------------------------------------------------
#			 Rules for Creating (Packing) Binary Packge
#-------------------------------------------------------------------------------------------------------
def build_package_Bootloader_ex():
	return Py_PackSPX("./","%s/uboot/u-boot.bin"%(PrjVars["BOOT_SRC"]))


#-------------------------------------------------------------------------------------------------------

