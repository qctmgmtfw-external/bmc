#!/usr/bin/env python

#-----------------------------------------Import Rules Files -------------------------------------------
import PyRules
from   PyRules import PrjVars
from   PyRules import *
#-------------------------------------------------------------------------------------------------------

KernelVersion="3.14.17-ami"

#-------------------------------------------------------------------------------------------------------
#			Rules for Extracting Source
#-------------------------------------------------------------------------------------------------------
def extract_source():

	return 0

#-------------------------------------------------------------------------------------------------------
#			Rules for Clean Source Directory
#-------------------------------------------------------------------------------------------------------
def clean_source():


	retval = Py_Cwd(PrjVars["KERNEL_SRC"]+"/linux")
	if retval != 0:
		return retval 

	retval=Py_SetEnv("ARCH","arm")			# Change to linux specific ARCH name 
	if retval != 0:
		return retval 

	retval =Py_RunMake("clean");
	if retval != 0:
		return retval 

	retval =Py_RunMake("distclean");
	if retval != 0:
		return retval 

	retval=Py_SetEnv("ARCH","ARM")			# Restore SPX spcecific ARCH name
	if retval != 0:
		return retval 

	return 0

#-------------------------------------------------------------------------------------------------------
#			Rules for Building Source
#-------------------------------------------------------------------------------------------------------
def build_source():

	if "CONFIG_SPX_FEATURE_NCSI_MULTI_NODE_CX4" in PrjVars:
		SrcFile="%s/%s/spx/DEFCONFIG.macvlan"%(PrjVars["BUILD"],PrjVars["PACKAGE"])
	else:
		SrcFile="%s/%s/spx/DEFCONFIG"%(PrjVars["BUILD"],PrjVars["PACKAGE"])
	DestFile="%s/linux/.config"%(PrjVars["KERNEL_SRC"])
	TempDir="%s/%s"%(PrjVars["TEMPDIR"],PrjVars["PACKAGE"])

	Py_AddMacro("SPX_KERNEL_VERSION",KernelVersion)

	retval = Py_CopyFile(SrcFile,DestFile)
	if retval != 0:
		return retval 

	retval = Py_Cwd(PrjVars["KERNEL_SRC"]+"/linux")
	if retval != 0:
		return retval 

	retval=Py_SetEnv("ARCH","arm")
	if retval != 0:
		return retval 

	retval =Py_RunMake("oldconfig");
	if retval != 0:
		return retval 

	retval =Py_RunMake("uImage KALLSYMS_EXTRA_PASS=1");
	if retval != 0:
		return retval 

	retval =Py_RunMake("modules");
	if retval != 0:
		return retval 


	retval = Py_MkdirClean("%s/modules"%(TempDir))
	if retval != 0:
		return retval 

	retval =Py_RunMake("modules_install INSTALL_MOD_PATH=%s/modules"%(TempDir))
	if retval != 0:
		return retval 

	retval = Py_MkdirClean("%s/kernel-dev"%(TempDir))
	if retval != 0:
		return retval 

	retval =Py_RunMake("headers_install INSTALL_HDR_PATH=%s/kernel-dev"%(TempDir))
	if retval != 0:
		return retval 

	retval=Py_SetEnv("ARCH","ARM")
	if retval != 0:
		return retval 

	return 0


#-------------------------------------------------------------------------------------------------------
#			 Rules for Creating (Packing) Binary Packge
#-------------------------------------------------------------------------------------------------------
def build_package_Kernel_ex():
	return Py_PackSPX("./boot/kernel","%s/linux/arch/arm/boot/uImage"%(PrjVars["KERNEL_SRC"]))

def build_package_Kernel_dev_ex():
	TempDir="%s/%s"%(PrjVars["TEMPDIR"],PrjVars["PACKAGE"])
	return Py_PackSPX("./","%s/kernel-dev"%(TempDir))
	#return Py_PackSPX("./usr/include/linux","%s/linux/include"%(PrjVars["KERNEL_SRC"]))

def build_package_Kernel_modules_ex():

	TempDir="%s/%s"%(PrjVars["TEMPDIR"],PrjVars["PACKAGE"])
	KernelVer=PrjVars["SPX_KERNEL_VERSION"]

	retval = Py_Delete("%s/modules/lib/modules/%s/source"%(TempDir,KernelVer))
	if retval != 0:
		return retval 

	retval = Py_Delete("%s/modules/lib/modules/%s/build"%(TempDir,KernelVer))
	if retval != 0:
		return retval 

	return Py_PackSPX("./","%s/modules"%(TempDir))

#-------------------------------------------------------------------------------------------------------


