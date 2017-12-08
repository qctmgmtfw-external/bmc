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

var redirection_started = false;
var ipaddr;
var w_text;
var portno;
var session_token;
var m_SSL;
var web_token;
var retry_count = 1;
var timer;
var vmedia_ssl;

function doInit()
{
	// TODO: add page initialization code
	if (window.ActiveXObject)
		setTimeout("fillData();",2000);
}

function fillData()
{
	getSessionToken();
	web_token = readCookie('SessionCookie');

	ipaddr = document.location.href.split('/');
	if (ipaddr.length)
		ipaddr = ipaddr[2];
	w_text = "Video Redirection - "+ipaddr;
}

function readCookie(cookiename)
{
	var nameEQ = cookiename + "=";
	var ca = window.opener.document.cookie.split(';');
	for(var i=0;i < ca.length;i++)
	{
		var c = ca[i];
		while (c.charAt(0)==' ')
			c = c.substring(1,c.length);
		if (c.indexOf(nameEQ) == 0)
			return c.substring(nameEQ.length,c.length);
	}
	return null;
}

function getSessionTokenRes(arg)
{
	if (arg.HAPI_STATUS == 0)
	{
		session_token = WEBVAR_JSONVAR_GETSESSIONTOKEN.WEBVAR_STRUCTNAME_GETSESSIONTOKEN[0]['SESSION_TOKEN'];
		getAdviserCfg();
	}
	else
	{
		alert(eLang.getString("common","STR_ACTIVEX_GETTOKEN_FAILURE"));
		setTimeout("window.close();", 1000);
	}
}

function timedOut()
{
	alert(eLang.getString("common","STR_ACTIVEX_GETTOKEN_FAILURE"));
	setTimeout("window.close();", 1000);
}

function getSessionToken()
{
	xmit.get({url:'/rpc/getsessiontoken.asp', onrcv:getSessionTokenRes, status:'', ontimeout:timedOut});
}

function getRemoteSessionCfgRes(arg)
{
	if(arg.HAPI_STATUS != 0)
	{
		errstr = eLang.getString('common','STR_ACTIVEX_VMEDIA_GETVAL');
		errstr +=(eLang.getString('common','STR_IPMI_ERROR') + GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
		setTimeout("window.close();", 1000);
	}
	else
	{
		vmedia_ssl = WEBVAR_JSONVAR_GETREMOTESESSIONCFG.WEBVAR_STRUCTNAME_GETREMOTESESSIONCFG[0].MEDIAENCRYPTION;
		if(redirection_started)
			return;
		redirection_started = true;
		setTimeout("initRedirection();",2000);
		timer = setTimeout("window.close();", 10000);
	}
}

function getAdviserCfgRes(arg)
{
	if (arg.HAPI_STATUS == 0)
	{
		ADVISER_CFG = WEBVAR_JSONVAR_GETADVISERCFG.WEBVAR_STRUCTNAME_GETADVISERCFG[0];
		m_SSL = ADVISER_CFG.V_STR_SECURE_CHANNEL;
		if (ADVISER_CFG.V_STR_SECURE_CHANNEL)
			portno = ADVISER_CFG.V_STR_SECURE_PORT;
		else
			portno = ADVISER_CFG.V_STR_PORT;

		xmit.get ({url:"/rpc/getremotesession.asp",onrcv:getRemoteSessionCfgRes, status:''});
	}
	else
	{
		errstr = eLang.getString('common','STR_ACTIVEX_ADVISER_GETVAL')
		errstr += (eLang.getString('common','STR_IPMI_ERROR') + GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
		setTimeout("window.close();", 1000);
	}
}

function getAdviserCfg()
{
	xmit.get({url:'/rpc/getadvisercfg.asp', onrcv:getAdviserCfgRes, status:'', ontimeout:timedOut});
}

function initRedirection()
{
	try
	{
		//alert ("ipaddr::"+ipaddr+"\nw_text::"+w_text+"\nsession_token::"+session_token+"\nportno::"+portno+"\nm_SSL::"+m_SSL+"\nVmedia ::"+ vmedia_ssl + "\nweb_token::"+web_token);
		document.getElementById('AVCVIEW').InitiateRedirection(ipaddr, w_text, session_token, portno, m_SSL, vmedia_ssl, web_token);
		//AVCVIEW.InitiateRedirection(ipaddr, w_text, session_token,portno,m_SSL,web_token);
	}
	catch(e)
	{
		if (retry_count < 3)
		{
			retry_count++;
			setTimeout("initRedirection();",2000);
			if (timer)
			{
				clearTimeout (timer);
				timer = setTimeout("window.close();", 10000);
			}
		}
		else
		{
			alert (eLang.getString('common','STR_ACTIVEX_FAILURE'));
			setTimeout("window.close();", 1000);
			if (timer)
				clearTimeout (timer);
		}
	}
}
