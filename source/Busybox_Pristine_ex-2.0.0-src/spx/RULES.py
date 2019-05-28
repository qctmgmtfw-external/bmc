#!/usr/bin/env python

#-----------------------------------------Import Rules Files -------------------------------------------
import PyRules
from   PyRules import PrjVars
from   PyRules import *
#-------------------------------------------------------------------------------------------------------

#--------------------------------------- Extra Pyhon modules -------------------------------------------
import os

#-------------------------------------------------------------------------------------------------------


Busybox_Version="1.21.1"		# Change when migrating to other versions

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
	return  Py_DeleteAll(BinDir,"Busybox_Pristine_ex-*-*.spx")


#-------------------------------------------------------------------------------------------------------
#			Rules for Building Source
#-------------------------------------------------------------------------------------------------------
def build_source():

	PkgDir="%s/%s"%(PrjVars["BUILD"],PrjVars["PACKAGE"])

	Busybox_Src=PrjVars["BUSYBOX_SRC"]

	retval = Py_MkdirClean(Busybox_Src)
	if retval != 0:
		return retval 
	
	retval = Py_UntarBz2(PkgDir+"/data/busybox-"+Busybox_Version+".tar.bz2",Busybox_Src)
	if retval != 0:
		return retval 

	retval= Py_Rename(Busybox_Src+"/busybox-"+Busybox_Version,Busybox_Src+"/busybox")
	if retval != 0:
		return retval 

	return 0


#-------------------------------------------------------------------------------------------------------
#			 Rules for Creating (Packing) Binary Packge
#-------------------------------------------------------------------------------------------------------
def build_package_Busybox_Pristine_ex():
	return Py_PackSPX()



#-------------------------------------------------------------------------------------------------------

		
