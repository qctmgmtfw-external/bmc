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

function doInit()
{
	exposeElms(["_enableRADIUS",
		"_port",
		"_timeout",
		"_ipaddr",
		"_secret",
		"_saveBtn",
		"_resetBtn"]);

	if(top.user.isAdmin())
	{
		enableRADIUS.onclick = doRADIUScfg;
		saveBtn.onclick = isRADIUSCfgChange;
		resetBtn.onclick = reloadRADIUSCfg;
	}
	else
		disableActions();

	_begin();
}

function _begin()
{
	xmit.get({url:"/rpc/getradiuscfg.asp",onrcv:getRADIUSCfgRes, status:""});
}

function getRADIUSCfgRes (arg)
{
	if (arg.HAPI_STATUS != 0)
	{
		errstr = eLang.getString("common","STR_CONF_RADIUS_GETVAL");
		errstr +=(eLang.getString("common","STR_IPMI_ERROR") + GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
		return;
	}
	else
	{
		RADIUSCFG_DATA = WEBVAR_JSONVAR_HL_GETRADIUSCFG.WEBVAR_STRUCTNAME_HL_GETRADIUSCFG[0];
		reloadRADIUSCfg();
	}
}

function fillRADIUSCfg()
{
	port.value = RADIUSCFG_DATA.PORTNUM;
	timeout.value = RADIUSCFG_DATA.TIMEOUT;
	ipaddr.value = RADIUSCFG_DATA.IP;
	secret.value = "";
}

/*
 * It will load response data from global variable to respective controls in UI.
 */
function reloadRADIUSCfg()
{
	enableRADIUS.checked = (RADIUSCFG_DATA.ENABLE) ? true : false;
	doRADIUScfg();
	if (enableRADIUS.checked) {
		fillRADIUSCfg();
	}
}

/*
 * This function will compare the configuration values with the data in 
 * the controls.
 */
function isRADIUSCfgChange()
{
	if ((RADIUSCFG_DATA.ENABLE == enableRADIUS.checked) && 
		(RADIUSCFG_DATA.PORTNUM == port.value) && 
		(RADIUSCFG_DATA.TIMEOUT == timeout.value) && 
		(RADIUSCFG_DATA.IP == ipaddr.value) && 
		("" == secret.value)) {
		return;
	}
	validateRADIUSCfg()
}

/*
 * This will validate all the UI controls value of RADIUS configuration before 
 * saving it.
 */
function validateRADIUSCfg()
{
	if (enableRADIUS.checked) {
		if (!eVal.port(port.value)) {
			alert(eLang.getString("common", "STR_INVALID_PORT"));
			port.focus();
			return;
		}

		if (!eVal.isnumstr(timeout.value, 3, 300)) {
			alert(eLang.getString("common", "STR_INVALID_TIMEOUT") + 
				eLang.getString("common", "STR_HELP_INFO"));
			timeout.focus();
			return;
		}

		if (!eVal.ip(ipaddr.value)) {
			alert(eLang.getString("common", "STR_INVALID_IP") + 
				eLang.getString("common", "STR_HELP_INFO"));
			ipaddr.focus();
			return;
		}

		if (!eVal.password(secret.value, 4, 31)) {
			alert(eLang.getString("err", 0x03) + 
				eLang.getString("common", "STR_HELP_INFO"));
			secret.focus();
			return;
		}
	}
	setRADIUSCfg();
}

/*
 * It will invoke the RPC method to set the RADIUS configuration.
 * Once it get response from RPC, on receive method will be called 
 * automatically.
 */
function setRADIUSCfg()
{
	if (top.user.isAdmin()) {
		var req = new xmit.getset({url:"/rpc/setradiuscfg.asp", 
			onrcv:setRADIUSCfgRes});
		req.add("ENABLE", enableRADIUS.checked?1:0);
		req.add("PORTNUM", port.value);
		req.add("TIMEOUT", timeout.value);
		req.add("IP", ipaddr.value);
		req.add("SECRET", secret.value);
		req.send();
		delete req;
	} else {
		alert(eLang.getString("common", "STR_CONF_ADMIN_PRIV"));
	}
}

function setRADIUSCfgRes (arg)
{
	if(arg.HAPI_STATUS != 0)
	{
		errstr =  eLang.getString("common","STR_CONF_RADIUS_SETVAL");
		errstr += (eLang.getString("common","STR_IPMI_ERROR") + GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	}
	else
	{
		alert(eLang.getString("common","STR_CONF_RADIUS_SAVE_SUCCESS"));
		_begin();
	}
}

function doRADIUScfg()
{
	if (top.user.isAdmin())
	{
		bopt = !enableRADIUS.checked;
		port.disabled = bopt;
		timeout.disabled = bopt;
		ipaddr.disabled = bopt;
		secret.disabled = bopt;
		if (!bopt)
			port.focus();
		else
			fillRADIUSCfg();
	}
}
