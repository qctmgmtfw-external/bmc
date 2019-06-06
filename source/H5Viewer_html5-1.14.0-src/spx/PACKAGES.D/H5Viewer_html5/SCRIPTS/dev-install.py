#!/usr/bin/env python

# Install Script
import PyRules

def build_install():
	return Py_CopyDir("./www", PrjVars['IMAGE_TREE']+"/usr/local/www")

def debug_install():
	return 0
