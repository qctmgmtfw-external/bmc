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

// File Name  : configure_network_imp.js
// Brief      : This implementation is to display and configure the network
// configuration in the BMC.
// Author Name: Arockia Selva Rani. A

var LANCFG_DATA;			//It holds the get RPC LAN response data

/*
 * This function will be called when its corresponding page gets loaded.
 * It will expose all the user controls and checks for user privilege.
 * Finally it will invoke the begin method. 
 */
function doInit()
{
	exposeElms([
		"_lstLANInterface",
		"_chkLANEnable",
		"_txtMACAddress",
		"_chkV4IPSource",
		"_txtV4IPAddress",
		"_txtV4Subnet",
		"_txtV4Gateway",
		"_chkV6Enable",
		"_chkV6IPSource",
		"_txtV6IPAddress",
		"_txtV6Subnet",
		"_txtV6Gateway",
		"_chkVLANEnable",
		"_txtVLANId",
		"_txtVLANPriority",
		"_btnSave",
		"_btnReset"]);

	if(top.user.isAdmin()) {
		chkV4IPSource.onclick = doV4DHCP;
		chkV6Enable.onclick = enableV6;
		chkV6IPSource.onclick = enableV6;
		chkVLANEnable.onclick = enableVLAN;
		btnSave.onclick = validateNetworkInfo;
		btnReset.onclick = reloadLANInfo;
	} else if(top.user.isOperator()) {
		disableActions({id: ["_lstLANInterface"]});
	} else {
		alert(eLang.getString("common","STR_PERMISSION_DENIED"));
		location.href = "dashboard.html";
		return;
	}
	_begin();
}

/*
 * It will fill data for user controls like list box, if any.
 * It will invoke the RPC method to get the data for the page.
 */
function _begin()
{
	fillLanChannel();
	getAllLANCfg();
}

/*
 * It will invoke the RPC method to get the network bonding configuration.
 * Once it get response from RPC, on receive method will be called automatically.
 */
function getNWBondCfg()
{
	xmit.get({url:"/rpc/getnwbondcfg.asp", onrcv:getNWBondCfgRes, status:""});
}

/*
 * This is the response function for getNWBondCfg RPC. 
 * Need to check HAPI_STATUS, intimate end user if it returns non-zero value.
 * If success, move the response data to the global variable and invoke the 
 * method to load the data value in UI. 
 * @param arg object, RPC response data from xmit library
 */
