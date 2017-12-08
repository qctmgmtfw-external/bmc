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
	exposeElms(['_enableSystemLog',
				'_localLog',
				'_remoteLog',
				'_fileSize',
				'_rotateCount',
				'_serverAddr',
				'_enableAuditLog',
				'_saveBtn',
				'_resetBtn']);

	if(top.user.isAdmin())
	{
		enableSystemLog.onclick = enableSysLog;
		localLog.onclick = enableLog;
		remoteLog.onclick = enableLog;
		saveBtn.onclick = setLogCfg;
		resetBtn.onclick = reloadLogCfg;
	}
	else
		disableActions();

	_begin();
}

function _begin()
{
	optind = 0;
	getLogCfg();
}

function getLogCfg()
{
	xmit.get({url:'/rpc/getlogcfg.asp',onrcv:getLogCfgRes,status:''});
}

function getLogCfgRes(arg)
{
	if(arg.HAPI_STATUS)
	{
		errstr = eLang.getString('common','STR_CONF_SYS_AUDIT_GETVAL');
		errstr +=(eLang.getString('common','STR_IPMI_ERROR') + GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	}
	else
	{
		SYSLOG_DATA = WEBVAR_JSONVAR_HL_GETLOGCFG.WEBVAR_STRUCTNAME_HL_GETLOGCFG[0];
		reloadLogCfg();
	}
}

function setLogCfg()
{
	if (top.user.isAdmin())
	{
		if(enableSystemLog.checked)
		{
			if(localLog.checked)
			{
				if(!eVal.isnumstr(fileSize.value, 3, 65535))
				{
					alert(eLang.getString('common','STR_CONF_SYS_AUDIT_INVALID_FILESIZE') + eLang.getString('common','STR_HELP_INFO'));
					fileSize.focus();
					return;
				}
				if(!eVal.isnumstr(rotateCount.value, 0, 255))
				{
					alert(eLang.getString('common','STR_CONF_SYS_AUDIT_INVALID_ROTATECNT') + eLang.getString('common','STR_HELP_INFO'));
					rotateCount.focus();
					return;
				}
			}
			else if(remoteLog.checked)
			{
				if(!eVal.ip(serverAddr.value, 0, 255) && !eVal.domainname(serverAddr.value,1))
				{
					alert(eLang.getString('common','STR_INVALID_SERVERADDR') + eLang.getString('common','STR_HELP_INFO'));
					serverAddr.focus();
					return;
				}
			}
			else
			{
				alert (eLang.getString('common','STR_CONF_SYS_AUDIT_INVALID_LOGTYPE'));
				localLog.focus();
				return;
			}
		}
		var req = new xmit.getset({url:'/rpc/setlogcfg.asp',onrcv:setLogCfgRes,status:''});
		req.add("AUDITENABLE", enableAuditLog.checked?1:0);
		req.add("SYSLOGENABLE", (enableSystemLog.checked ? (localLog.checked ? 1 : (remoteLog.checked ? 2 : 0)) : 0));
		req.add("FILESIZE", parseInt(fileSize.value,10));
		req.add("ROTATECNT", parseInt(rotateCount.value,10));
		req.add("SERVERADDR", serverAddr.value);
		req.send();
		delete req;
	}
	else
		alert(eLang.getString('common','STR_CONF_ADMIN_PRIV'));
}

function setLogCfgRes(arg)
{
	if(arg.HAPI_STATUS)
	{
		errstr = eLang.getString('common','STR_CONF_SYS_AUDIT_SETVAL');
		errstr +=(eLang.getString('common','STR_IPMI_ERROR') + GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	}
	else
	{
		alert (eLang.getString("common","STR_CONF_SYS_AUDIT_SAVE_SUCCESS"));
		getLogCfg();
	}
}

function reloadLogCfg()
{
	enableAuditLog.checked = (SYSLOG_DATA.AUDITENABLE)?true:false;
	enableSystemLog.checked = (SYSLOG_DATA.SYSLOGENABLE)?true:false;
	enableSysLog();
	fillLogCfg();
}

function fillLogCfg()
{
	localLog.checked = (SYSLOG_DATA.SYSLOGENABLE == 1) ? true : false;
	remoteLog.checked = (SYSLOG_DATA.SYSLOGENABLE == 2) ? true : false;
	enableLog();
	fileSize.value = SYSLOG_DATA.FILESIZE;
	rotateCount.value = SYSLOG_DATA.ROTATECNT;
	serverAddr.value = SYSLOG_DATA.SERVERADDR;
}

function enableSysLog()
{
	if(enableSystemLog.checked)
	{
		localLog.disabled = false;
		remoteLog.disabled = false;
		enableLog();
	}
	else
	{
		localLog.disabled = true;
		remoteLog.disabled = true;
		fileSize.disabled = true;
		rotateCount.disabled = true;
		serverAddr.disabled = true;
	}
}

function enableLog()
{
	fileSize.disabled = !localLog.checked;
	rotateCount.disabled = !localLog.checked;
	if(localLog.checked)
		fileSize.focus();

	serverAddr.disabled = !remoteLog.checked;
	if(remoteLog.checked)
		serverAddr.focus();
}