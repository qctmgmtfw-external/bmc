//;*****************************************************************;
//;*****************************************************************;
//;**                                                             **;
//;**     (C) COPYRIGHT American Megatrends Inc. 2008-2009        **;
//;**                     ALL RIGHTS RESERVED                     **;
//;**                                                             **;
//;**  This computer software, including display screens and      **;
//;**  all related materials, are confidential and the            **;
//;**  exclusive property of American Megatrends, Inc.  They      **;
//;**  are available for limited use, but only pursuant to        **;
//;**  a written license agreement distributed with this          **;
//;**  computer software.  This computer software, including      **;
//;**  display screens and all related materials, shall not be    **;
//;**  copied, reproduced, published or distributed, in whole     **;
//;**  or in part, in any medium, by any means, for any           **;
//;**  purpose without the express written consent of American    **;
//;**  Megatrends, Inc.                                           **;
//;**                                                             **;
//;**                                                             **;
//;**                American Megatrends, Inc.                    **;
//;**           5555 Oakbook Parkway, Building 200                **;
//;**     Norcross,  Georgia - 30071, USA. Phone-(770)-246-8600.  **;
//;**                                                             **;
//;*****************************************************************;
//;*****************************************************************;

//=======================================================================================================================================//
// =============================================================================
// Bookmark Jump Tag Table - Use the tags to search for bookmarks in the code
// =============================================================================
// Jump Tags  - Description
// -----------------------------------------------------------------------------
// 1151529970 -  PROC - "eGUI.addElement()"
// 1151529975 -  PROC - "eGUI.createAnchor()"
// 1152544737 -  PROC - "eGUI.addSpacer()"
// 1152551603 -  PROC - "eGUI.addImage()"
//
//	GUI Control Library Functions
//---------------------------------------------------------------------------------------------------------------------------------------//
//	This file defines generic functions for UI
//	Author: Brandon Burrell
//=======================================================================================================================================//


var eGUI = {};



eGUI.getLayoutPath=function()
{

	return(parent.UIOBJ.config.layoutpath);	// returns exact string in app_JSON

}

eGUI.getApplicationPath=function()
{
	return(parent.gAppPath);	// Returns /Applications/[appname]
}

//______________________________________________________________________________
//==============================================================================
//                              eGUI.createDOMAPIElm
//------------------------------------------------------------------------------
// Function to call DOMAPI createElm from an object argument, so that
// parameters can be compiled from eLang.getObjProps.
//
// Input: parent, x, y, w, h, bgcolor, color, type(HTML tag)
//
// Output: reference to new object
//
// [BrandonB] 10/2/2003
//______________________________________________________________________________
//==============================================================================


eGUI.createDOMAPIElm = function(arg)
{
	return(core.createElm(
						arg.parent,
						arg.x,
						arg.y,
						arg.w,
						arg.h,
						arg.bgcolor,
						arg.color,
						arg.type));
}



//______________________________________________________________________________
//==============================================================================
//                             eGUI.addElement
//------------------------------------------------------------------------------
//
//
// Input
// -----
//   arg -
//
// Output
// ------
//    -
//
//                                                                    Author:BMB
//______________________________________________________________________________
//==============================================================================
// Jump Tag 1151529970 [  PROC - "eGUI.addElement()" ]

eGUI.addElement=function(arg)
{
	if (arg.type==undefined)
		{
		arg.type='div';
		}
	var xx = document.createElement(arg.type);
	var par=(arg.parent==undefined?document.body:arg.parent);

	// Set props to null unless explicitely given, so they will be governed by
	// the style sheet
	if (arg.className==undefined)
		{
		// Process the input properties so that I do not have to assign any
		// value if they do not exist.  This is to allow the style sheet to
		// override.
		if (arg.color!=undefined){xx.style.color=arg.color;}
		if (arg.fontSize!=undefined){xx.style.fontSize=arg.fontSize;}
		if (arg.fontFamily!=undefined){xx.style.fontFamily=arg.fontFamily;}
		if (arg.fontWeight!=undefined){xx.style.fontWeight=arg.fontWeight;}
		}
	else
		{
		xx.className=arg.className;
		}
   par.appendChild(xx);
	return xx;

}

//______________________________________________________________________________
//==============================================================================
//                            eGUI.createAnchor
//------------------------------------------------------------------------------
//
//
// Input
// -----
//   aname -
//
// Output
// ------
//    -
//
//                                                                    Author:BMB
//______________________________________________________________________________
//==============================================================================
// Jump Tag 1151529975 [  PROC - "eGUI.createAnchor()" ]

eGUI.createAnchor=function(aname)
{
	var x=document.createElement('a');
	x.name=aname;
	x.id=aname;
	document.body.appendChild(x);
	return x;

}



//______________________________________________________________________________
//==============================================================================
//                              eGUI.addSpacer
//------------------------------------------------------------------------------
// Adds a blank space between top-down rendered elements.
//
// Input
// -----
//   pheight = pixel height for spacer block
//
// Output
// ------
//    -
//
//                                                                    Author:BMB
//______________________________________________________________________________
//==============================================================================
// Jump Tag 1152544737 [  PROC - "eGUI.addSpacer()" ]

eGUI.addSpacer=function(pheight)
{
	if (pheight==undefined)
		{
		pheight=20;
		}
   eGUI.addImage({ src:"/GUISE/Graphics/spacer.gif", h:pheight});

	// Added to make rendering focus go to the next line after a spacer image,
	// so multiple spacers can be used [BrandonB] 3/12/2007
	var xx = document.createElement("br");
	document.body.appendChild(xx);



}

//______________________________________________________________________________
//==============================================================================
//                              eGUI.addImage
//------------------------------------------------------------------------------
//
//
// Input
// -----
//   arg -
//
// Output
// ------
//    -
//
//                                                                    Author:BMB
//______________________________________________________________________________
//==============================================================================
// Jump Tag 1152551603 [  PROC - "eGUI.addImage()" ]

eGUI.addImage=function(arg)
{
	var xx = document.createElement("img");
	var par=(arg.parent==undefined?document.body:arg.parent);
	if (arg.w!=undefined){xx.width=arg.w;};
	if (arg.h!=undefined){xx.height=arg.h;xx.style.height=arg.h;};
	xx.src=arg.src;
	par.appendChild(xx);
	return xx;	// To assign handlers


}

eGUI.loaded = true;