#!/usr/bin/env python

#-----------------------------------------Import Rules Files -------------------------------------------
import PyRules
from   PyRules import PrjVars
from   PyRules import *
#-------------------------------------------------------------------------------------------------------

#--------------------------------------- Extra Pyhon modules -------------------------------------------
import os

#-------------------------------------------------------------------------------------------------------


Bootloader_Version="2013.07"		# Change when migrating to other versions


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
	return  Py_DeleteAll(BinDir,"Bootloader_Pristine_ex-*-*.spx")


#-------------------------------------------------------------------------------------------------------
#			Rules for Building Source
#-------------------------------------------------------------------------------------------------------
def build_source():

	PkgDir="%s/%s"%(PrjVars["BUILD"],PrjVars["PACKAGE"])

	Boot_Src=PrjVars["BOOT_SRC"]

	retval = Py_MkdirClean(Boot_Src)
	if retval != 0:
		return retval 
	
	retval = Py_UntarBz2(PkgDir+"/data/u-boot-"+Bootloader_Version+".tar.bz2",Boot_Src)
	if retval != 0:
		return retval 

	retval= Py_Rename(Boot_Src+"/u-boot-"+Bootloader_Version,Boot_Src+"/uboot")
	if retval != 0:
		return retval 

	return 0


#-------------------------------------------------------------------------------------------------------
#			 Rules for Creating (Packing) Binary Packge
#-------------------------------------------------------------------------------------------------------
def build_package_Bootloader_Pristine_ex():
	return Py_PackSPX()



#-------------------------------------------------------------------------------------------------------


