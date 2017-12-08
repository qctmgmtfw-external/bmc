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
File 		- widget.js
Module 		- IWISP Library
Author		- chandrasekarr@amiindia.co.in

*/

var widgets = [];

function widget(id, widget_width)
{
	if(widget_width==undefined)	widget_width = 220;

	//constrained by screensize for short time
	var max_widgets = Math.floor(document.body.clientWidth/widget_width)
	if(widgets.nullLessLength()>=max_widgets)
	{
		alert("Max widget limit reached. Please close any widget to continue");
		return false;
	}

	if (widgets[id] != undefined && widgets[id] != null)
		return document.getElementById('widget_'+id);

	this.container = document.createElement('iframe');
	this.container.frameBorder = '0';
	this.container.id = 'widget_'+id;
	this.container.className = 'widget';

	this.container.src = '../page/widget_page.html?id='+id;

	document.body.appendChild(this.container);

	this.autoPosition();

	widgets[id] = this;

	return this;
}

widget.prototype.isWidget = true;

widget.prototype.autoPosition = function()
{
	this.autoPositionLeft();
	this.autoPositionTop();
}

widget.prototype.autoPositionTop = function()
{
	this.container.style.top = (document.body.clientHeight - this.container.offsetHeight)+'px';
}

widget.prototype.autoPositionLeft = function()
{
	this.container.style.left = (widgets.nullLessLength() * (this.container.offsetWidth+10))+'px';
}

widget.prototype.close = function()
{
	//force unload events;
	this.container.src = '../page/blank.html';
	document.body.removeChild(this.container);
	delete this.container;

	for(var j=0; j<widgets.length; j++)
	{
		if (widgets[j] != undefined && widgets[j] != null)
		{
			if(widgets[j]==this)
			{
				widgets[j] = null;
				break;
			}
		}
	}

	try{ pageFrame.widgetListener(j); }catch(e){}

	reposition_all_widgets();
}

closeAllWidgets = function()
{
	for(var i=0; i<widgets.length; i++)
	{
		if (widgets[i] != undefined && widgets[i] != null)
			widgets[i].close();
	}
}

function reposition_all_widgets()
{
	var count = 0;
	for(var j=0; j<widgets.length; j++)
	{
		try{
			if (widgets[j] == undefined || 
				widgets[j] == null) continue;

			widgets[j].container.style.left = ((count++) * (widgets[j].container.offsetWidth+10))+'px';
			widgets[j].container.style.top = (document.body.clientHeight - widgets[j].container.offsetHeight)+'px';
		}catch(e)
		{
			delete widgets[j];
		}
	}
}