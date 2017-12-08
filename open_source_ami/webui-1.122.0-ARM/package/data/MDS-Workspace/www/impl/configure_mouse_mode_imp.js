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

var mouseMode;
var gmousemode = -1;

function doInit()
{
	exposeElms(['_mInfo',
				'_absMod',
				'_relMod',
				'_applyBtn',
				'_resetBtn']);

	if (top.user.isAdmin())
	{
		absMod.onclick = onAbsolute;
		relMod.onclick = onRelative;
		applyBtn.onclick = setMouseMode;
		resetBtn.onclick = reloadMouseMode;
	}
	else
		disableActions();

	_begin();
}

function _begin()
{
	getMouseMode();
}

function onAbsolute()
{
	gmousemode = 2;
	absMod.checked = true;
	relMod.checked = false;
}

function onRelative()
{
	gmousemode = 1;
	absMod.checked = false;
	relMod.checked = true;
}

function getMouseMode()
{
	xmit.get({url:"/rpc/getmousemode.asp",onrcv:getMouseModeRes, status:''});
}

function getMouseModeRes(arg)
{
	if(arg.HAPI_STATUS)
	{
		errstr = eLang.getString('common','STR_CONF_MOUSE_GETVAL');
		errstr +=(eLang.getString('common','STR_IPMI_ERROR') + GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
		return;
	}
	MOUSE_MODE = WEBVAR_JSONVAR_GETMOUSEMODE.WEBVAR_STRUCTNAME_GETMOUSEMODE;
	reloadMouseMode();
}

function activateButtons()
{
	if (top.user.isAdmin())
	{
		absMod.disabled = false;
		relMod.disabled = false;
	}
	absMod.checked = false;
	relMod.checked = false;
}

function reloadMouseMode()
{
	activateButtons();
	if(MOUSE_MODE.length==0)
	{
		alert(eLang.getString('common','STR_CONF_MOUSE_UNKNOWN'));
		return;
	}

	mouseMode = MOUSE_MODE[0].GET_MOUSE_MODE;

	if (mouseMode == 2)
	{
		mInfo.innerHTML = eLang.getString('common','STR_CONF_MOUSE_ABSOLUTE');
		absMod.disabled = true;
	}
	else if ( mouseMode == 1 )
	{
		mInfo.innerHTML = eLang.getString('common','STR_CONF_MOUSE_RELATIVE');
		relMod.disabled = true;
	}
	else
	{
		alert(eLang.getString('common','STR_CONF_MOUSE_UNKNOWN'));
	}
}

function setMouseMode()
{
	if(top.user.isAdmin())
	{
		if(!absMod.checked && !relMod.checked)
		{
			alert(eLang.getString('common','STR_CONF_MOUSE_ALERT1'));
			return;
		}

		if(gmousemode==mouseMode)
		{
			alert(eLang.getString('common','STR_CONF_MOUSE_ALERT2'));
			return;
		}

		if(confirm(eLang.getString('common','STR_CONF_MOUSE_CONFIRM')))
		{
			var req = new xmit.getset({url:"/rpc/setmousemode.asp",onrcv:setMouseModeRes});
			req.add("SET_MOUSE_MODE",gmousemode);
			req.send();
			delete req;
		}
		else
			getMouseMode();
	}
	else
	{
		alert(eLang.getString('common','STR_CONF_ADMIN_PRIV'));
	}
}

function setMouseModeRes(arg)
{
	if(arg.HAPI_STATUS != 0)
		alert(eLang.getString('common','STR_CONF_MOUSE_SETVAL'));
	else
		getMouseMode();
}
