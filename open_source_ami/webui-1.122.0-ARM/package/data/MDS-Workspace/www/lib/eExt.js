//;*****************************************************************;
// =============================================================================
// Bookmark Jump Tag Table - Use the tags to search for bookmarks in the code
// =============================================================================
// Jump Tags  - Description
// -----------------------------------------------------------------------------
// 1158783913 -  PROC - "eExt.getHTTPHeaderProps()"
//
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
//----------------------------------------------------------------------------//
// Extension Module
// (c) American Megatrends Inc. 2003
//
// Depends on:  Nothing
//
// Author: Brandon Burrell
//----------------------------------------------------------------------------//
var eExt	= {};		// Create object


//______________________________________________________________________________
//==============================================================================
//                            eExt.parseURLVars
//------------------------------------------------------------------------------
// Searches the URL for a given token variable and returns its value (if
// present).
//
// Input:  searchtoken = token name to search URL for
//
// Output: token's value, or null (constant, not string) if not found
//______________________________________________________________________________
//==============================================================================

eExt.parseURLvars=function(searchtoken)
{

// NOTE: Changed [BrandonB] 12/18/2006 to decodeURIComponent
// so that UTF8 data passed in the URL as tokens will properly be
// decoded back to UTF8.  Firefox was converting the hex bytes to the
// default code page when using unescape.
// 	var URLVariables=unescape(location.search);     // Convert esc code back to &

	var URLVariables=decodeURIComponent(location.search);	// Convert esc code back to &
	URLVariables=URLVariables.substring(1);         // Clear off ? from query
	return (parsetokens(URLVariables,searchtoken,/\s*&\s*/));
}


//______________________________________________________________________________
//==============================================================================
//                            eExt.insertURLvar
//------------------------------------------------------------------------------
// Searches the URL for a given token variable and returns its value (if
// present).
//
// Input:  searchtoken = token name to search URL for
//
// Output: token's value, or null (constant, not string) if not found
//______________________________________________________________________________
//==============================================================================

eExt.insertURLvar=function(link, tokentoadd, tokenvalue)
{
	var curlink=new String(link);
	var newlink=new String();

// These functions are deprecated, use encodeURIComponent instead
// [BrandonB] 12/18/2006
// 	tokentoadd=escape(tokentoadd);
// 	tokenvalue=escape(tokenvalue);
	tokentoadd=encodeURIComponent(tokentoadd);
	tokenvalue=encodeURIComponent(tokenvalue);
	// Does current link already have some search tokens?
	if (curlink.indexOf('?')==-1)
		{
		newlink=curlink+'?'+tokentoadd+'='+tokenvalue;
		}
	else
		{
		newlink=curlink+'&'+tokentoadd+'='+tokenvalue;
		}
	return (newlink);
}


//______________________________________________________________________________
//==============================================================================
//                 				eExt.parseCookies
//------------------------------------------------------------------------------
// Parses the list of cookies for the document.
//
// Input: searchtoken - the cookie you are looking for
//
// Output: string - the value of the cookie, or null if not found
//
// [BrandonB] 7/3/2003
//______________________________________________________________________________
//==============================================================================

eExt.parseCookies=function(searchtoken, boolDebug)
{
	return(parsetokens(document.cookie,searchtoken,/\s*;\s*/, boolDebug));
}

//______________________________________________________________________________
//==============================================================================
//                               parsetokens
//------------------------------------------------------------------------------
//	Generic function for parsing strings of the form aaa=bbbb?ccc=ddd where ? is
// some delimiter character
//
// Input
// -----
//   tokenlist - the string containing the token pair list
//
//   searchtoken - the token you are looking for
//
//   delimiter - the character used to separate token pairs
//
// Output
// ------
//    string - the value of the token, or null if not present
//
//______________________________________________________________________________
//==============================================================================

