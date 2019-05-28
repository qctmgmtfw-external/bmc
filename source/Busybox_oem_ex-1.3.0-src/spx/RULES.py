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
	BinDir = PrjVars["BINARY"]
	return  Py_DeleteAll(BinDir,"Busybox_oem_ex-*-*.spx")


#-------------------------------------------------------------------------------------------------------
#			Rules for Building Source
#-------------------------------------------------------------------------------------------------------
def build_source():

	PatchDir="%s/%s/data"%(PrjVars["SOURCE"],PrjVars["PACKAGE"])

	PatchFiles=sorted(glob.glob(PatchDir+"/*"))

	for PatchFile in PatchFiles:
		if os.path.isfile(PatchFile) == False:
				continue
		if Py_ApplyPatch("1",PatchFile,PrjVars["BUSYBOX_SRC"]+"/busybox") == 1:
			printf("[ERROR]: Failed to patch file %s\n",PatchFile)
			return -1
	
	PkgDir="%s/%s"%(PrjVars["BUILD"],PrjVars["PACKAGE"])

	retval =Py_CopyFile(PkgDir+"/spx/RULES.D/Busybox_oem_ex.busybox.options", PrjVars["BUSYBOX_SRC"]+".cfg")
	if retval != 0:
		return retval
	return 0

#-------------------------------------------------------------------------------------------------------
#			 Rules for Creating (Packing) Binary Package
#-------------------------------------------------------------------------------------------------------


# Rules to create Busybox_oem_ex package
def build_package_Busybox_oem_ex():
	return Py_PackSPX()
