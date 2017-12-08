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

// =============================================================================
// Bookmark Jump Tag Table - Use the tags to search for bookmarks in the code
// =============================================================================
// Jump Tags  - Description
// -----------------------------------------------------------------------------
// 1130445634 -  PROC - "xmit.goajax()"
// 1130445659 -  PROC - "xmit.getset()"
// 1130446077 -  PROC - "xmit.showdebugWindow()"
// 1130446081 -  PROC - "xmit.returnpath()"
// 1130446115 -  PROC - "xmit.striptags()"
// 1130446124 -  PROC - "xmit.encodespecial()"
// 1144877290 -  PROC - "xmit.dumpHex()"
// 1150915376 -  PROC - "xmit.get()"
// 1155222794 -  PROC - "xmit.checkJSON()"
// 1168986635 -  PROC - "xmit.debugOut()"
//
//----------------------------------------------------------------------------//
// DATA TRANSMISSION MODULE - XMIT.JS
//----------------------------------------------------------------------------//
// This file contains all routines used in client-server data transmission,
// including AJAX file requests and POST data methods for sending large amounts
// of data to a HAPI function.
// It is the collection of routines from sysfn and eExt modules, put in one
// place to help organize things.//
//
// Depends on:  eExt.js  - for debugging routine
//              eLang.js - for error message
//
//
// ERROR CODES: There are some error codes returned by the xmit module in the
// HAPI_STATUS field to indicate processing problems.
//
// 9998 - eval error
// 9997 - file not found
//
//
// [Brandon Burrell]
//----------------------------------------------------------------------------//
// HISTORY E-engine, D-debugger
// E2.0.0
// D2.0.0 - 011207 -	Made significant visual enhancements; added style sheet.
//	        			   Added ability to show hex for any response call.
//							Added new numbering system Request X.Y where X is the
//							total number of ajax calls (from upper frameset down
//							to current page for example) and Y is the number of calls
//							from the current page (ie when the xmit module was loaded)
// E2.0.1 - 011607 - Made some changes to help eliminate memory leaks
// D2.0.1 - 011607 - Improved hex dump, added routines to dump debug data outside
//							of the ajax handler (to clean it up)
// D2.0.2 - 030807 - Made small addition to data block to allow spaces in between
//							tokens so they have a place to break across lines (visual
//							change only in debug window)
// E2.0.2 - 031307 - Added ability to parse negative HAPI_STATUS
// E2.0.3 - 031407 - Added new global gAJAXAlertMissingASP to alert the user
//							when a request is made for a missing ASP file.  This only
//							applies to ASPs...
//----------------------------------------------------------------------------//
var AJAXDEBUGGERVER="D2.0.2"
var AJAXENGINEVER="E2.0.2"

var DUMMY_SERVER= false; //disable this in all real implementations. This is iwisp
						//specific for using dummy rpc calls.

var xmit={};

ajaxdatareqcount=0;	// Global counter that simply increments to give unique IDs
								 // to ajax objects

//______________________________________________________________________________
//==============================================================================
//                               xmit.goajax
//------------------------------------------------------------------------------
// AJAX OBJECT CONSTRUCTOR
//
// DO NOT CALL THIS FUNCTION DIRECTLY, USE GET OR GETSET, which will ensure
// that a NEW operator is used when creating sequential calls.  This is to make
// sure they don't collide.
//
//
// Input
// -----
//	.url = url of requested file
// .optarg = optional argument for use in the onrec handler
// .onrcv = reference to receive handler function, will be called when data has returned
//
//          onrcv is called with the following parameters:
//          timeout  = true/false to indicate the operation timed out.  You can also still
//                      use a separate function but this is easier.
//          url      = url of the requested file
//          xmitid   = a unique id assigned to each data transaction.  Just used for
//                      identifying objects.
//          optarg   = optional argument for error display purposes
//
// .timeout = value in seconds to wait for timeout
// .async = type of call to use, true=use asynch, false=sync.  I don't recommende
//				using this input, but I added support for it anyway.
//
//
// .username = username (optional)
// .password = password (optional)
//
//
// *** NOTE ***
// *** NOTE ***
// *** NOTE ***
//
// If you specify a path at all in the url, this function will take the exact
// path you specify.  Otherwise, it will construct a path to the application's
// RPC directory, and then based on gRPCpath.
//
// Examples:
// If arg.url =
// 	"getusers.asp" then the proper app path+rpc path will be constructed,
//    					ie /Applications/<appname>/RPC/getusers.asp
//    "/getusers.asp" then it will look in the webroot for the file
//
// Essentially, if there is a slash ANYWHERE in the url, it will use that
// path explicitely
//
//-------------------------------------//
// There are two ways to make successive goajax calls :
// 1) make each call in the response handler of the previous call (ie chain them)
// 2) Make sure you put NEW when you declare an xmit.ajax object so a new
//    instance is created.  Otherwise they will collide!
//
// Methods
// -------
// .add(name, value) = add a name and value hidden input control to url/form
// .send() = triggers send function of XMLHttpRequest object
//
//
//
// Output
// ------
//    -
//
//____________________________________________________________________Author:BMB
//==============================================================================
// Jump Tag 1130445634 [  PROC - "xmit.goajax()" ]

