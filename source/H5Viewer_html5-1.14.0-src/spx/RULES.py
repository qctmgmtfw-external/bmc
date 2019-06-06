#!/usr/bin/env python

import PyRules
from PyRules import PrjVars
from PyRules import *

import os

def extract_source():
	return 0


def build_source():
	build_dir = PrjVars['BUILD']+"/"+PrjVars['PACKAGE']+"/data/"
	tmp_dir = PrjVars['TEMPDIR']
	Py_Cwd(build_dir)

	if "CONFIG_SPX_FEATURE_HTML5_KVM" in PrjVars.keys():	
		if PrjVars['BASESOC'] == "AST":
			Py_CopyFile(build_dir + "app/libs/videoplayback_ast.js", build_dir + "app/libs/videoplayback.js")
		elif PrjVars['BASESOC'] == "PILOT":
			Py_CopyFile(build_dir + "app/libs/videoplayback_pilot.js", build_dir + "app/libs/videoplayback.js")
	else:
		Py_CopyFile(build_dir + "app/libs/videoplayback_none.js", build_dir + "app/libs/videoplayback.js")
	
	Py_MkdirClean(build_dir+"vendor")
	Py_MkdirClean(build_dir+"node_modules")
	
	Py_CopyDir(tmp_dir+"/webui_html5_libs/data/vendor", build_dir+"vendor")
	Py_CopyDir(tmp_dir+"/webui_html5_libs/data/node_modules", build_dir+"node_modules")
	Py_CopyFile(build_dir+"app/config.main.js", build_dir+"app/config.js")
	
	#Py_CopyDir(tmp_dir+"/webui_scaffold/data/output/collection", build_dir+"app/collection")
	#Py_CopyDir(tmp_dir+"/webui_scaffold/data/output/models", build_dir+"app/models")
	#Py_CopyDir(tmp_dir+"/webui_scaffold/data/output/strings", build_dir+"app/strings")
	#Py_CopyDir(tmp_dir+"/webui_scaffold/data/output/templates", build_dir+"app/templates")
	#Py_CopyDir(tmp_dir+"/webui_scaffold/data/output/views", build_dir+"app/views")
	#Py_CopyFile(tmp_dir+"/webui_scaffold/data/output/auto_router.js", build_dir+"app/auto_router.js")
	
	#TODO: Make this conditional on H5Viewer package selection
	if 'CONFIG_SPX_FEATURE_HTML5_KVM' in PrjVars.keys():
		if not os.path.exists(build_dir+"app/libs"): #Quanta++ for svn control
			Py_CopyDir(tmp_dir+"/H5ViewerUIExt/libs", build_dir+"app/libs")
		if not os.path.exists(build_dir+"app/views"): #Quanta++ for svn control
			Py_CopyDir(tmp_dir+"/H5ViewerUIExt/views", build_dir+"app/views")
		Py_CopyFile(tmp_dir+"/H5ViewerUIExt/kvm.tag", build_dir+"kvm.tag")
		Py_CopyFile(tmp_dir+"/H5ViewerUIExt/kvm.shim", build_dir+"kvm.shim")
		Py_CopyFile(tmp_dir+"/H5ViewerUIExt/media.tag", build_dir+"media.tag")
		Py_CopyFile(tmp_dir+"/H5ViewerUIExt/media.shim", build_dir+"media.shim")
		Py_CopyFile(tmp_dir+"/H5ViewerUIExt/oem.tag", build_dir+"oem.tag")
		Py_CopyFile(tmp_dir+"/H5ViewerUIExt/oem.shim", build_dir+"oem.shim")
		Py_RunShell("add_kvm.sh")
		Py_RunShell("add_media.sh")
		Py_RunShell("add_oem.sh")

	if 'CONFIG_SPX_FEATURE_HTML5_WEB_UI_ZH_TW_SUPPORT' in PrjVars.keys():
		Py_RunShell("add_lang_zh_tw.sh")
		Py_CopyDir(build_dir+"app/lang_support/zh-tw/", build_dir+"app/strings/nls/zh-tw/")	#Copy string file for 'zh-tw' support
	else:
		Py_DeleteAll(build_dir+"app/strings/nls/zh-tw","*")	#Remove unnecessary string file

	return Py_Execute("grunt default") #Use debug for generating source map file


def clean_source():
	BinDir = PrjVars["BINARY"]
	return Py_DeleteAll(BinDir, "H5Viewer_html5-*-*.spx")

#--------------------- Mandatory Rules for every binary package -------------------------


def build_package_H5Viewer_html5():
	build_dir = PrjVars['BUILD']+'/'+PrjVars['PACKAGE']+'/data'
	tmp_dir = PrjVars['TEMPDIR']+'/'+PrjVars['PACKAGE']+'/tmp/www'
	Py_MkdirClean(tmp_dir)
	Py_MkdirClean(tmp_dir+"/fonts")
	Py_MkdirClean(tmp_dir+"/images")
	#Py_MkdirClean(tmp_dir+"/viewer")
	#Py_CopyDir(build_dir+'/certs', tmp_dir+'/certs')
	Py_CopyDir(build_dir+'/dist/fonts', tmp_dir+'/fonts')
	Py_CopyDir(build_dir+'/dist/libs', tmp_dir+'/libs')
	#Py_CopyDir(build_dir+'/dist/libs', tmp_dir+'/viewer/libs')
	Py_CopyDir(build_dir+'/dist/images', tmp_dir+'/images')
	Py_CopyFile(build_dir+'/dist/blue.png', tmp_dir)
	Py_CopyFile(build_dir+'/dist/blue@2x.png', tmp_dir)
	Py_CopyFile(build_dir+'/dist/viewer.min.js', tmp_dir)
	#TODO: set a debug flag and enable this automatically based on debug mode. Should be used only in NFS
	#Py_CopyFile(build_dir+'/dist/viewer.min.js.map', tmp_dir)
	Py_CopyFile(build_dir+'/dist/viewer.min.css', tmp_dir)
	Py_CopyFile(build_dir+'/dist/viewer.html', tmp_dir)

	extensions = ['.js', '.html', '.css', '.png', '.woff', '.jpg']

	for ext in extensions:
		Py_Gzip_Web("*" + ext, tmp_dir)

	for dir,subdir,files in os.walk(tmp_dir):
		for file in files:
			fullname=dir+"/"+file
			fullname_no_gz, fullname_ext = os.path.splitext(fullname)
			if fullname_ext == ".gz" and os.path.splitext(fullname_no_gz)[1] in extensions:
				os.rename(fullname, fullname_no_gz)	

	return Py_PackSPX("./",PrjVars['TEMPDIR']+'/'+PrjVars['PACKAGE']+'/tmp')

