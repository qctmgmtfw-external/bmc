#!/usr/bin/env python

#-----------------------------------------Import Rules Files -------------------------------------------
import PyRules
from   PyRules import *
#-------------------------------------------------------------------------------------------------------

#--------------------------------------- Extra Pyhon modules -------------------------------------------
import glob
#-------------------------------------------------------------------------------------------------------


#-------------------------------------------------------------------------------------------------------
#	Parse options file (kernel/busybox config file format) into dictionay
#-------------------------------------------------------------------------------------------------------
def ParseOptions(filename,options):

	# Open the file in read text mode
	try:
		fd=open(filename,"rt")
	except (IOError):
		print "ERROR: Unable to open ",filename
		return 1

	while True :

		# Read line by line
		try:
			line = fd.readline()
		except (IOError):
			print "ERROR: Unable to read" ,filename
			fd.close()
			return 1

		# Check if end of file is reached
		if line == '':
			break
	
		# Skip Empty of comment line 	
		if CheckBlankLine(line) == True :
			continue

		# Check option of format "# CONFIG_XYZ is not set"	
		if line[0] == '#':
			opt=line.split()
			if len(opt) < 2:
				continue
			opt= opt[1].strip()
			if opt[0:6] == "CONFIG":
				options[opt]=(False,"")
			continue
		
		# Clean the line of any comments 
		CleanLine=line.split("#")[0]
		CleanLine=CleanLine.strip()
		if CleanLine == "":
			continue
		line=CleanLine

		
		# Check option of format "CONFIG_XYZ=Values"
		opt=line.split("=",1)
		if len(opt) < 2:
			continue
		value=opt[1].strip()
		opt=opt[0].strip()
		if opt[0:6] == "CONFIG":
			options[opt]=(True,value)
		

	fd.close()
	return 0

#-------------------------------------------------------------------------------------------------------
#	 		Check for Blank Empty lines
#------------------------------------------------------------------------------------------------------
def CheckBlankLine(line=""):
	
	# Strip trailing \n
	line=line.rstrip("\n");

	# Remove white spaces around
	line=line.strip("\t ")

	if line == "" :
		return True
	
	return False


#-------------------------------------------------------------------------------------------------------
#	Merge new options to global config file and write back new config file
#-------------------------------------------------------------------------------------------------------

def MergeConfig(SrcDir,DestFile):

	GlobalOpts={}
	NewOpts={}

	# Delete existing file	
	Py_Delete(DestFile)

	# Parse global config file. Only one should be present
	gfiles=glob.glob(SrcDir+"/*.busybox.config")
	if len(gfiles) != 1:				
		return 1
	if ParseOptions(gfiles[0],GlobalOpts) == 1:
		return 1

	# Get New config files
	files=glob.glob(SrcDir+"/*.busybox.options")
	for file in files:
		if ParseOptions(file,NewOpts) == 1:
			return 1
		
	# Merge the dictionay
	for opt in NewOpts:
		if opt not in  GlobalOpts:
			print "ERROR: %s does not have option %s"%(gfiles[0],opt)
			return 1
		GlobalOpts[opt]=NewOpts[opt]

	#Write the new config file
	try:
		fd=open(DestFile,"wt")
	except (IOError):
		print "ERROR: Unable to open ",DestFile
		return 1

	# Write the dictionory to file in the required format
	for opt in GlobalOpts:
		Value=GlobalOpts[opt]
		try:
			if Value[0] == True:
				fd.write("%s=%s\n"%(opt,Value[1]))
			else:		
				fd.write("# %s is not set\n"%(opt))

		except (IOError):
			print "ERROR: Unable to write ",DestFile
			fd.close()
			return 1

	fd.close()
	return 0

#-------------------------------------------------------------------------------------------------------
