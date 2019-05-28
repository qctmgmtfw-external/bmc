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

	return 0

#----------------------- Function simulating sed fucntionality -------------------
def Py_sed(file,oldstr,newstr):

	try:
		fd= open(file,'r')
		contents = fd.read()
		fd.close()
	except:
		print "SED: Error read opening %s"%(file)
		return -1
	
	newcontents=contents.replace(oldstr,newstr)
	
	try:
		fd=open(file,'w+')
		contents = fd.write(newcontents)
		fd.close()
	except:
		print "SED: Error read opening %s"%(file)
		return -1

	return 0

	
#-------------------------------------------------------------------------------------------------------
#			Rules for Building Source
#-------------------------------------------------------------------------------------------------------
def build_source():
	PkgDir="%s/%s"%(PrjVars["BUILD"],PrjVars["PACKAGE"])
	MultiArch=PrjVars["MULTI_ARCH_LIB"]
	
	retval =Py_CreateSysTree(PkgDir+"/spx/RULES.D", PkgDir+"/data/tree",PkgDir+"/data/defaults")
	if retval != 0:
		return retval 
	
	#Fix MultiArch for Pam files
	retval=Py_sed(PkgDir+"/data/tree/etc/defconfig/pam_withunix","%TRIPLET%",MultiArch)
	if retval != 0:
		return retval
	
	retval=Py_sed(PkgDir+"/data/tree/etc/defconfig/pam_wounix","%TRIPLET%",MultiArch)
	if retval != 0:
		return retval
	
	retval=Py_sed(PkgDir+"/data/tree/conf/pam_withunix","%TRIPLET%",MultiArch)
	if retval != 0:
		return retval
	
	retval=Py_sed(PkgDir+"/data/tree/conf/pam_wounix","%TRIPLET%",MultiArch)
	if retval != 0:
		return retval
	
	if os.path.exists(PkgDir+"/data/tree/bkupconf/pam_withunix") == True:
		retval=Py_sed(PkgDir+"/data/tree/bkupconf/pam_withunix","%TRIPLET%",MultiArch)
		if retval != 0:
			return retval
	
	if os.path.exists(PkgDir+"/data/tree/bkupconf/pam_wounix") == True:
		retval=Py_sed(PkgDir+"/data/tree/bkupconf/pam_wounix","%TRIPLET%",MultiArch)
		if retval != 0:
			return retval

	#Add busybox.config
	retval = Py_AddBusyBoxConfig(PkgDir+"/spx/RULES.D/sys_base.busybox.config")
	if retval != 0:
		return retval

	# Create ld.so.conf.d/<MULTI_ARCH_LIB to added multiarch libraries
	LdEntry="#SPX Multiarch support\n/lib/%s\n/usr/lib/%s"%(MultiArch,MultiArch)
	retval= Py_WriteFile(LdEntry,PkgDir+"/data/tree/etc/ld.so.conf.d/"+MultiArch+".conf")
	if retval != 0:
		return retval 
	
	# Write target version
	retval = Py_WriteFile(PrjVars["TARGET_VER"],PkgDir+"/data/tree/etc/target.ver")
	return retval
	


#-------------------------------------------------------------------------------------------------------
#			 Rules for Creating (Packing) Binary Packge
#-------------------------------------------------------------------------------------------------------
def build_package_sys_base_ex():
	PkgDir="%s/%s"%(PrjVars["BUILD"],PrjVars["PACKAGE"])

	return Py_PackSPX("./", PkgDir+"/data/tree")



#-------------------------------------------------------------------------------------------------------

