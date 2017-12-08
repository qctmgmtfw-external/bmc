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
var action;
var current_state;
var state_to_expect = 0xf; //f is dont care
var MaxRetries = 3;
/*var UseExternalBMC = 0;
var msgCableChkBMC = eLang.getString('common','STR_SERVER_EXTERNAL_BMC')
var msgCableChkFeature = eLang.getString('common','STR_SERVER_FEATURE_CABLE')
*/
function doInit()
{
	exposeElms(['_statusMsg',
			'_resetSrvr',
			'_iPwrOffSrvr',
			'_oPwrOffSrvr',
			'_pwrOnSrvr',
			'_pwrCycleSrvr',
			'_prfmAction']);

	if(!top.user.isAdmin())
		disableActions();

	_begin();
}

function _begin()
{
	getHostStatus();

	if(top.user.isAdmin())
	{
		resetSrvr.onclick = function ()
		{
			action = 3;
			prfmAction.disabled = false;
		};
		iPwrOffSrvr.onclick = function ()
		{
			action = 0;
			prfmAction.disabled = false;
		};
		oPwrOffSrvr.onclick = function ()
		{
			action = 5;
			prfmAction.disabled = false;
		};
		pwrOnSrvr.onclick = function ()
		{
			action = 1;
			prfmAction.disabled = false;
		};
		pwrCycleSrvr.onclick = function ()
		{
			action = 2;
			prfmAction.disabled = false;
		};
		prfmAction.onclick = doPowerAction;
	}
}

function getHostStatusRes(arg)
{
	if(arg.HAPI_STATUS)
	{
		errstr = eLang.getString('common','STR_SERVER_GETSTATUS')
		errstr += (eLang.getString('common','STR_IPMI_ERROR')+ GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	}
	else
	{
		var res = WEBVAR_JSONVAR_HL_SYSTEM_STATE.WEBVAR_STRUCTNAME_HL_SYSTEM_STATE;

		if(res.length)
			current_state = res[0].JF_STATE;
		else
		{
			alert(eLang.getString('common',"STR_SERVER_STATE_NOTAVAIL"));
			prfmAction.disabled = true;
			return;
		}

		if(state_to_expect != 0xf)
		{
			if(current_state != state_to_expect)
			{
				MaxRetries--;
				showWait(true,eLang.getString('common','STR_SERVER_RETRYING') + MaxRetries);
				if(MaxRetries == 0)
				{
					showWait(false);
  					if(action == 5)
						alert(eLang.getString('common','STR_SERVER_ERR1'));
					else
						alert(eLang.getString('common','STR_SERVER_ERR2'));

					return;
				}
				if(action == 5)
					setTimeout("getHostStatus()",10000);
				else
					setTimeout("getHostStatus()",5000);

				return;
			}
		}
		if(WEBVAR_JSONVAR_HL_SYSTEM_STATE.WEBVAR_STRUCTNAME_HL_SYSTEM_STATE[0].JF_STATE==0)
		{
			statusMsg.innerHTML = eLang.getString('common','STR_SERVER_HOSTOFF');
			statusMsg.style.color = '#990000';

			pwrOnSrvr.checked = true;
			if(top.user.isAdmin())
			{
				pwrOnSrvr.disabled = false;
				resetSrvr.disabled = true;
				iPwrOffSrvr.disabled = true;
				oPwrOffSrvr.disabled = true;
				pwrCycleSrvr.disabled = true;
			}
			action = 1;
			MaxRetries = 3;
		}
		else
		{
			statusMsg.innerHTML = eLang.getString('common','STR_SERVER_HOSTON');
			statusMsg.style.color = '#009900';

			resetSrvr.checked = true;
			if(top.user.isAdmin())
			{
				resetSrvr.disabled = false;
				iPwrOffSrvr.disabled = false;
				oPwrOffSrvr.disabled = false;
				pwrCycleSrvr.disabled = false;
				pwrOnSrvr.disabled = true;
			}
			action = 3;
			MaxRetries = 3;
		}
	}
}

function getHostStatus()
{
	xmit.get({url:"/rpc/hoststatus.asp",onrcv:getHostStatusRes, status:''});
}

function powerActionRes(arg)
{
	if(arg.HAPI_STATUS)
	{
		errstr = eLang.getString('common','STR_SERVER_SETACTION')
		errstr += (eLang.getString('common','STR_IPMI_ERROR')+ GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	}
	else
	{
		showWait(true,eLang.getString('common','STR_SERVER_POWER_ACTION_WAIT'));
		if(action == 2)
			inittmout = 20000; //for power cycle we check if host is powered on again after 10 secs
		else if(action == 5)
			inittmout = 30000;
		else
			inittmout = 10000;
		MaxRetries = 3;
		setTimeout("getHostStatus()",inittmout);
	}
}

function doPowerAction()
{
	if (top.user.isAdmin())
	{
		switch(action)
		{
			case 0:
				state_to_expect = 0;
			break;
			case 1:
				state_to_expect = 1;
			break;
			case 2:
				state_to_expect = 1; //power on after some time I suppose
			break;
			case 3:
				state_to_expect = 1;
			break;
			case 5:
				state_to_expect = 0;
			break;
		}

		var req = new xmit.getset({url:"/rpc/hostctl.asp",onrcv:powerActionRes});
		req.add("WEBVAR_POWER_CMD",action);
		req.send();
		delete req;
	}
	else
		alert(eLang.getString('common','STR_CONF_ADMIN_PRIV'));
}