function parsetokens(tokenlist,searchtoken, delimiter, boolDebug)
{
	// Take apart each 'token=value' and put them into an array
	var tokenpairs=tokenlist.split(delimiter);         // Now we have an array of
																	//  token pairs
	var tokenvals= new Array();                     // Create array object for
	var tokenpieces= new Array();                     // Create array object for
																	//  further splitting
	// This loop will split each token set (token=value) into
	// token[x*2] and value[x*2+1]

	for (var i=0;i<tokenpairs.length;i++)
		 {
		 tokenpieces=tokenpairs[i].split("=");       // split each set at '='
		 tokenvals[i*2]=tokenpieces[0];              // Assign token to new array
		 tokenvals[i*2+1]=tokenpieces[1];            // Assign value to adjacent pos
		 }
	// Done with token name and value extraction.  Now just
	// parse the array to find the one we want.

	var finaltokenvalue=null;                       // Default to null
	for (i=0;i<(tokenvals.length/2);i++)
		 {
		 if (tokenvals[i*2]==searchtoken)
			  {
			  finaltokenvalue=tokenvals[i*2+1];
			  }
		 // Display all token names and values
		 if (boolDebug)
			 {
			 document.write("[searchtoken:"+searchtoken+"][tokenName:"+tokenvals[i*2]+"] [tokenValue:"+tokenvals[i*2+1]+"]"+(tokenvals[i*2]==searchtoken)+"<BR>");
			 }
		 }
	return(finaltokenvalue);
}



//______________________________________________________________________________
//==============================================================================
//             					eExt.showBody
//------------------------------------------------------------------------------
// Debugging function to popup a window with the rendered BODY code
//
// Input: none
//
// Output: none
//
// [BrandonB] 7/3/2003
//______________________________________________________________________________
//==============================================================================

eExt.showBody=function(framename)
{

	if (framename==null)
		{
		var src=document.body.innerHTML;
		var h = document.getElementsByTagName("HEAD");

		}
	else
		{
		var src=parent.frames[framename].document.body.innerHTML;
		var h = parent.frames[framename].document.getElementsByTagName("HEAD");
		}
	var hsrc=h[0].innerHTML;
	hsrc=hsrc.replace(/</ig,"&lt;");
	hsrc=hsrc.replace(/>/ig,"&gt;");
	hsrc=hsrc.replace(/\x09/ig,"   ");
	hsrc=hsrc.replace(/\x0d\x0a/ig,"<br>");
	hsrc=hsrc.replace(/&gt;&lt;/ig,"&gt;<br>&lt;");
	hsrc="<pre>"+hsrc+"<\/pre>";

	hsrc="<b>&lt;HEAD&gt;</b>"+hsrc+"<"+"b>&lt;/HEAD&gt;"+"<"+"/b>";



	src=src.replace(/</ig,"&lt;");
	src=src.replace(/>/ig,"&gt;");
	src=src.replace(/\x09/ig,"   ");
	src=src.replace(/\x0d\x0a/ig,"<br>");
	src=src.replace(/&gt;&lt;/ig,"&gt;<br>&lt;");
	src="<pre>"+src+"<\/pre>";
	src=hsrc+"<br>"+"<b>&lt;BODY&gt;</b>"+src+"<"+"b>&lt;/BODY&gt;"+"<"+"/b>";
	eExt.showdebugWindow("<html><h1>Rendered DHTML <body> Source<\/h1><p>"+src+"<\/html>",600,600);

}



// Must generate a unique window name for each window instance, because otherwise
//	IE will only open ONE window when this is called from inside a modal dialog.
// I wonder if this is an IE bug, because it opens a new window each time
// when called from code NOT inside a modal dialog.
var debugwindowuniquename=0;

eExt.showdebugWindow=function(dwcontent,w,h)
{
	// Open new window, ensuring a unique name each time so multiple windows
	// can be opened when called from code inside a modal dialog.
	var windowpointerle=window.open("","dbwin"+debugwindowuniquename++,"height="+h+",width="+w+",resizable,scrollbars");

	if (windowpointerle)
		{
		windowpointerle.document.open();
		windowpointerle.document.write(dwcontent);
		windowpointerle.document.close();
		windowpointerle.focus();
		}
	else
		{
		alert("Cannot create debug window");
		}

}
eExt.showdebugWindowURL=function(url,w,h)
{
	// Open new window, ensuring a unique name each time so multiple windows
	// can be opened when called from code inside a modal dialog.
	var windowpointerle=window.open(url,"dbwin"+debugwindowuniquename++,"height="+h+",width="+w+",resizable,scrollbars");

	if (!windowpointerle)
		{
		alert("Cannot create url debug window");
		}
}

var gDebugStack="";