function getNWBondCfgRes(arg)
{
	var errstr;		//Error string
	if(arg.HAPI_STATUS != 0) {
		errstr = eLang.getString("common", "STR_CONF_NW_BOND_GETVAL");
		errstr += (eLang.getString("common", "STR_IPMI_ERROR") + 
			GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
		
	} else {
		NWBONDCFG_DATA = WEBVAR_JSONVAR_GETNWBONDCFG.WEBVAR_STRUCTNAME_GETNWBONDCFG[0];
		if(NWBONDCFG_DATA.BOND_ENABLE) 
			disableV6();
	}
}

/*
 * It will disable the v6 user controls.
 */
function disableV6()
{
	chkV6Enable.disabled = true;
	chkV6IPSource.disabled = true;
}

/*
 * It will fill LAN Interface list box with eth Interface string and LAN
 * Channel number.
 */
function fillLanChannel()
{
	for (i = 0; i < top.settings.lan.length; i++) {
		lstLANInterface.add(new Option(top.settings.ethstr[i], 
			top.settings.lan[i]), isIE?i:null);
	}
	if (top.mainFrame.pageFrame.location.hash) {
		lstLANInterface.value = comboParser(top.mainFrame.pageFrame.location.hash, 
			lstLANInterface.id);	//id->id of the selection box
	}

	if (fnCookie.read('gMultiLAN'))
	{
		chkLANEnable.disabled = false;
		chkLANEnable.onclick = enableLAN;
		lstLANInterface.onchange = reloadLANInfo;
	} else {
		chkLANEnable.disabled = true;
		chkLANEnable.onclick = function (){};
	}
}

/*
 * It will invoke the RPC method to get all LAN configuration.
 * Once it get response from RPC, on receive method will be called automatically.
 */
function getAllLANCfg()
{
	xmit.get({url:"/rpc/getalllancfg.asp", onrcv:getAllLANCfgRes, status:""});
}

/*
 * This is the response function for getAllLANCfg RPC. 
 * Need to check HAPI_STATUS, intimate end user if it returns non-zero value.
 * If success, move the response data to the global variable and invoke the 
 * method to load the data value in UI. 
 * @param arg object, RPC response data from xmit library
 */
function getAllLANCfgRes(arg)
{
	var errstr;		//Error string
	switch(arg.HAPI_STATUS) {
	case 0x0:
		LANCFG_DATA = WEBVAR_JSONVAR_GETALLNETWORKCFG.WEBVAR_STRUCTNAME_GETALLNETWORKCFG;
		reloadLANInfo();
	break;

	case 0x1D4:
		alert (eLang.getString("common", "STR_PERMISSION_DENIED"));
		location.href = "dashboard.html";
	break;

	case 0xE2:	case 0xE3:
		errstr = eLang.getString("common", "STR_CONF_NW_GETVAL");
		errstr += (eLang.getString("common", "STR_CONF_NW_ERR_" +
			arg.HAPI_STATUS));
		alert(errstr);
	break;
	
	default:
		errstr = eLang.getString("common", "STR_CONF_NW_GETVAL");
		errstr += (eLang.getString("common", "STR_IPMI_ERROR") +
			GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	}
}

/*
 * It will load response data from global variable to respective controls in UI.
 */
function reloadLANInfo()
{
	var index = 0;		//index of the global RPC response data
	index = getIndex(lstLANInterface.value);
	chkLANEnable.checked = (LANCFG_DATA[index].lanEnable == 1) ? true : false;

	txtMACAddress.value = LANCFG_DATA[index].macAddress;
	//Ignores backspace functionality
	txtMACAddress.onkeydown = checkBackspace;
	if (getbits (LANCFG_DATA[index].v4IPSource,3,0) == 
		top.CONSTANTS.IPSOURCE_DHCP) {
		chkV4IPSource.checked = true;
	} else if(getbits (LANCFG_DATA[index].v4IPSource,3,0) == 
		top.CONSTANTS.IPSOURCE_STATIC) {
		chkV4IPSource.checked = false;
	}
	loadV4Info(index);
	//doV4DHCP();

	chkV6Enable.checked = LANCFG_DATA[index].v6Enable ? true : false;
	if (getbits (LANCFG_DATA[index].v6IPSource,3,0) == 
		top.CONSTANTS.IPSOURCE_DHCP) {
		chkV6IPSource.checked = true;
	} else if(getbits (LANCFG_DATA[index].v6IPSource,3,0) == 
		top.CONSTANTS.IPSOURCE_STATIC) {
		chkV6IPSource.checked = false;
	}
	loadV6Info(index);
	//enableV6();

	chkVLANEnable.checked = (LANCFG_DATA[index].vlanEnable == 1) ? true : false;
	txtVLANId.value = LANCFG_DATA[index].vlanID;
	txtVLANPriority.value = LANCFG_DATA[index].vlanPriority;

	enableLAN();
}

/*
 * It will return the index value of the LAN channel from the global RPC LAN
 * configuration data. 
 * @param channel number, one of the value in global RPC response data.
 * @return number, index position of the global RPC data.
 */
function getIndex(channel)
{
	var i; 				//loop counter
	var index = -1;		//position of the global RPC data
	for (i = 0; i < LANCFG_DATA.length; i++) {
		if (LANCFG_DATA[i].channelNum == channel) {
			index = i;
			break;
		}
	}
	if (index == -1) {
		alert ("Error in getting the index for Channel::" + channel);
		index = 0;
	}
	return index;
}

/*
 * It will load v4 configuration response data from global variable to 
 * respective controls in UI.
 */
function loadV4Info(index)
{
	txtV4IPAddress.value = LANCFG_DATA[index].v4IPAddr;
	txtV4Subnet.value = LANCFG_DATA[index].v4Subnet;
	txtV4Gateway.value = LANCFG_DATA[index].v4Gateway;
}

/*
 * It will load v6 configuration response data from global variable to 
 * respective controls in UI.
 */
function loadV6Info(index)
{
	txtV6IPAddress.value = LANCFG_DATA[index].v6IPAddr;
	txtV6Subnet.value = LANCFG_DATA[index].v6Prefix;
	txtV6Gateway.value = LANCFG_DATA[index].v6Gateway;
}

/*
 * It will validate the data of all user controls before saving it.
 * IPv4 Address, Netmask & Gateway - IPv4 Address format
 * IPv4 Address, v6 Gateway - IPv6 Address format
 * v6 Subnet - number ranges from 0 to 128
 * VLAN ID - number ranges from 1 to 15
 * VLAN Priority - number ranges from 1 to 7
 */
function validateNetworkInfo()
{
	if (chkLANEnable.checked) {
		if(!chkV4IPSource.checked) {
			if(!eVal.ip(txtV4IPAddress.value)) {
				alert (eLang.getString("common", "STR_CONF_NW_ERR_223"));
				txtV4IPAddress.focus();
				return;
			}
			if (!eVal.netMask(txtV4Subnet.value)) {
				alert (eLang.getString("common", "STR_CONF_NW_ERR_225"));
				txtV4Subnet.focus();
				return;
			}
			var ipv = txtV4Gateway.value.split(".");
			if (!((ipv.length == 4) && (ipv[0]==0) && (ipv[1]==0) && (ipv[2]==0)
				&& (ipv[3]==0) && (ipv[0]==" ") && (ipv[1]==" ") 
				&& (ipv[2]==" ") && (ipv[3]==" "))) {
				if (!eVal.ip(txtV4Gateway.value,true)) {
					alert (eLang.getString("common", "STR_CONF_NW_ERR_224"));
					txtV4Gateway.focus();
					return;
				}
			}
		}

		if(chkV6Enable.checked) {
			if (!chkV6IPSource.checked) {			//V6 DHCP
				if(!eVal.ipv6(txtV6IPAddress.value, false, false)) {
					alert (eLang.getString("common", "STR_CONF_NW_ERR_226"));
					txtV6IPAddress.focus();
					return;
				}
				if(!eVal.isnumstr(txtV6Subnet.value, 0, 128)) {
					alert (eLang.getString("common", 
						"STR_CONF_NW_INVALID_V6SUBNET"));
					txtV6Subnet.focus();
					return;
				}
				if(!eVal.ipv6(txtV6Gateway.value, false, true)) {
					alert (eLang.getString("common", "STR_CONF_NW_ERR_227"));
					txtV6Gateway.focus();
					return;
				}
			}
		}

		if(chkVLANEnable.checked) {
			if(!eVal.isnumstr(txtVLANId.value, 1, 4095)) {
				alert (eLang.getString("common", 
					"STR_CONF_NW_INVALID_VLANID"));
				txtVLANId.focus();
				return;
			}
			if(!eVal.isnumstr(txtVLANPriority.value, 1, 7)) {
				alert (eLang.getString("common", 
					"STR_CONF_NW_INVALID_VLANPRIORITY"));
				txtVLANPriority.focus();
				return;
			}
		}
	}
	setNetworkInfo();
}

/*
 * It will invoke the RPC method to set the network configuration.
 * Once it get response from RPC, on receive method will be called automatically.
 */
function setNetworkInfo()
{
	if(confirm(eLang.getString("common", "STR_CONF_NW_SAVE_CONFIRM") + 
			eLang.getString("common", "STR_CONF_NW_WARNING"))) {
		/* Disable the SAVE button here till the request is served */
		btnSave.disabled = true;
		var req = new xmit.getset({url:"/rpc/setnwconfig.asp", 
			onrcv:setNetworkInfoRes, status:"", onerror:errorHandler});

		var newLANEnable  = (chkLANEnable.checked) ? 1 : 0;
		if (newLANEnable != LANCFG_DATA[getIndex(lstLANInterface.value)].lanEnable) {
			req.add("ethIndex", top.settings.eth[getIndexFromLANChannel(lstLANInterface.value)]);
			req.add("lanEnable", newLANEnable);
		}
		if (chkLANEnable.checked) {
			req.add("channelNum", lstLANInterface.value);
			if (chkV4IPSource.checked) {		//V4 DHCP
				req.add("v4IPSource", top.CONSTANTS.IPSOURCE_DHCP);
			} else {							//V4 Static
				req.add("v4IPSource", top.CONSTANTS.IPSOURCE_STATIC);
				req.add("v4IPAddr", txtV4IPAddress.value);
				req.add("v4Subnet", txtV4Subnet.value);
				req.add("v4Gateway", txtV4Gateway.value);
			}

			req.add("v6Enable", chkV6Enable.checked ? 1 : 0);
			if(chkV6Enable.checked) {
				if (chkV6IPSource.checked) {		//V6 DHCP
					req.add("v6IPSource", top.CONSTANTS.IPSOURCE_DHCP);
				} else {							//V6 Static
					req.add("v6IPSource", top.CONSTANTS.IPSOURCE_STATIC);
					req.add("v6IPAddr", txtV6IPAddress.value);
					req.add("v6Prefix", txtV6Subnet.value);
					req.add("v6Gateway", txtV6Gateway.value);
				}
			}

			if(chkVLANEnable.checked) {
				req.add("vlanEnable", 1);
				req.add("vlanID", txtVLANId.value);
				req.add("vlanPriority", txtVLANPriority.value);
			} else {
				req.add("vlanEnable", 0);
			}
		}
		req.send();
		delete req;
	}
}

/*
 * This function will handle any error while retrieving the response of 
 * setNetworkInfo RPC. 
 */
function errorHandler() 
{
	alert(eLang.getString("common", "STR_NETWORK_ERROR"));
}

/*
 * This is the response function for setNetworkInfo RPC. 
 * Need to check HAPI_STATUS, intimate end user if it returns non-zero value.
 * If zero, then network configuration is success, intimate proper message to end 
 * user.
 * @param arg object, RPC response data from xmit library
 */
function setNetworkInfoRes(arg)
{
	btnSave.disabled = false;
	switch(arg.HAPI_STATUS)
	{
		case 0:
			btnSave.disabled = true;
			resetNetwork();
		break;
		case 0xDE:	case 0xDF:
		case 0xE0:	case 0xE1:
		case 0xE2:	case 0xE3:
		case 0x1D5:
			errstr =  eLang.getString("common", "STR_CONF_NW_SETVAL");
			errstr += (eLang.getString("common", "STR_CONF_NW_ERR_" + 
				arg.HAPI_STATUS));
			alert(errstr);
		break;
		default:
			errstr =  eLang.getString("common", "STR_CONF_NW_SETVAL");
			errstr += (eLang.getString("common", "STR_IPMI_ERROR") + 
				GET_ERROR_CODE(arg.HAPI_STATUS));
			alert(errstr);
	}
}

/*
 * It will disable or enable the user controls based on Enable check box of 
 * LAN Settings.
 */
function enableLAN()
{
	var bopt;		//boolean value to enable/disable
	bopt = !chkLANEnable.checked;
	txtMACAddress.disabled = bopt;
	chkV4IPSource.disabled = bopt;
	chkV6Enable.disabled = bopt;
	DHCPEnable((chkV4IPSource.disabled || chkV4IPSource.checked), "V4");
	enableV6();
}

/*
 * It is used to disable or enable DHCP option for v4 configuration of LAN 
 * configuration 
 */
function doV4DHCP()
{
	DHCPEnable(chkV4IPSource.checked,"V4");
}

/*
 * It will disable or enable the v6 user controls based on Enable check box of 
 * v6 settings.
 */
function enableV6()
{
	chkV6IPSource.disabled = (chkV6Enable.disabled || (!chkV6Enable.checked));
	DHCPEnable((chkV6Enable.disabled || chkV6IPSource.disabled || 
		chkV6IPSource.checked), "V6");
	if(checkProjectCfg("NWBONDING"))
		getNWBondCfg();
}

/*
 * It is used to disable or enable v4 or v6 user controls based on DHCP option. 
 * @param enable boolean, true-disable, false-enable
 * @param str string, v4 or v6
 */
function DHCPEnable(enable, str)
{
	var index = 0;		//position of the global RPC data
	$("_txt" + str + "IPAddress").disabled = enable;
	$("_txt" + str + "Subnet").disabled = enable;
	$("_txt" + str + "Gateway").disabled = enable;
	if (enable) {
		index = getIndex(lstLANInterface.value);
		if (str == "V4") {
			loadV4Info(index);
		} else if (str == "V6") {
			loadV6Info(index);
		}
	}
}

/*
 * It will disable or enable VLAN user controls based on Enable check box of 
 * VLAN settings.
 */
function enableVLAN()
{
	var bopt;		//boolean value to enable/disable
	bopt = !chkVLANEnable.checked;
	txtVLANId.disabled = bopt;
	txtVLANPriority.disabled = bopt;
}
