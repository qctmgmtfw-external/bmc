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

var cell = 0;
function doInit()
{
	exposeElms(["_remoteControl"]);
	_begin();
}

function _begin()
{
	getConsoles(javaAvailable, activeXAvailable);
}

function javaAvailable(bool)
{
	if(bool) {
		tcell = remoteControl.insertCell(cell++);
		tcell.innerHTML = "<input type='button' id='_btnJava' value='" + 
			eLang.getString("common", "STR_CONSOLE_JAVA") + 
			"' onclick='doLaunchJava()'/>";
		if(!top.user.isAdmin()) {
			disableActions();
		}
	}
}

function activeXAvailable(bool)
{
	if(bool) {
		tcell = remoteControl.insertCell(cell++);
		tcell.innerHTML = "<input type='button' id='_btnActiveX' value='" + 
			eLang.getString("common", "STR_CONSOLE_ACTIVEX") + 
			"' onclick='doLaunchActiveX()'/>";
		reloadHelp();
		if(!top.user.isAdmin()) {
			disableActions();
		}
	}
	if (cell == 0) {
		tcell = remoteControl.insertCell(cell);
		tcell.innerHTML = "<strong>" + eLang.getString("common", 
			"STR_NOT_SUPPORT") + "</strong>";
	} else {
		if(top.user.isAdmin()) {
			xmit.get({url:"/rpc/getadvisercfg.asp", onrcv:getAdviserCfgRes, status:""});
		}
	}
}

function getAdviserCfgRes(arg)
{
	var KVM_CFG;
	if (arg.HAPI_STATUS) {
		errstr = eLang.getString("common", "STR_CONF_RMT_SESS_GETVAL");
		errstr += (eLang.getString("common", "STR_IPMI_ERROR") + 
			GET_ERROR_CODE(arg.HAPI_STATUS));
		alert (errstr);
	} else {
		KVM_CFG = WEBVAR_JSONVAR_GETADVISERCFG.WEBVAR_STRUCTNAME_GETADVISERCFG[0];
		try {
			$("_btnJava").disabled = KVM_CFG.V_STR_KVM_STATUS ? false : true;
		} catch(e) {
		}
		try {
			$("_btnActiveX").disabled = KVM_CFG.V_STR_KVM_STATUS ? false : true;
		} catch(e) {
		}
	}
}
// rest in common_function.js