var dbgwinstackhd="<html><body style='font: small Tahoma'><H2>Castor UI Debug Call Stack </H2>";
var dbgwinstack="";
var dbgwinstackft="<hr><button onclick='javascript:window.opener.parent.gDebugStack=\"\";window.opener.parent.eExt.showdebugWindowRunning(\"\")'>Clear</button></body></html>";
var winpnterle=null;

eExt.showdebugWindowRunning=function(dwcontent)
{
	var w=800;h=400;
//    var isdebugwindowopen= windowpointerle==null?false:!windowpointerle.closed;
	var winpnterle=window.open("","dbguidbwin0","height="+h+",width="+w+",resizable,scrollbars");

	if (winpnterle)
		{
			winpnterle.document.open();

			if (parent.gDebugStack==undefined)
				{
				parent.gDebugStack=dwcontent;
				}
			else
				{
				parent.gDebugStack+=dwcontent;
				}
         winpnterle.document.write(dbgwinstackhd+parent.gDebugStack+dbgwinstackft);
			winpnterle.document.close();
			winpnterle.scrollTo(0,999999999);
			winpnterle.focus();      // This will pull the debug window to the front
//             windowpointerle.opener.focus();  // Put focus back to opening window to keep debug win from stealing input focus

		}
	else
		{
		alert("Cannot create debug window");
		}

}


eExt.debugCP=function(checkpointtxt, isEnabled)
{
	if ((parent.gUIDebug)&&(isEnabled))
		{
		eExt.showdebugWindowRunning(checkpointtxt+"<br>");
		}
}



//______________________________________________________________________________
//==============================================================================
//                              eExt.createDOMAPIElm
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


eExt.createDOMAPIElm = function(arg)
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


eExt.waitopenload=function()
{
document.write("<body bgcolor=buttonface>"+
						"<table id='waittable' width='100%' height='100%' style='background-color:buttonface;top:0;left:0;'>"+
						"<tbody>"+
						"<tr>"+
							"<td style='font-family:Arial Black;font-size:20px;text-align:center;vertical-align:center;color:black'>"+
							"<img src='/Graphics/Icons/ani_clock.gif'><span style='position:relative;top=-5px;'>&nbsp;&nbsp;"+
							parent.eLang.getSysString("STR_WAIT")+"<span>"+
							"</td>"+
						"</tr>"+
						"</tbody>"+
					"</table></body>");
}

eExt.waitopen=function()
{
//    wt=document.createElement('div');
//    wt.overflow='visible';
//    wt.style.backgroundColor='red';
//    wt.style.position='absolute';
//
//    wt.innerHTML="<table id='waittable' width='100%' height='100%' style='border:4px solid;background-color:buttonface;top:0;left:0;'>"+
//                   "<tbody>"+
//                   "<tr>"+
//                      "<td style='font-family:Arial Black;font-size:20px;text-align:center;vertical-align:center;color:black'>"+
//                      "<img src='/Graphics/Icons/ani_clock.gif'><span style='position:relative;top=-5px;'>&nbsp;&nbsp;"+
//                      eLang.getSysString("STR_WAIT")+"<span>"+
//                      "</td>"+
//                   "</tr>"+
//                   "</tbody>"+
//                "</table>";
//    document.body.appendChild(wt);
//

	wt=document.createElement('table');
	wt.id='waittable';
	wt.style.position="absolute";
	wt.width="100%";
	wt.height="100%";
	wt.style.backgroundColor="buttonface";
	wt.style.top=0;
	wt.style.left=0;
	wtb=document.createElement('tbody');
	wtr=document.createElement('tr');
	wtd=document.createElement('td');
	wtd.style.fontFamily="Arial Black";
	wtd.style.fontSize="20px";
	wtd.style.textAlign="center";
	wtd.style.verticalAlign="middle";
	wtd.style.color="black";

	wtd.innerHTML="<img src='/Graphics/Icons/ani_clock.gif'><span style='position:relative;top=-5px;'>&nbsp;&nbsp;"+
							parent.eLang.getSysString("STR_WAIT")+"<span>";

	wtr.appendChild(wtd);
	wtb.appendChild(wtr);
	wt.appendChild(wtb);
	document.body.appendChild(wt);



}

//-------------------------------------//
//-------------------------------------//

