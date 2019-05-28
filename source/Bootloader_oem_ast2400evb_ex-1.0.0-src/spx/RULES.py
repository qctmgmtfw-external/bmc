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
	return  Py_DeleteAll(BinDir,"Bootloader_oem_ast2400evb_ex-*-*.spx")


#-------------------------------------------------------------------------------------------------------
#			Rules for Building Source
#-------------------------------------------------------------------------------------------------------
def build_source():

	PatchDir="%s/%s/data"%(PrjVars["SOURCE"],PrjVars["PACKAGE"])

	PatchFiles=sorted(glob.glob(PatchDir+"/*"))

	for PatchFile in PatchFiles:
		if os.path.isfile(PatchFile) == False:
				continue
		if Py_ApplyPatch("1",PatchFile,PrjVars["BOOT_SRC"]+"/uboot") == 1:
			printf("[ERROR]: Failed to patch file %s\n",PatchFile)
			return -1

	return 0

#-------------------------------------------------------------------------------------------------------
#			 Rules for Creating (Packing) Binary Package
#-------------------------------------------------------------------------------------------------------


# Rules to create Bootloader_oem_ast2400evb_ex package
def build_package_Bootloader_oem_ast2400evb_ex():
	return Py_PackSPX()
