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

/*
File 		- events.js
Module 		- IWISP Library
Author		- chandrasekarr@amiindia.co.in

*/
/*
eType	= {}; //constant
eType.DOC_ON_LOAD 			= 0x0000;
eType.DOC_ON_CLICK 			= 0x0001;
eType.DOC_ON_DBLCLICK 		= 0x0002;
eType.DOC_ON_BLUR 			= 0x0003;
eType.DOC_ON_FOCUS			= 0x0004;
eType.DOC_ON_ABORT			= 0x0005;
eType.DOC_ON_CHANGE			= 0x0006;
eType.DOC_ON_ERROR			= 0x0007;
eType.DOC_ON_RESIZE			= 0x0008;
eType.DOC_ON_SELECT			= 0x0009;
eType.DOC_ON_KEYPRESS	 	= 0x0010;
eType.DOC_ON_KEYDOWN 		= 0x0011;
eType.DOC_ON_KEYUP 			= 0x0012;
eType.DOC_ON_MOUSEOVER 		= 0x0020;
eType.DOC_ON_MOUSEOUT 		= 0x0021;
eType.DOC_ON_MOUSEDOWN 		= 0x0022;
eType.DOC_ON_MOUSEUP 		= 0x0023;
eType.DOC_ON_MOUSEMOVE 		= 0x0024;
eType.DOC_ON_UNLOAD			= 0x0099;
*/

events	=	{

		eventBag	: [],

		register	:	function(eAction, fnPtr, obj)
		{
			//expected : eAction: "click"
			_debug(eAction+' listening on '+fnPtr);

			if(obj.addEventListener)
			{
				obj.addEventListener(eAction,fnPtr,false);
			}else if(obj.attachEvent)
			{
				obj.attachEvent("on"+eAction,fnPtr);
			}else
			{
				/* following not used */
				try{

					if(events.eventBag.indexOf(eAction)==-1)
						events.eventBag.push(eAction);
				}catch(e){}

				try{
					if(events.eventBag[eAction].length==undefined)
					{
						events.eventBag[eAction] = [];
					}
				}catch(e)
				{
					events.eventBag[eAction] = [];
				}
				try{
					if(events.eventBag[eAction].indexOf(fnPtr)==-1)
						events.eventBag[eAction].push(fnPtr);
					else
						_debug('Event already registered for this handler');
					_debug('Bag has now:'+events.eventBag[eAction].length);
				}catch(e){}
			}
		},

		process		: function(e)
		{
			if(!e) e = window.event;

			try{
				if(events.eventBag[e.type].length)
				{
					for(var j=0; j<events.eventBag[e.type].length; j++)
					{
						_debug('fn: '+events.eventBag[e.type][j]);
						return events.eventBag[e.type][j](e);
					}
				}
			}catch(e){}
		},

		unregister		: function(eAction, fnPtr)
		{
			try{
			if(events.eventBag[eAction].length)
			{
				for(var j=0; j<events.eventBag[eAction].length; j++)
				{
					if(events.eventBag[eAction][j]==fnPtr) break;
				}
				events.eventBag[eAction].splice(j,1);
			}
			}catch(e){}
		},

		getTargetObject	: function(e)
		{
			var tObj = null;
				if(e.target)
				{
					tObj = e.target;
				}else if(e.srcElement)
				{
					tObj = e.srcElement;
				}

				//safari assuming text node as target - solved
				if(tObj.nodeType==3)
				{
					tObj = tObj.parentNode;
				}

				return tObj;
		}

}

if(!document.attachEvent && !document.addEventListener)
{
onclick =
onload =
onbeforeunload =
onunload =
ondblclick =
onblur =
onfocus =
onabort =
onchange =
onerror =
onresize =
onselect =
onkeypress =
onkeydown =
onkeyup =
onmouseover =
onmouseout =
onmousedown =
onmouseup =
onmousemove = events.process;
};

events.loaded = true;