xmit.goajax = function(arg)
{
	var datapath=xmit.returnpath(arg.url);
	this.url = datapath;			// Fully qualified url with data
	this.data="";					// Keeps running data for GET/POST
	this.urlorg = arg.url;		// Keep copy of file path requested
	this.onrcv = arg.onrcv;
   this.timeoutvaluems=arg.timeout==undefined?60000:arg.timeout*1000;	// converted to milliseconds
	this.ontimeout = arg.ontimeout==undefined?arg.onrcv:arg.ontimeout;
	this.onerror = (arg.onerror == undefined) ? "" : arg.onerror;
	this.optarg=(arg.optarg==undefined?"":arg.optarg);	// User argument to pass to receive handler
	this.async=arg.async==undefined?true:arg.async;	// true=async, false=sync

	this.errState=0;
	this.method="GET";
	this.username=arg.username==undefined?"":arg.username;
	this.password=arg.password==undefined?"":arg.password;
	this.postdata=arg.postdata==undefined?null:arg.postdata;
	this.get="";	// Debugging string
	this.show_progress=arg.show_progress==undefined?true:arg.show_progress;

//    var dt=new Date();
//    this.idnum=dt.getUTCMilliseconds()+ajaxdatareqcount++;  // just a unique id numer
	this.idnum=ajaxdatareqcount++;

	if (top.gDebugwindowReqID==undefined)
		{
		top.gDebugwindowReqID=0;
		}
	this.totidnum=top.gDebugwindowReqID++;


	this.evalit=arg.evalit==undefined?true:arg.evalit;

   //-------------------------------------//
	// xmit.add()
   //-------------------------------------//
	this.add = function(name, val)
		{



		if (/^false$/.test(val))
			{
			alert(name+" is \"false\" instead of 0")
			}
		if (/^true$/.test(val))
			{
			alert(name+" is \"true\" instead of 1")
			}





		if (parent.gAJAXDebug==true)
			{
			if (typeof(val)=='string')
				{
				var valshow=xmit.striptags(val);
				}
			else
				{
				valshow=val;
				}

			// Dont add szContent keys for file writes unless gAJAXDebugShowPOST is defined
			if (parent.gAJAXDebugShowPOST==true)
				{
				this.get+="<b>"+name+"</b> ="+valshow+"<br>";
				}
			else
				{
				// Don't want to show actual data, but do want to snag any other data vars
				if (/szContent/.exec(name)==null)
					{
					this.get+="<b>"+name+"</b> = "+valshow+"<br>";
					}
				}
			}

		// Keep running string of all data, regardless of GET or POST
		this.data=(this.data==""?(name+"="+xmit.encodespecial(val)):this.data+"&"+name+"="+xmit.encodespecial(val));
		}

   //-------------------------------------//
	// xmit.send()
   //-------------------------------------//
   this.send = function()
		{
		if(this.show_progress)
			showWait(true,"Requesting");
		// Check for data length OR any non-alphanums in the data.  If so use POST.
		if ( ((this.url+this.data).length>512)  ||(/[^_a-zA-Z0-9]/.exec(this.data)!=null))
			{
			// Use POST method
			this.method="POST";
			this.postdata=this.data;
			if (parent.gAJAXDebugShowPOST==false)
				{
				this.get+="<br><i>Input data may be too large to show for POST method, set \"gAJAXDebugShowPOST=true\" to override</i>";
				}
			}
		else
			{
			// GET method, put data in URL
			this.method="GET";
			this.data=(this.data);
			this.url=(this.data==""?this.url:this.url+"?"+this.data);
			this.postdata=null;

			}
		//-------------------------------------//
		xmit.debugOut(this, "SEND");
			// Third arg : true=async, false=sync
		this.xmldoc.open(this.method, this.url, this.async, this.username, this.password);

		if (this.method=='POST')
			{
			this.xmldoc.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
			this.xmldoc.setRequestHeader("Content-length", this.postdata.length);
			this.xmldoc.setRequestHeader("Connection", "close");
			}
		// If posting data, it is a string of name/value pairs, as in  "myvar1=123&myvar2=345"
		this.xmldoc.send(this.postdata);

		}
	//----------------------------------------------------------------------------//
	// This function is called from event and timeout handlers (ie 'THIS' will
	// not be correct, so parent object reference must be given via the
	// datao parameter. [BrandonB] 8/18/2005
	//----------------------------------------------------------------------------//
	this.ajaxhandler=function(datao)
		{
		var hapivalerrmsg="";
		if (datao.xmldoc.readyState == 1) { 
		//to check for any error generated while operation is ongoing.
			datao.xmldoc.onerror = this.onerror;
		}
		if ( (datao.xmldoc.readyState==4)&&(datao.errState!=99))
			{
			if(datao.show_progress)	showWait(false);
			//-------------------------------------//
			// Operation is complete, process the data
			//-------------------------------------//
			clearTimeout(datao.timeout);
			var docerror=false;


			// These vars represent the actual text received...
			// datatxt is the raw response, and datatxtafter is the processed
			// data (if needed).
			var datatxt=datao.xmldoc.responseText;

			// action : check for session timeout purpose
			//Status get HTML page due to serverside redirection of page to sessionexpired.asp
			//That causes the expecting json record to be shown as script error also there is no action from client side after that.
			//So we check if it is valid here if not we move it to login page
			// caution : If the return record is not of json format then its moved to login page
			if(datatxt.match('<html>') && (datatxt.match('expired') || datatxt.match('Access Denied')))
			{
				//parent.location.href = "login.html";
				top.document.cookie = "SessionExpired=true;path=/";
				top.location.href = "/index.html";
			}

			var datatxtafter=xmit.checkJSON(datatxt);

			var hapismatch=/(HAPI_STATUS[ \t"]*:[ \t]*([\-0-9]+))/.exec(datatxtafter);
			var hapival=null;
			if (hapismatch!=null)
				{
				hapival=hapismatch[2]/1;
				}
			else
				{
				try{
					if (datao.xmldoc.status!=200)
					{
						docerror=true;
						var bIsASPfile=/\.asp/.test(datao.url);

						// Process possible error situations here, and set codes
						if (datao.xmldoc.status==404)
						{
							hapival=9997;
							hapivalerrmsg="File ["+datapath+"] was not found <br>AJAX Status:("+datao.xmldoc.status+":"+datao.xmldoc.statusText+")";

							if (parent.gAJAXAlertMissingASP==true)
							{
								alert("Unable to communicate with BMC to load ASP file ["+datapath+"].");
							}
							datao.onrcv({timeout:false,
										status:datao.xmldoc.status,
										statustext:'Page not found',
										HAPI_STATUS:9998})

							return;
						}

					}
				}catch(e)
				{
					//do nothing
					//this try statement was introduced for unknown exception thrown
					//by mozilla when datao.xmldoc has no return html status
				}
				}

			xmit.debugOut(this, "REC", datatxt, hapival, hapivalerrmsg);

			// If I get a hapival of 7001-7009, then something is wrong with the session.
			// Go ahead and throw an exception to handle it.
			if ( (hapival>=7000)&&(hapival<=7009))
				{
				// Handler for session errors thrown by HAPI codes 7001-7009
				xmit.alertError(hapival);
				if (parent.application_sessionTimedOut!=undefined)
					{
					parent.application_sessionTimedOut(hapival);  // Call timeout handler
					// For mem leak issues, mark for garbage collection
					// [BrandonB] 1/16/2007
					datao.xmldoc=null;
					datao.onrcv=function(){};
					return;
					}
				}

			// This will eval the response text and place it into the DOM.
			// If the response contains variables with no VAR definition, it will be placed
			// in the global scope (for example, HAPI ASP files, or string files).
			// If the response contains any code, it will execute HERE, in this
			// function's scope.  Likewise any function definitions will exist only
			// within this function's scope.
			// So, if you want to use ajax to load a file with function definitions,
			// use the readFromFile function, and in the response handler
			// eval(RPC_FILEREAD.szContent) to place its contents into the DOM
			// at THAT scope.

			if ( (datao.evalit==true)&&(docerror==false))
				{
				try
					{
					eval(datatxtafter);

					xmit.debugOut(this, "EVALOK");


					}
				catch(err)
					{
//					NOTE:  The code within this catch is only executed under debug
//					conditions to prevent errors being thrown
//					when the user navigates away from a page that is loading a large
//					RPC data payload.  In this case, the JSON structure may be
//					incomplete and malformed, but since we are navigating away we
//					don't care.
//					The disadvantage to this is that we won't know the structure is
//					invalid if it is malformed due to errors with the server macro
//					code.
					xmit.debugOut(this, "EVALOK", datatxtafter);

					if (parent.gAJAXJSONEvalDebug)
						{
						alert("An error occurred when trying to interpret data from the server! \nURL:"+datao.url+" --- \nERR:"+err);
						// Go ahead and call response handler
						datatxtafter="";
						hapival=9998;	// Eval error



						datao.onrcv({timeout:false,
										url:datao.url,
										id:datao.idnum,
										optarg:datao.optarg,
										data:"",
										dataraw:"",
										status:datao.xmldoc.status,
										statustext:datao.xmldoc.statusText,
										HAPI_STATUS:9998})
						}
					// Do this inside of catch...
					// For mem leak issues, mark for garbage collection
					// [BrandonB] 1/16/2007
					datao.onrcv=function(){};
					datao.xmldoc=null;

					}

				}

			try
			{
				datao.onrcv({timeout:false,
							url:datao.url,
							id:datao.idnum,
							optarg:datao.optarg,
							data:datatxtafter,
							dataraw:datatxt,
							HAPI_STATUS:hapival,
							status:datao.xmldoc.status,
							statustext:datao.xmldoc.statusText
							})
			}
			catch(e){}
			// For mem leak issues, mark for garbage collection
			// [BrandonB] 1/16/2007
			datao.xmldoc=null;
			datao.onrcv=function(){};

			}
		else
			{
			if (datao.errState==99)
				{
				showWait(false);
				// Here, I must check the readystate first to see if it has
				// already completed (state=4) If not, abort the ajax request.
				// Note that doing so will cause the handler to fire again with
				// readystate=4, which is why I must check it...otherwise I
				// get stuck in a loop. [BrandonB] 1/27/2006
				if (datao.xmldoc.readyState!=4)
				{
					datao.xmldoc.abort();
				}
				else
				{
					xmit.debugOut(this, "TIMEOUT");

					datao.onrcv({timeout:true,
									url:datao.url,
									id:datao.idnum,
									optarg:datao.optarg,
									data:null,
									dataraw:datatxt,
									HAPI_STATUS:9999,
									status:datao.xmldoc.status,
									statustext:datao.xmldoc.statusText
									});
				}
				// For mem leak issues, mark for garbage collection
				// [BrandonB] 1/16/2007
				datao.xmldoc=null;
				datao.onrcv=function(){};
				}
			}
		}

   //-------------------------------------//
	// Init
   //-------------------------------------//
	if (window.XMLHttpRequest)
		{
		this.xmldoc= new XMLHttpRequest();
		// branch for IE/Windows ActiveX version
		}
	else
		{
		if (window.ActiveXObject)
			{
			this.xmldoc= new ActiveXObject("Microsoft.XMLHTTP");
			}
		}
	if (this.xmldoc)
		{
		// create some references to the parent object so 'this' pointer
		// will be correct inside handlers
		var t=this;
		this.xmldoc.onreadystatechange = function()
			{
			t.ajaxhandler(t);
			}
		this.timeout=setTimeout(function()
			{
			t.errState=99;
			t.ajaxhandler(t);
			},this.timeoutvaluems);
		}

	if (parent.gSessionCheck==true)
		{
		if (!parent.application_checkSessionValid())	// Call app.js function to check valid session
			{
			parent.gSessionCheck=false;	// Let login page reload!
			this.send=parent.application_sessionTimedOut;
			}
		}

	return this;
}


//______________________________________________________________________________
//==============================================================================
//                              xmit.debugOut
//------------------------------------------------------------------------------
// Single entry point for output to debug window.  Just makes the
// actual ajax handler a bit cleaner.
//
// Input
// -----
//   dataob -
//
//   phase -
//
//   datatxt -
//
//   hapival -
//
//   hapimsg -
//
// Output
// ------
//    -
//
//                                                                    Author:BMB
//______________________________________________________________________________
//==============================================================================
// Jump Tag 1168986635 [  PROC - "xmit.debugOut()" ]

xmit.debugOut=function(dataob, phase, datatxt, hapival, hapimsg)
{
	if (parent.gAJAXDebug==true)
		{
		// Show all response data regardless of file type (string files, help files, etc)
		var bAJAXDebugShowAllResponseData = (parent.gAJAXDebugShowAllResponseData==undefined)?false:parent.gAJAXDebugShowAllResponseData;
		// Just show data from .asp files
		var bAJAXDebugShowOnlyASPResponseData = (parent.gAJAXDebugShowOnlyASPResponseData==undefined)?false:parent.gAJAXDebugShowOnlyASPResponseData;
		var  output="";
		switch (phase)
			{
         //-------------------------------------//
			case "SEND":
				var data=dataob.get==""?"None":dataob.get;
				var databg=dataob.data==""?"":"sendBlockData";
				output="<div class='sendBlock' ><div class='sendBlockTitle'>Request #"+
					(dataob.totidnum+1)+"."+(dataob.idnum+1)+
					") --- From: "+window.name+" ["+location.pathname+"] METHOD:"+dataob.method+"</div>"+
					"<div class='sendBlockDataInputs'><b><u>File</u></b><br>"+
					"<b>Original URL :</b> "+dataob.urlorg+
					"<br>"+
					"<b>Converted URL :</b> "+dataob.url+
					"<p><b><u>Data</u></b><br><div class='"+databg+"'>"+
						dataob.data.replace(/&/g," & ")+	// give data tokens a place to break
					  "</div></div></div>";
				break;

         //-------------------------------------//
			case "REC":
				var rettextsnippet=datatxt;

				var bIsASPfile=/\.asp/.test(dataob.url);
				if ( (bAJAXDebugShowAllResponseData==false) || ( (bAJAXDebugShowAllResponseData==true)&&(bAJAXDebugShowOnlyASPResponseData==true)&&(!bIsASPfile)) )
					{
					if (datatxt.length>150)
						{
						rettextsnippet=datatxt.substr(0,150)+"......... (truncated, set gAJAXDebugShowAllResponseData=true to override)";
						}
					}
				if (!(/[^\n\r\s]/g.test(rettextsnippet)))
					{
					rettextsnippet="(File has no text)";
					}
				else
					{
					rettextsnippet=xmit.striptags(rettextsnippet);
					}
				//-------------------------------------//
				// Set up hapi status
				//-------------------------------------//
				var hapistatusmsg="<p><div class='hapinone'>*** (NO HAPI STATUS) ***</div>";
				if (hapival!=null)
					{
					var hvstyle="hapiok";
					if (hapival!=0)
						{
						hvstyle="hapierror";
						}
					hapistatusmsg="<p><div class='"+hvstyle+"'>"+
						"HAPI_STATUS = "+hapival+
						"<span class='hapierrordesc'>"+
						(hapimsg==""?"":"<br>"+hapimsg)+
						"</span>"+
						"</div>";
					}
				hapistatusstr="<p><table width='99%' border='0'><tbody><tr>"+
					"<td width='40%'>&nbsp;</td>"+
					"<td align='right'>"+
					hapistatusmsg+
					"</td></tr></tbody></table>";
				//-------------------------------------//
				// Show rec debug window
				//-------------------------------------//
				var bgASP=bIsASPfile?"recBlockDataASP":"recBlockDataNonASP";
				var hexbutton="";
				var dhex="";
				if (parent.gAJAXDebugShowHex)
					{
					dhex="<div class='recBlockHexDump' id='hexdiv"+(dataob.totidnum+1)+"_"+(dataob.idnum+1)+"' >"+
						xmit.dumpHex(datatxt)+"</div>";
					hexbutton="<input type=button onclick=javascript:document.getElementById('hexdiv"+
						(dataob.totidnum+1)+"_"+(dataob.idnum+1)+
						"').style.display='block' value='Show hex' />";
					}
				output="<div class='recBlock'><div class='recBlockTitle'>Request #"+
					(dataob.totidnum+1)+"."+(dataob.idnum+1)+
					") --- <span class='recBlockTitleRecMsg'>Received (<span style='color:black;font-weight:normal'>"+
					dataob.url+
					"</span>) <br>From: "+
					window.name+
					" ["+
					location.pathname+"]</span></div>"+
					"<br><br><div class='"+bgASP+"'>"+rettextsnippet+
					"</div>"+
					hexbutton+
					dhex+
					hapistatusstr+
					"</div>";
				break;

         //-------------------------------------//
			case "EVALOK":
				output="<div class='recBlockEvalSuccess'><div class='recBlockEvalSuccesstitle'>Request #"+
					(dataob.totidnum+1)+"."+(dataob.idnum+1)+
					") --- <span class='recBlockEvalSuccesstitleMsg'>Eval Succeeded On File: "+
					"<span style='color:black;font-weight:normal'>"+
					dataob.url+
					"</span>"+
					"<br>From: "+window.name+" ["+location.pathname+"]</span></div></div>";
				break;
         //-------------------------------------//
			case "EVALFAIL":
				var hex="";
				if (parent.gAJAXDebugShowHex==true)
					{
					// Show hex dump of data
					hex="<p><h2>Data Dump:</h2><div class='recBlockEvalFileHexDump'><pre>"+xmit.dumpHex(datatxt)+"</pre></div>";
					}
				output="<div class='recBlockEvalFail'><div class='recBlockEvalFailtitle'>Request #"+
					(dataob.totidnum+1)+"."+(dataob.idnum+1)+
					") --- <span class='recBlockEvalFailtitleMsg'>EVAL FAILED On File: "+
					"<span style='color:yellow;font-weight:normal'>"+
					dataob.url+
					"</span>"+
					"<br>From: "+window.name+" ["+location.pathname+"]</span></div>"+hex+"</div>";


				break;

         //-------------------------------------//
			case "TIMEOUT":
				output="<div class='recBlockTimeout'>Request #"+
					(dataob.totidnum+1)+"."+(dataob.idnum+1)+
					") --- TIMED OUT "+
					"<div class='recBlockTimeouttitle'> for File: "+
					dataob.url+
					"<br>From: "+window.name+" ["+location.pathname+"]"+
					"<br>Timer Setting: "+(dataob.timeoutvaluems/1000)+"seconds"+
					"</div></div>";
				break;
			default:
				break;
			}

		xmit.showdebugWindow(output);
		}
}




xmit.gwgetsetFlash=function(functionname, varname, onrechandler)
{
	xmit.gwgetset(functionname, "libifc_flasher.so.1.0", varname, onrechandler);
}
xmit.gwgetsetLDAP=function(functionname, varname, onrechandler)
{
	xmit.gwgetset(functionname, "libifc_ldap", varname, onrechandler);
}

xmit.gwgetsetIPMI=function(functionname, varname, onrechandler)
{
	xmit.gwgetset(functionname, "libifc_ipmi", varname, onrechandler);
}
xmit.gwgetsetUser=function(functionname, varname, onrechandler)
{
	xmit.gwgetset(functionname, "libifc_userm", varname, onrechandler);
}


xmit.gwgetset=function (functionname, library, varname, onrechandler)
{
	var arg={url:"/RPC/ajaxgw.asp", onrcv:onrechandler, varname:varname};
   var tempdata=new xmit.goajax(arg);
	tempdata.add("FUNCTION",functionname);
	tempdata.add("LIB",library);
	tempdata.add("FUNCTION",functionname);
	tempdata.send();

}


//______________________________________________________________________________
//==============================================================================
//                                 xmit.get
//------------------------------------------------------------------------------
//
// Simple abstracted function that just gets a file.  Only difference is that
// there is no data, and I do the send() here instead of requiring the UI code
// to do it.
//
// Input
// -----
//   arg - args that would be passed to goajax
//
// Output
// ------
//    -
//
//____________________________________________________________________Author:BMB
//==============================================================================
// Jump Tag 1150915376 [  PROC - "xmit.get()" ]

xmit.get= function(arg)
{
	var tempdata=new xmit.goajax(arg);
	tempdata.send();
	delete tempdata;
}
//______________________________________________________________________________
//==============================================================================
//                                 xmit.getset
//------------------------------------------------------------------------------
// Generic routine to get data that needs to have some parameters set first.
//
// Input
// -----
//   arg - args that would be passed to goajax
//
// Output
// ------
//    -
//
//____________________________________________________________________Author:BMB
//==============================================================================
// Jump Tag 1130445659 [  PROC - "xmit.getset()" ]
xmit.getset= function(arg)
{
	var tempdata=new xmit.goajax(arg);
	return tempdata;
}

//______________________________________________________________________________
//==============================================================================
//                             xmit.returnpath
//------------------------------------------------------------------------------
// Translates the given url to the appropriate path based on state of settings.js
// NOTE: This is only done for .asp files...all other extensions pass through
// unaltered.
//
// Input
// -----
//   url -
//
// Output
// ------
//    -
//
//____________________________________________________________________Author:BMB
//==============================================================================
// Jump Tag 1130446081 [  PROC - "xmit.returnpath()" ]

xmit.returnpath=function(url)
{
	if(DUMMY_SERVER==true)
	{
		url = '../dummy'+url;
	}
	return url;
}



//______________________________________________________________________________
//==============================================================================
//                           xmit.showdebugWindow
//------------------------------------------------------------------------------
//
//
// Input
// -----
//   dwcontent - content to display
//
// Output
// ------
//    -
//
//____________________________________________________________________Author:BMB
//==============================================================================
// Jump Tag 1130446077 [  PROC - "xmit.showdebugWindow()" ]
var debugwindowstackhd="<html><head><title>AJAX Debugging Window</title>"+
	"<style>pre{font-size:12px}</style>"+
	"<link href='/GUISE/xmit.css' rel='stylesheet' type='text/css'></head>"+
	"<body><H2>AJAX Debug Window</H2><h3>Debugger Version:"+AJAXDEBUGGERVER+", Engine Version: "+AJAXENGINEVER+"<br>BrandonB</h3>";
var debugwindowstack="";
var debugwindowstackft="<hr><center>"+
	"<button onclick='javascript:window.opener.parent.gDebugwindowstack=\"\";"+
		"window.opener.parent.gDebugwindowReqID=0;window.opener.parent.xmit.showdebugWindow(\"\")'>Clear Debugger History</button>"+
		"</center></body></html>";
var windowpointerle=null;


// var gDebugwindowstack="";	// Changes this to clear debugger stack with every page load
									// because it makes the window faster.  [BrandonB] 1/11/2007

xmit.showdebugWindow=function(dwcontent)
{
	var w=600;h=800;
//    var isdebugwindowopen= windowpointerle==null?false:!windowpointerle.closed;
	windowpointerle=window.open("","ajaxdbwin0","height="+h+",width="+w+",resizable,scrollbars");

	if (windowpointerle)
		{
			windowpointerle.document.open();

// Just use local global instead of trying to create one in the parent so
// stack gets cleared whenever xmit is loaded. [BrandonB] 1/11/2007
			if (top.gDebugwindowstack==undefined)
				{
				top.gDebugwindowstack=dwcontent;
				}
			else
				{
				top.gDebugwindowstack+=dwcontent;
				}

// 			top.gDebugwindowstack+=dwcontent;


			var runningstack=top.gDebugwindowstack;
// 			var runningstack=gDebugwindowstack;



         windowpointerle.document.write(debugwindowstackhd+runningstack+debugwindowstackft);
			windowpointerle.document.close();
			windowpointerle.scrollTo(0,999999999);
			windowpointerle.focus();      // This will pull the debug window to the front
//             windowpointerle.opener.focus();  // Put focus back to opening window to keep debug win from stealing input focus

		}
	else
		{
		alert("Cannot create debug window");
		}

}

//______________________________________________________________________________
//==============================================================================
//                              xmit.striptags
//------------------------------------------------------------------------------
// Converts tags from any file read to HTML displayable form (ie so I can
// see the tags instead of having them interpreted).
//
//
// Input
// -----
//   text -
//
// Output
// ------
//    -
//
//____________________________________________________________________Author:BMB
//==============================================================================
// Jump Tag 1130446115 [  PROC - "xmit.striptags()" ]

xmit.striptags=function(text)
{
	text=text.replace(/</g,"&lt;").replace(/>/g,"&gt;");
	text=text.replace(/\xd|\xa/g,"<br>");
	// replace any newline (\n) sequences with <br> so text file streams
	// will have a place to break
	text=text.replace(/\\n/g,"<br>");

	return text;
}


//______________________________________________________________________________
//==============================================================================
//                            xmit.encodespecial
//------------------------------------------------------------------------------
// Have to specially encode potentially troublesome characters in the value
// field for the name/value pairs when sending data to the server.
// These include ; / ? : @ & = + $ , #
//
//
// Input
// -----
//   value -
//
// Output
// ------
//    -
//
//____________________________________________________________________Author:BMB
//==============================================================================
// Jump Tag 1130446124 [  PROC - "xmit.encodespecial()" ]

xmit.encodespecial=function(value)
{
	if (typeof(value)=='string')
		{
		// MUST encode percent first, since that is the character used to encode
		// everything else!  I could include this in the one liner below but
		// I want to do it explicely here for clarity.  This will screw up
		// source code badly if not done correctly. [BrandonB] 10/19/2005
		value=value.replace(/%/g,"%25");

		// Now just encode all other characters
		value=value.replace(/\+/g,"%2B").replace(/&/g,"%26").replace(/#/g,"%23");
		}

	return value;

}



//----------------------------------------------------------------------------//

//______________________________________________________________________________
//==============================================================================
//                               xmit.dumpHex
//------------------------------------------------------------------------------
// Takes a string and returns a hex-dump formatted string
//
// Input
// -----
//   inputstr -
//
// Output
// ------
//    -
//
//______________________________________________________________________________
//==============================================================================
// Jump Tag 1144877290 [  PROC - "xmit.dumpHex()" ]

xmit.dumpHex=function(inputstr)
{

	var slen=inputstr.length;
	var dumpstr="";
	var digitsvalue, digitsstr;
	var leftside="", rightside="";

	for (var i=0;i<slen/16;i++)
		{

		leftside="";rightside="";

		for (var x=0;x<16;x++)
			{
			digitstr=inputstr.substr(i*16+x,1);

      if (digitstr=="")
				{
				digitsvalue="..";
				}
			else
				{
				digitsvalue=digitstr.charCodeAt(0).toString(16);
				if (digitsvalue.length==1)
					{
					digitsvalue="0"+digitsvalue;
					}
				switch (digitsvalue)
					{
					case "0d":
					case "0a":
					case "09":
						digitstr=".";
						break;
					case "3c":
						digitstr="&lt;";
						break;
					case "3e":
						digitstr="&gt;";
						break;
					default:
						break;
					}
				}
			leftside+=digitsvalue+" ";
			rightside+=digitstr + "";
			}
		dumpstr+=leftside+" ---- "+rightside+"<br>";

		}
	return dumpstr;
	}


xmit.alertError=function(errnum)
{
	// Error numbers for login are 7000+err index
	errnum=parseInt(errnum)-7000;
	var strerrcod="STR_LOGINERRORCODE_"+errnum;

	if (eLang.strings[strerrcod]!= undefined)
		{
		alert(eLang.strings[strerrcod]);
		}
	else
		{
		if (erStr[errnum]!=undefined)
			{
			alert(erStr[errnum]);
			}
		else
			{
			alert("Unknown XMIT Session Error :"+errnum+7000);
			}
		}
}


//______________________________________________________________________________
//==============================================================================
//                              xmit.checkJSON
//------------------------------------------------------------------------------
// Function to do any parsing of the returned data (specifically JSON structures).
//
// Here, I attempt to search for and strip out any extraneous empty objects inside
// arrays that might be returned by older versions of the web server.  The blank
// objects do NOT need to be there and will throw off the array item count.

// Invalid structures:
//
// Case 1:
// VAR={
// 	MYVAR:[
// 		{}
// 		],HAPI_STATUS:0}
//
// Case 2:
// VAR={
// 	MYVAR:[
// 		{somedata:1, somemoredata:2},
// 		{}
// 		],HAPI_STATUS:0}
//
// In case 1,
// FROM -> {}],HAPI_STATUS
//   TO -> ], HAPI_STATUS
//
// In case 2,
// FROM -> ,{}],HAPI_STATUS
//   TO -> ], HAPI_STATUS
//
// In a nutshell, I am globally replacing any pattern of ,{}] or {}] with ]
//
// 																			[BrandonB] 8/10/2006
//
// Input
// -----
//   jsonstruc -
//
// Output
// ------
//    -
//
//                                                                    Author:BMB
//______________________________________________________________________________
//==============================================================================
// Jump Tag 1155222794 [  PROC - "xmit.checkJSON()" ]

xmit.checkJSON=function(jsonstruc)
{
	// Check for rogue commas for example:
	// {prop:1,
	//  prop2:2, <--
	// }
	// or
	//		[ elm1,
	// 	  elm2, <--
	// 	]

	if (/,[\r\n\s]*[\]}]/.test(jsonstruc))
		{
		alert("WARNING: XMIT AJAX module detected an extra comma after the last "+
		"element in an array or object property list...please check and correct.");
		}
	return jsonstruc.replace(/(,[\r\n\s]*)?\{[\r\n\s]*\}[\r\n\s]*\][\r\n\s]*/g, "]");
}