//-------------------------------------//
//-------------------------------------//

eExt.waitclose=function()
{
	var wtid=document.getElementById('waittable');
	if (wtid!=undefined)
		{
		document.body.removeChild(wtid);
		}
}

//-------------------------------------//
//-------------------------------------//
// Development routine to overlay a
// hashed grid in varying resolutions
// for object placement/layout
//-------------------------------------//
//-------------------------------------//

eExt.showScreenSize=function(sizeOfScreen)
{

scrdiv=document.createElement("DIV");
var resstr="";

with (scrdiv.style)
	{
	zIndex=-999;
	position='absolute';
	top=0;
	left=0;
	textAlign='right';
	fontFamily='Arial Black';
   }

switch (sizeOfScreen)
	{
	case 1:     // 1024x768
		with (scrdiv.style)
			{
			width=1024;
			height=768;
			border='2px dashed yellow';
			color='yellow';
			}
		resstr='1024x768';
		break;
	case 2:     // 1280x1024
		with (scrdiv.style)
			{
			width=1280;
			height=1024;
			border='2px dashed red';
			color='red';
			}
		resstr='1280x1024';
		break;
	case 0:     // 800x600
	default:
		with (scrdiv.style)
			{
			width=800;
			height=600;
			border='2px dashed white';
			color='white';
			}
		resstr='800x600';
		break;

	}

// Allocate a 'danger' area of 145 pixels at the bottom to account for user's
// address bar, buttons, title bar, link bar etc.

	var greyzoneheight=190;
	var greyzonescrollwidth=30;

	scrdivdanger=document.createElement("DIV");
	with(scrdivdanger.style)
		{
		position='absolute';
		top=parseInt(scrdiv.style.height)-greyzoneheight;
		left=0;
		height=greyzoneheight-5;
		width=parseInt(scrdiv.style.width)-5;
		backgroundColor='gray';
		textAlign='center';
		fontFamily='Arial Black';
		}

	scrdivdangerrt=document.createElement("DIV");
	with(scrdivdangerrt.style)
		{
		position='absolute';
		top=15;
		left=parseInt(scrdiv.style.width)-greyzonescrollwidth-5;
		height=parseInt(scrdiv.style.height)-20;
		width=greyzonescrollwidth;
		backgroundColor='gray';
		textAlign='center';
		fontFamily='Arial Black';
		}
//    scrdivdanger.innerHTML="This zone represents the user's link bar, icons, "+
//       "address bar, etc. which takes up approximately 145 pixels from the "+
//       "usable desktop space.  UI items should ideally be placed above this zone "+
//       "to prevent the user from having to scroll the screen.";
	scrdiv.appendChild(scrdivdangerrt);
	scrdiv.appendChild(scrdivdanger);

	resstrs=document.createElement("SPAN");
	resstrs.innerHTML=resstr;
	scrdiv.appendChild(resstrs);
	document.body.appendChild(scrdiv);

}

//-------------------------------------//
// These conversion utils use toFixed to
// round to the nearest decimal place
// for GB, or 0 places for MB, ie
//
//	1.999(1) -> 2.0
//-------------------------------------//
eExt.converttoGB=function(numberink,boolnumberonly)
{
	var newnum=new Number(numberink);
	newnum=newnum/1048576;
	if (boolnumberonly)
		{
		return(newnum.toFixed(1));
		}
	else
		{
		return(newnum.toFixed(1)+parent.eLang.getSysString("STR_UNITS_GB"));
		}

}
eExt.converttoMB=function(numberink,boolnumberonly)
{
	var newnum=new Number(numberink);
	newnum=newnum/1024;
	if (boolnumberonly)
		{
		return(newnum.toFixed(0));
		}
	else
		{
		return(newnum.toFixed(0)+parent.eLang.getSysString("STR_UNITS_MB"));
		}

}
eExt.converttoMBorGBString=function(numberinKB)
{
	if (numberinKB>=1048576)
		{
		return(eExt.converttoGB(numberinKB));
		}
	else
		{
		return(eExt.converttoMB(numberinKB));
		}
}

