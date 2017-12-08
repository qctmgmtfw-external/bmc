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
File 		- menu.js
Module 		- IWISP Library
Author		- chandrasekarr@amiindia.co.in

*/

oMenu = 
{

/* class functions */

	menuBar				:	null,

	createMenuBar		:	function(oNav) //oNav will be top most navigation object
	{
		var dMenuBar = document.createElement('div');
		dMenuBar.className = 'defaultMenu';
		this.menuBar = dMenuBar;

		for(var i=0;i<oNav.items.length; i++)
		{
			if (oNav.items[i].enabled == 1)
				oNav.items[i].anc = this.createMenu(oNav.items[i]);
		}

		//add Help
		this.createHelpLink();
		var fffix = document.createElement('div');
		fffix.style.clear = 'both';
		this.menuBar.appendChild(fffix);
		return this.menuBar;
	},

	enableFeaturesList		:	function(oNav)
	{
		if ((top.settings.features != undefined) && (top.settings.features.length > 0)){
			for(var i = 0; i < oNav.items.length; i++)
			{
				if(oNav.items[i].userprops.feature != "COMMON") {
					if (top.settings.features.indexOf(oNav.items[i].userprops.feature) != -1) {
						oNav.items[i].enabled = 1;
					}
				}

				if (oNav.items[i].nav != "") {
					oMenu.enableFeaturesList(eval(oNav.items[i].nav));
				}
			}
		}
	},

	createHelpLink		:	function()
	{
		var	help = document.createElement('a');
		help.href = 'javascript:top.toggleHelp();';
		help.innerHTML = eLang.getString('common','STR_HELP');
		help.className = 'helpBtn';
		help.title = eLang.getString('common','STR_HELP_TOOLTIP');
		this.menuBar.appendChild(help);
	},

	createMenu			:	function(oNavigation)
	{
		var dMenu = document.createElement('a');
		dMenu.href = "javascript://";

		if(oNavigation.func!="")
		{
			dMenu.onclick = eval(oNavigation.func.replace(/\(.*\)/ig,''));
		}

		if(oNavigation.page)
		{
			dMenu.href = '..'+oNavigation.page;
			dMenu.target = 'pageFrame';
			dMenu.onclick = function()
			{
				oMenu.closeAllNavigations();
				if (!sslCertUpload("STR_CONF_SSLCERTUPLOAD_ABORT"))
				{
					return false;
				}
				//oMenu.createBreadScrumb(this);
				oMenu.highLight(this);
			}
		}

		if(oNavigation.nav)
		{
			dMenu.id = oNavigation.nav;
			dMenu.onclick = this.expandNavigation;
		}

		dMenu.innerHTML = eLang.getString('navobj',oNavigation.label);

		this.menuBar.appendChild(dMenu);
		return dMenu;
	},

	expandNavigation	:	function(e)
	{
		if(!e) e = window.event;

		var tObj = events.getTargetObject(e);

		oMenu.closeAllNavigations();
		e.cancelBubble = true;
		if(e.stopPropagation) e.stopPropagation();

		var oNavigation = eval(tObj.id);

		if(oNavigation)
		{
			var oDOMMenu = oMenu.menu(tObj.id);
			oDOMMenu.style.left = tObj.offsetLeft + 'px';
			oDOMMenu.style.top  = (oMenu.menuBar.parentNode.offsetTop + oMenu.menuBar.parentNode.offsetHeight) + 'px';
			factory.animations._expand(oDOMMenu,document.body);
			//events.register("click",oMenu.closeAllNavigations,document);
			//document.onclick = oMenu.closeAllNavigations;
		}
	},

	menu				:	function(name)
	{
		var oDOMNav = null;
		var divMenuUp; //Created a division to display with style the Up Arrow.
		var divMenuDown; //Created a division to display with style the Down Arrow.
		var noItemsDisplay; //to get the no of Items that could be loaded within 
		// the height of the resized page.
		var start; //to hold the starting index of the menu items.
		var end; //to hold the ending index of the menu items.
		divMenuUp = document.createElement("div");
		divMenuDown = document.createElement("div");
		if(document.getElementById(name+'_menu')==null)
		{
			oDOMNav = document.createElement('div');
			oDOMNav.id = name+'_menu';
			oDOMNav.className = 'dropdownmenu';
			var mObj = eval(name);
			noItemsDisplay = (parseInt(document.body.offsetHeight / 30) - 2); 
			// To get the total no of elements that could be loading in the 
			// available space, by dividing the total height by each menu element
			// height(30). This calculation should always exclude the two
			// arrows so 2 is subtracted.
			if (noItemsDisplay < 3) {
				noItemsDisplay = 3; //Minimum no of Items loaded is 3.
			}
			if (noItemsDisplay < mObj.items.length) {
			//if the browser is resized and the menu to be displayed has 
			//less than total no of menu items then load the Up arrow.
				var imgUp ; //To load the Up Arrow Image.
				imgUp = document.createElement("img");
				imgUp.src = "../res/img/sortup.gif";
				divMenuUp.className = "menuNavArrow";
				divMenuUp.appendChild(imgUp);
				oDOMNav.appendChild(divMenuUp);
				divMenuUp.style.display = "none";
				//First time the up arrow should not be loaded
			}
			for (var i = 0; i < mObj.items.length; i++) {
				if (mObj.items[i].enabled == 1) {
					var mItem = document.createElement('a');
					mItem.innerHTML = eLang.getString('navobj',mObj.items[i].label);
					mItem.className = "submenu";
					if (mObj.items[i].page) {
						mItem.href = '..'+mObj.items[i].page;
						mItem.target = 'pageFrame';
						mItem.onclick = function()
						{
							oMenu.closeAllNavigations();
							if(!sslCertUpload("STR_CONF_SSLCERTUPLOAD_ABORT")) {
								return false;
							}
							//oMenu.createBreadScrumb(this);
							oMenu.highLight(this);
						}
					}
					if (mObj.items[i].func) {
						mItem.onclick = mObj.items[i].func;
					}
					oDOMNav.appendChild(mItem);
				}
			}
			if (noItemsDisplay < oDOMNav.childNodes.length) {
			// If the browser is resized and the menu to be displayed has 
			// less than total no of menu items,then load the Down arrow.
				var imgDown; //To load the Down Arrow Image.
				var index; //loop counter.
				imgDown = document.createElement("img");
				imgDown.src = "../res/img/sortdown.gif";
				divMenuDown.className = "menuNavArrow";
				divMenuDown.appendChild(imgDown);
				oDOMNav.appendChild(divMenuDown);
				for (index = noItemsDisplay + 1; 
					index < oDOMNav.childNodes.length - 1; index++)
				{	
				// This loop will hide the rest of the menu items which 
				//  cannot be fitted in the available browser height.
					oDOMNav.childNodes[index].style.display = "none";
				}
				start = 1;
				end = noItemsDisplay;
			}

			function moveDown() 
			{
				oDOMNav.childNodes[++end].style.display = "";
				oDOMNav.childNodes[start++].style.display = "none";
				arrowDisplay();
			}

			function moveUp() 
			{
				oDOMNav.childNodes[--start].style.display = "";
				oDOMNav.childNodes[end--].style.display = "none";
				arrowDisplay();
			}

			function arrowDisplay()
			{
				if (start == 1) {
					divMenuUp.style.display = "none";
				} else {
					divMenuUp.style.display = "";
				}
				if (end == (oDOMNav.childNodes.length - 2)) {
					divMenuDown.style.display = "none";
				} else {
					divMenuDown.style.display = "";
				}
			}
			divMenuUp.onclick = moveUp;
			divMenuDown.onclick = moveDown;
		}else
		{
			oDOMNav = document.getElementById(name+'_menu');
		}

		return oDOMNav;
	},

	highLight					:	function(anc)
	{
		var _tanc = anc.href.split('?')[0];
		var _tpg = _tanc.split('/page/');
		var pagePtr = '/page/'+_tpg[_tpg.length-1];

		oMenu.walkThroughMenuItems(oMenu.removeHighlight);

		if(anc.className=='helpBtn') return;

		for(var i=0; i<TOPNAV.items.length; i++)
		{
			if(pagePtr==TOPNAV.items[i].page)
			{
				TOPNAV.items[i].anc.className = 'hiLight';
				return;
			}
		}

		for(var i=0; i<TOPNAV.items.length; i++)
		{
			var SUBNAV = eval(TOPNAV.items[i].nav);
			if(SUBNAV==undefined) continue;

			for(var b=0; b<SUBNAV.items.length; b++)
			{
				if(pagePtr==SUBNAV.items[b].page)
				{
					TOPNAV.items[i].anc.className = 'hiLight';
					break;
				}
			}
		}
	},

	removeHighlight				:	function(anc)
	{
		if(anc.className!='helpBtn')
		{
			anc.className = 'normal';
		}
	},

	disableNavBar				:	function()
	{
		oMenu.walkThroughMenuItems(oMenu.disableItem);
	},

	disableItem					:	function(oItem)
	{
		oItem.href = "javascript://";
		oItem.style.outline="none";
		oItem.onclick = function(){};
		if(window.ActiveXObject)
		{
			oItem.style.filter="alpha(opacity=10)";
		}else
		{
			oItem.style.opacity=0.1;
		}
		oItem.style.cursor = "default";
	},

	createBreadScrumb			:	function(anc)
	{
		var bsStack = [];
		var breadScrumb = $('breadScrumb');

		var _tpg = anc.href.split('/page/');
		var pagePtr = '/page/'+_tpg[_tpg.length-1];
		for(var i=0; i<TOPNAV.items.length; i++)
		{
			if(pagePtr==TOPNAV.items[i].page)
			{
				bsStack.push(anc.innerHTML)
			}
		}
		if(bsStack.length==0)
		{
			for(var i=0; i<TOPNAV.items.length; i++) {
				var SUBNAV = eval(TOPNAV.items[i].nav);
				if(SUBNAV==undefined) continue;

				for(var b=0; b<SUBNAV.items.length; b++) {
					if (SUBNAV.items[b].enabled == 1) {
						if(pagePtr==SUBNAV.items[b].page) {
							bsStack.push(eLang.getString('navobj',SUBNAV.items[b].label));
							bsStack.push(eLang.getString('navobj',TOPNAV.items[i].label));
							break;
						}
					}
				}
			}
		}
		breadScrumb.innerHTML = "";

		for(i=bsStack.length-1;i>=0;i--)
		{
			breadScrumb.innerHTML += bsStack[i];

			if(i!=0) breadScrumb.innerHTML += "<font color='#AAAAAA'> &gt;</font> ";
		}

		oMenu.breadScrumbHolder.appendChild(breadScrumb);
	},

	closeNavigation		:	function(mItem)
	{
		if(document.getElementById(mItem.id+'_menu'))
			factory.animations._contract(document.getElementById(mItem.id+'_menu'),document.body);
	},

	closeAllNavigations	:	function()
	{
		oMenu.walkThroughMenuItems(oMenu.closeNavigation);
	},

	walkThroughMenuItems:	function(fnPtr)
	{
		var menuItem = oMenu.menuBar.firstChild;
		while(menuItem!=null)
		{
			fnPtr(menuItem);
			menuItem = menuItem.nextSibling;
		}
	}
};

menu={loaded:true};
