#!/usr/bin/env python

#-----------------------------------------Import Rules Files -------------------------------------------
import PyRules
from   PyRules import PrjVars
from   PyRules import *
#-------------------------------------------------------------------------------------------------------

#--------------------------------------- Extra Pyhon modules -------------------------------------------
import os

#-------------------------------------------------------------------------------------------------------


Kernel_Version="3.14.17"		# Change when migrating to other versions


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
	return  Py_DeleteAll(BinDir,"Kernel_Pristine_ex-*-*.spx")


#-------------------------------------------------------------------------------------------------------
#			Rules for Building Source
#-------------------------------------------------------------------------------------------------------
def build_source():

	PkgDir="%s/%s"%(PrjVars["BUILD"],PrjVars["PACKAGE"])

	Kernel_Src=PrjVars["KERNEL_SRC"]

	retval = Py_MkdirClean(Kernel_Src)
	if retval != 0:
		return retval 
	

	retval = Py_UntarGz(PkgDir+"/data/linux-"+Kernel_Version+".tar.gz",Kernel_Src)
	if retval != 0:
		return retval 

	retval= Py_Rename(Kernel_Src+"/linux-"+Kernel_Version,Kernel_Src+"/linux")
	if retval != 0:
		return retval 

	return 0


#-------------------------------------------------------------------------------------------------------
#			 Rules for Creating (Packing) Binary Packge
#-------------------------------------------------------------------------------------------------------
def build_package_Kernel_Pristine_ex():
	return Py_PackSPX()



#-------------------------------------------------------------------------------------------------------