//-------------------------------------//
// The truncated versions do not round,
// they truncate any decimal places
// after the number specified. This
// is used in space calculations so
// user does not see more space than is
// really available, for example if there
// is 95.56GB free you don't want to
// show 95.6 and use that in calculations.
//	These use new function prototyped to
// the Number object, defined at the
// bottom of this file.
// [BrandonB] 6/22/2004
//
//	1.999(1) ->	1.9
//-------------------------------------//

eExt.converttoMBTruncated=function(numberink,boolnumberonly)
{
	var newnum=new Number(numberink);
	newnum=newnum/1024;
	if (boolnumberonly)
		{
		return(newnum.toTruncated(0));
		}
	else
		{
		return(newnum.toTruncated(0)+parent.eLang.getSysString("STR_UNITS_MB"));
		}

}
eExt.converttoGBTruncated=function(numberink,boolnumberonly)
{
	var newnum=new Number(numberink);
	newnum=newnum/1048576;
	if (boolnumberonly)
		{
		return(newnum.toTruncated(1));
		}
	else
		{
		return(newnum.toTruncated(1)+parent.eLang.getSysString("STR_UNITS_GB"));
		}

}
eExt.converttoMBorGBStringTruncated=function(numberinKB)
{
	if (numberinKB>=1048576)
		{
		return(eExt.converttoGBTruncated(numberinKB));
		}
	else
		{
		return(eExt.converttoMBTruncated(numberinKB));
		}
}


// Generic function, input = kHz whereas 1kHz=1000 Hz
eExt.converttoGHz=function(numberink,boolnumberonly)
{
	var newnum=new Number(numberink);
	newnum=newnum/1000000;

	if (boolnumberonly)
		{
		return(newnum.toFixed(1));
		}
	else
		{
		return(newnum.toFixed(1)+parent.eLang.getSysString("STR_UNITS_GHZ"));
		}


}


// Debugging function to expose all properties and methods of a given
// object
eExt.debugExposeProperties=function(object)
{
	var x;
	var str="";
	for (x in object)
		{
		str+="<b>["+x+"]</b> - ";

		if (typeof(object[x])=="function")
			{
			str+="FUNC - ";
			}
		else
			{
			if (typeof(object[x])==null)
				{
				str+="NULL - ";
				}
			else
				{
				str+="PROP - ";
				}
			}
		str+=object[x]+"<br>";
      }
	eExt.showdebugWindow(str,500,600);

}


eExt.enableControl=function(ctrl,boolEnable)
{
	ctrl.disabled=!boolEnable;	// HTML
//    See if DOMAPI --- "enabled" is a DOMAPI object property.
	if (ctrl.enabled!=undefined)
		{
		// DOMAPI
		ctrl.enabled=boolEnable;
		ctrl.reDraw();
		}
	// Handle IE quirks
	if (ctrl.type!=undefined)
		{
		//Style text boxes to lightgray if IE
		if ((ctrl.type=='text')||(ctrl.type=='password'))
			{
			ctrl.style.backgroundColor=(boolEnable?"":'lightgrey');
			}
		}

	// Handle Firefox quirks
	if (!document.all)
		{
		if (ctrl.tagName=='LABEL')
			{
			ctrl.style.color=(boolEnable?"":'#a0a0a0');
			}
		}

}

// A new function to add to the Number object to truncate a fractional portion
// after the decimal WITHOUT ROUNDING.  For example, 2.99999 with a numdigits
// value of 2 would return 2.99, whereas the builtin toFixed rounds it and
// returns 3.00.
// [BrandonB] 6/16/2004
Number.prototype.toTruncated=function(numdigits)
{
	var decimalmover=Math.pow(10,numdigits);
	return ( (Math.floor(this*decimalmover))/decimalmover );
}




//______________________________________________________________________________
//==============================================================================
//                             cleanupTextArea
//------------------------------------------------------------------------------
// Escapes any quotes from text areas and removes any new lines
//
// Input
// -----
//   string -
//
// Output
// ------
//    -
//
//____________________________________________________________________Author:BMB
//==============================================================================

