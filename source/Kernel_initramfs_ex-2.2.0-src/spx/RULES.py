#!/usr/bin/env python

#-----------------------------------------Import Rules Files -------------------------------------------
import PyRules
from   PyRules import PrjVars
from   PyRules import *
#-------------------------------------------------------------------------------------------------------

BBVer="1.22.1"


#-------------------------------------------------------------------------------------------------------
#			Rules for Extracting Source
#-------------------------------------------------------------------------------------------------------
def extract_source():

	return 0


#-------------------------------------------------------------------------------------------------------
#			Rules for Clean Source Directory
#-------------------------------------------------------------------------------------------------------
def clean_source():

	PkgDir="%s/%s"%(PrjVars["BUILD"],PrjVars["PACKAGE"])
	DataDir="%s/data"%(PkgDir)

	retval = Py_Cwd(DataDir+"/busybox")
	if retval != 0:
		return retval 

	retval =Py_RunMake("clean",verbose=False)
	if retval != 0:
		return retval 

	retval =Py_RunMake("distclean",verbose=False)
	if retval != 0:
		return retval 

	return 0

#-------------------------------------------------------------------------------------------------------
#			Rules for Building Source
#-------------------------------------------------------------------------------------------------------
def build_source():

	PkgDir="%s/%s"%(PrjVars["BUILD"],PrjVars["PACKAGE"])
	DataDir="%s/data"%(PkgDir)
	TempDir="%s/%s/temp"%(PrjVars["TEMPDIR"],PrjVars["PACKAGE"])


	# Unpack busybox
	retval=Py_UntarBz2(DataDir+"/busybox-%s.tar.bz2"%(BBVer),DataDir)
        if retval != 0:
                return retval

        retval=Py_Rename(DataDir+"/busybox-%s"%(BBVer),DataDir+"/busybox")
        if retval != 0:
                return retval



	# Patch the necessary files
	PatchDir="%s/%s/data/PATCHES"%(PrjVars["SOURCE"],PrjVars["PACKAGE"])

	PatchFiles=sorted(glob.glob(PatchDir+"/*"))

	for PatchFile in PatchFiles:
		if os.path.isfile(PatchFile) == False:
				continue
		if Py_ApplyPatch("1",PatchFile,DataDir+"/busybox") == 1:
			printf("[ERROR]: Failed to patch file %s\n",PatchFile)
			return -1


	# Copy Busybox configuration and build
	retval = Py_CopyFile("%s/spx/DEFCONFIG"%(PkgDir),"%s/busybox/.config"%(DataDir))
	if retval != 0:
		return retval 

	retval = Py_Cwd(DataDir+"/busybox")
	if retval != 0:
		return retval 

	retval =Py_RunMake("silentoldconfig",verbose=False)
	if retval != 0:
		return retval 

	retval =Py_RunMake("all",verbose=False)
	if retval != 0:
		return retval 


	# Copy busybox binary, initramfs list file and init script 	
	retval = Py_MkdirClean(TempDir)
	if retval != 0:
		return retval 

	retval = Py_CopyFile(DataDir+"/init.sh",TempDir+"/init.sh")
	if retval != 0:
		return retval 

	retval = Py_CopyFile(DataDir+"/initramfs.lst",TempDir+"/initramfs.lst")
	if retval != 0:
		return retval 

	retval = Py_CopyFile(DataDir+"/busybox/busybox",TempDir+"/busybox")
	if retval != 0:
		return retval

	return 0


#-------------------------------------------------------------------------------------------------------
#			 Rules for Creating (Packing) Binary Packge
#-------------------------------------------------------------------------------------------------------
def build_package_Kernel_initramfs_ex():

	TempDir="%s/%s/temp"%(PrjVars["TEMPDIR"],PrjVars["PACKAGE"])
	return Py_PackSPX("./",TempDir)

#-------------------------------------------------------------------------------------------------------


