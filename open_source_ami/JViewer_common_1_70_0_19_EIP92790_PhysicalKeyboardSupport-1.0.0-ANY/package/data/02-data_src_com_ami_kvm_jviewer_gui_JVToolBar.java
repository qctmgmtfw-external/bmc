--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JVToolBar.java	Tue Jul 10 13:50:39 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JVToolBar.java	Wed Jul 11 11:34:16 2012
@@ -1,3 +1,14 @@
+/****************************************************************
+ **                                                            **
+ **    (C) Copyright 2006-2009, American Megatrends Inc.       **
+ **                                                            **
+ **            All Rights Reserved.                            **
+ **                                                            **
+ **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
+ **                                                            **
+ **        Georgia - 30093, USA. Phone-(770)-246-8600          **
+ **                                                            **
+ ****************************************************************/
 package com.ami.kvm.jviewer.gui;
 
 import java.awt.Dimension;
@@ -238,11 +249,11 @@
 			
 			   JVMenu menu = JViewerApp.getInstance().getJVMenu();
 			   AutoKeyboardLayout autokeylayout;
-			   if(menu.getAutokeylayout() != null )
-				   autokeylayout = menu.getAutokeylayout();
+			   if(JViewerApp.getInstance().getAutokeylayout() != null )
+				   autokeylayout = JViewerApp.getInstance().getAutokeylayout();
 			   else{
 				   autokeylayout = new AutoKeyboardLayout();
-				   menu.setAutokeylayout(autokeylayout);
+				   JViewerApp.getInstance().setAutokeylayout(autokeylayout);
 			   }
 				   
 			if(menu.getMenuSelected(menu.AUTOMAIC_LANGUAGE) == false && menu.keyBoardLayout >= menu.LANGUAGE_ENGLISH_US){
@@ -254,19 +265,19 @@
 			   menu.SetMenuEnable(JVMenu.SOFTKEYBOARD, false);
 			   menu.getMenu(JVMenu.SOFTKEYBOARD).setEnabled(false);
 			   JVMenu.m_menuItems_setenabled.put(JVMenu.SOFTKEYBOARD, false);
-			   if(autokeylayout.getKeyboardlayout() == autokeylayout.KBD_TYPE_FRENCH){
+			   if(autokeylayout.getKeyboardType() == autokeylayout.KBD_TYPE_FRENCH){
 				   JViewerApp.getInstance().OnSkbrdDisplay(menu.LANGUAGE_FRENCH);
 			   }
-			   else if(autokeylayout.getKeyboardlayout() == autokeylayout.KBD_TYPE_GERMAN){
+			   else if(autokeylayout.getKeyboardType() == autokeylayout.KBD_TYPE_GERMAN){
 				   JViewerApp.getInstance().OnSkbrdDisplay(menu.LANGUAGE_GERMAN_GER);
 			   }
-			   else if(autokeylayout.getKeyboardlayout() == 1033){
+			   else if(autokeylayout.getKeyboardType() == 1033){
 				   JViewerApp.getInstance().OnSkbrdDisplay(menu.LANGUAGE_ENGLISH_US);
 			   }
-			   else if(autokeylayout.getKeyboardlayout() == autokeylayout.KBD_TYPE_JAPANESE){
+			   else if(autokeylayout.getKeyboardType() == autokeylayout.KBD_TYPE_JAPANESE){
 				   JViewerApp.getInstance().OnSkbrdDisplay(menu.LANGUAGE_JAPANESE);
 			   }
-			   else if(autokeylayout.getKeyboardlayout() == autokeylayout.KBD_TYPE_SPANISH){
+			   else if(autokeylayout.getKeyboardType() == autokeylayout.KBD_TYPE_SPANISH){
 				   JViewerApp.getInstance().OnSkbrdDisplay(menu.LANGUAGE_SPANISH);
 			   }
 			   else{