eExt.cleanupTextArea=function (string)
{
	// remove newlines
	string=string.replace(/[\r\n]/g, " ");
	// escape quotes
	string=string.replace(/'/g, "\'");
	string=string.replace(/"/g, "\"");
	return (string);

}



//______________________________________________________________________________
//==============================================================================
//                          eExt.showMeThisObject
//------------------------------------------------------------------------------
// Small routine to take an object and write its contents to a popup window.
// Extremely useful for debugging object manipulation in Studio!
//
// Input
// -----
//   obj -
//
// Output
// ------
//    -
//
//                                                                    Author:BMB
//______________________________________________________________________________
//==============================================================================
// Jump Tag 1153944906 [  PROC - "eExt.showMeThisObject()" ]

eExt.showMeThisObject=function (obj)
{
	var str=(displayJSONStructure(obj, "", 1, 1, 1));
	eExt.showdebugWindow(str,500,600);
}


//______________________________________________________________________________
//==============================================================================
//                         eExt.getHTTPHeaderProps
//------------------------------------------------------------------------------
// Scans and resolves some parameters from an HTTP GET header
//
// Input
// -----
//   text - string of the entire HTTP header
//
// Output
// ------
//   object - returns and object with the following parameters:
//    languagestring : the string of the Accept-Language field
//    langs : an array of user-preferred languages, resolved to two letter,
//            upper case
//    useragent: the User-Agent field
//
//
//                                                                    Author:BMB
//______________________________________________________________________________
//==============================================================================
//Jump Tag 1158783913 [  PROC - "eExt.getHTTPHeaderProps()" ]

eExt.getHTTPHeaderProps=function(text)
	{
	var mainobj={};
	mainobj.languagestring="";
	mainobj.langs=[];

	// Accept-Language
   var myregexp = /Accept-Language:(.*)/;
	var match = myregexp.exec(text);
	if (match != null && match.length > 1)
		{
		mainobj.languagestring=match[1];
		}
	else
		{
		mainobj.languagestring="";
		}
	// Now process lang codes
	mainobj.langs=mainobj.languagestring.split(",");
	for (var i=0;i<mainobj.langs.length;i++)
		{
		mainobj.langs[i]=(/[a-z]+[,$]*/.exec(mainobj.langs[i])[0]).toUpperCase();
		}

	// User-Agent
   var myregexp = /User-Agent:(.*)/;
	match = myregexp.exec(text);
	if (match != null && match.length > 1)
		{
		mainobj.useragent=match[1];
		}
	else
		{
		mainobj.useragent="";
		}

	// Host
   var myregexp = /Host:(.*)/;
	match = myregexp.exec(text);
	if (match != null && match.length > 1)
		{
		mainobj.host=match[1];
		}
	else
		{
		mainobj.host="";
		}

	return mainobj;

	}

//______________________________________________________________________________
//==============================================================================
//                                 setOrgs
//------------------------------------------------------------------------------
// Takes the values of the controls defined in the source array and sets them
// to their current values.  Typically called when all data has been loaded
// on the page for the first time (to set initial values) and in the response
// handler of any write function.
//
// Input
// -----
//   arOrgs - array containing control data references.  The data is stored
// 				as pairs, with the first value a string to eval to get the
//					data.  The second value is the placeholder for the value
//					when doing a set operation with this function.
//
//   Example:
// 	var orgsIPrange=[
// 		"chkIPEnabled.checked", 0,
// 		"txtRangeAddr.value",	"",
// 		"txtRangeSubnetMask.value",	""
// 		];
//
// Output
// ------
//    None
//                                                                    Author:BMB
//______________________________________________________________________________
//==============================================================================

eExt.setOrgs=function(arOrgs)
{
	for (var i=0;i<arOrgs.length/2;i++)
		{
		arOrgs[i*2+1]=eval(arOrgs[i*2]);
		}
}

//______________________________________________________________________________
//==============================================================================
//                                checkOrgs
//------------------------------------------------------------------------------
// Checks the values of the controls defined in the source array to see
// if they have changed.
//
// Input
// -----
//   arOrgs - array containing control data references
//
// Output
// ------
//    boolean - true=changed, false=unchanged
//
//                                                                    Author:BMB
//______________________________________________________________________________
//==============================================================================
eExt.checkOrgs=function(arOrgs)
{
	var isChanged=false;
	var temp;
	for (var i=0;i<arOrgs.length/2;i++)
		{
		if (arOrgs[i*2+1]!=eval(arOrgs[i*2]))
			{
			isChanged=true;
			break;
			}
		}
	return isChanged;
}

eExt.loaded = true;