xmit.head = function(url,onrcv)
{
	var xdoc = null;
	try{
		xdoc = new XMLHttpRequest();
	}catch(e)
	{
		try{
			xdoc = new ActiveXObject("Microsoft.XMLHTTP");
		}catch(e)
		{
			xdoc = null;
		}
	}
	if(xdoc)
	{
		xdoc.open('HEAD',url,true);
		xdoc.onreadystatechange = function(){
			if(xdoc.readyState==4)
			{
				var arg={
					status: xdoc.status,
					responseHeaders: xdoc.getAllResponseHeaders().split('\n')
				};
				onrcv(arg);
			}
		};
		xdoc.send(null);
	}
}

// Strings to use when xmit module is being used outside of Castor...
var erStr=[];
erStr[1]="Invalid session. Please try logging in again.";
erStr[2]="Session timed out. Please log in again.";
erStr[3]="Maximum number of sessions already in use.";
erStr[4]="Invalid session. Please try logging in again.";
erStr[5]="Invalid session. Please try logging in again.";
erStr[6]="Invalid session. Please try logging in again.";
erStr[7]="Login failed. Plase try again.";
erStr[8]="Logged out. Please log in again to access the device."; //logged out properly and cookie is set to logged out";
erStr[9]="Please log in to access the device.";

xmit.loaded =true;
