//;*****************************************************************;
//;*****************************************************************;
//;**                                                             **;
//;**     (C) COPYRIGHT American Megatrends Inc. 2008-2010        **;
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

// File Name  : configure_phy_imp.js
// Brief      : This implementation is to display and configure the network 
// link on the BMC.
// Author Name: Arockia Selva Rani. A

var PHYSUPPORTCFG_DATA;			//It holds get RPC supported PHY response data
var PHYCFG_DATA;				//It holds get RPC PHY configuration response data

/*
 * This function will be called when its corresponding page gets loaded.
 * It will expose all the user controls and checks for user privilege.
 * Finally it will invoke the begin method. 
 */
function doInit()
{
	exposeElms([
		"_lstLanInterface",
		"_rdoAutoNegOn",
		"_rdoAutoNegOff",
		"_lstLinkSpeed",
		"_lstDuplexMode",
		"_btnSave",
		"_btnReset"]);
	if(top.user.isAdmin()) {
		btnSave.onclick = setPHYCfg;
		btnReset.onclick = reloadPHYCfg;
		lstLinkSpeed.onchange = fillDuplexMode;
		rdoAutoNegOn.onclick = enableAutoNeg;
		rdoAutoNegOff.onclick = enableAutoNeg;
	} else if(top.user.isOperator()) {
		disableActions({id: ["_lstLanInterface"]});
	} else {
		alert(eLang.getString("common", "STR_PERMISSION_DENIED"));
		location.href = "dashboard.html";
		return;
	}
	_begin();
}

/*
 * It will initiate event handler for the controls which is accessible for all 
 * privileged user. It will invoke the RPC method to get the data for the page.
 */
function _begin()
{
	lstLanInterface.onchange = function ()
	{
		fillSupportCfg();
		reloadPHYCfg();
	};
	getPHYSupportCfg();
}

/*
 * It will invoke the RPC method to get the network link supported configuration.
 * Once it get response from RPC, on receive method will be called automatically.
 */
function getPHYSupportCfg()
{
	xmit.get({url:"/rpc/getphysupportcfg.asp", onrcv:getPHYSupportCfgRes, status:""});
}

/*
 * This is the response function for getPHYSupportCfg RPC. 
 * Need to check HAPI_STATUS, intimate end user if it returns non-zero value.
 * If success, move the response data to the global variable and invoke the 
 * method to load the data value in UI. 
 * @param arg object, RPC response data from xmit library
 */
function getPHYSupportCfgRes(arg)
{
	var errstr;		//Error string
	if(arg.HAPI_STATUS) {
		errstr = eLang.getString("common", "STR_CONF_PHY_GETVAL");
		errstr += (eLang.getString("common", "STR_IPMI_ERROR") + 
			GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	} else {
		PHYSUPPORTCFG_DATA = WEBVAR_JSONVAR_GETPHYSUPPORTCFG.WEBVAR_STRUCTNAME_GETPHYSUPPORTCFG;
		fillLANInterface();
		fillSupportCfg();
		getPHYCfg();
	}
}

/*
 * It will invoke the RPC method to get the network link configuration.
 * Once it get response from RPC, on receive method will be called automatically.
 */
function getPHYCfg()
{
	xmit.get({url:"/rpc/getphycfg.asp", onrcv:getPHYCfgRes, status:""});
}

/*
 * This is the response function for getPHYCfg RPC. 
 * Need to check HAPI_STATUS, intimate end user if it returns non-zero value.
 * If success, move the response data to the global variable and invoke the 
 * method to load the data value in UI. 
 * @param arg object, RPC response data from xmit library
 */
function getPHYCfgRes(arg)
{
	var errstr;		//Error string
	if(arg.HAPI_STATUS) {
		errstr = eLang.getString("common", "STR_CONF_PHY_GETVAL");
		errstr += (eLang.getString("common", "STR_IPMI_ERROR") + 
			GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	} else {
		PHYCFG_DATA = WEBVAR_JSONVAR_GETPHYCFG.WEBVAR_STRUCTNAME_GETPHYCFG;
		reloadPHYCfg();
	}
}

/*
 * It will load response data from global variable to respective controls in UI.
 */
function reloadPHYCfg()
{
	var index = getIndexFromPHYCfg(lstLanInterface.value);

	lstLinkSpeed.value = PHYCFG_DATA[index].LINK_SPEED;
	lstDuplexMode.value = PHYCFG_DATA[index].DUPLEX_MODE;

	rdoAutoNegOn.checked = PHYCFG_DATA[index].AUTO_NEG ? true : false;
	rdoAutoNegOff.checked = (!PHYCFG_DATA[index].AUTO_NEG) ? true : false;
	enableAutoNeg();
}

/*
 * It will invoke the RPC method to set the network link configuration.
 * Once it get response from RPC, on receive method will be called automatically.
 */
function setPHYCfg()
{
	var req;			//xmit object to send RPC request with parameters
	req = new xmit.getset({url:"/rpc/setphycfg.asp", onrcv:setPHYCfgRes, 
		status:""});
	req.add("ETHINDEX",lstLanInterface.value);
	req.add("AUTO_NEG",rdoAutoNegOn.checked ? 1 : 0);
	req.add("LINK_SPEED",lstLinkSpeed.value);
	req.add("DUPLEX_MODE",(lstDuplexMode.value == "HALF") ? 0 : 1);	//HALF->0, FULL->1
	req.send();
	delete req;
}

/*
 * This is the response function for setPHYCfg RPC. 
 * Need to check HAPI_STATUS, intimate end user if it returns non-zero value.
 * If zero, then setting network bonding configuration is success, intimate 
 * proper message to end user.
 * @param arg object, RPC response data from xmit library
 */
function setPHYCfgRes(arg)
{
	var errstr;		//Error string
	if(arg.HAPI_STATUS) {
		errstr = eLang.getString("common", "STR_CONF_PHY_SETVAL");
		errstr += (eLang.getString("common", "STR_IPMI_ERROR") + 
			GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	} else {
		alert (eLang.getString("common", "STR_CONF_PHY_SAVE_SUCCESS"));
		getPHYCfg();
	}
}

/*
 * It will return the index value of the ethIndex from the global RPC PHY
 * configuration data. 
 * @param ethIndex number, one of the value in global RPC response data.
 * @return number, index position of the global RPC data.
 */
function getIndexFromPHYCfg(ethIndex)
{
	var i; 				//loop counter
	var index = -1;		//position of the global RPC data
	for (i = 0; i < PHYCFG_DATA.length; i++) {
		if (PHYCFG_DATA[i].ETHINDEX == ethIndex) {
			index = i;
			break;
		}
	}
	if (index == -1) {
		alert ("Error in getting index for ETH Index::" + ethIndex);
		index = 0;
	}
	return index;
}

/*
 * It will return the index value of the ethIndex from the global RPC supported 
 * PHY capabilities configuration data. 
 * @param ethIndex number, one of the value in global RPC response data.
 * @return number, index position of the global RPC data.
 */
function getIndexFromSupportCfg(ethIndex)
{
	var i; 				//loop counter
	var index = -1;		//position of the global RPC data
	for (i = 0; i < PHYSUPPORTCFG_DATA.length; i++) {
		if (PHYSUPPORTCFG_DATA[i].ETHINDEX == ethIndex) {
			index = i;
			break;
		}
	}
	if (index == -1) {
		alert ("Error in getting index for ETH Index::" + ethIndex);
		index = 0;
	}
	return index;
}

/*
 * It will fill the supported PHY configuration data in the list box. 
 * Auto Negotiation, Link Speed and Duplex mode.
 */
function fillSupportCfg()
{
	fillAutoNeg();
	fillLinkSpeed();
	fillDuplexMode();
}

/*
 * It will fill the LAN interface string in the list box. 
 */
function fillLANInterface()
{
	var i; 				//loop counter
	lstLanInterface.innerHTML = "";
	for (i=0;i<PHYSUPPORTCFG_DATA.length;i++) {
		lstLanInterface.add(new Option(PHYSUPPORTCFG_DATA[i].IFCNAME,
			PHYSUPPORTCFG_DATA[i].ETHINDEX), isIE?i:null);
	}
}

/*
 * It will fill the Auto negotiation data of the supported PHY configuration.
 */
function fillAutoNeg()
{
	var index;		//position of the global RPC data
	index = getIndexFromSupportCfg(lstLanInterface.value);
	if(PHYSUPPORTCFG_DATA[index].AUTONEG == 0){		//Auto Negotiation is not supported
		rdoAutoNegOff.checked = true;
		rdoAutoNegOn.disabled = true;
		rdoAutoNegOff.disabled = true;
	} else if (PHYSUPPORTCFG_DATA[index].AUTONEG == 1){	//Auto Negotiation is supported
		rdoAutoNegOn.disabled = false;
		rdoAutoNegOff.disabled = false;
	}
}

/*
 * It will fill the Link speed data of the supported PHY configuration.
 */
function fillLinkSpeed()
{
	var i; 				//loop counter
	var index;			//position of the global RPC data
	var optind = 0;		//Optional index needed for IE, starts with 0
	var varLinkSpeed;	//Number to hold link speed data
	var varSpeedDuplex;	//String to hold speed and duplex data

	index = getIndexFromSupportCfg(lstLanInterface.value);
	if (PHYSUPPORTCFG_DATA[index].SUPPORTCFG != "") {
		lstLinkSpeed.innerHTML = "";
		varSpeedDuplex = PHYSUPPORTCFG_DATA[index].SUPPORTCFG.split(",");
		for (i = 0; i < varSpeedDuplex.length; i++) {
			if (varSpeedDuplex[i].indexOf(" ") != -1) {
				varLinkSpeed = varSpeedDuplex[i].split(" ")[0];
				lstLinkSpeed.add(new Option(eLang.getString("common", 
					"STR_PHY_LINKSPEED_" + varLinkSpeed), varLinkSpeed), 
					isIE?optind++:null);
			}
		}
		removeDuplicate(lstLinkSpeed);
	} else {
		alert (eLang.getString("common","STR_CONF_PHY_SUPPORT_GETVAL"));
	}
}

/*
 * It will fill the Duplex mode data of the supported PHY configuration.
 */
function fillDuplexMode()
{
	var i; 				//loop counter
	var index;			//position of the global RPC data
	var optind = 0;		//Optional index needed for IE, starts with 0
	var varDuplexMode;	//Number to hold link duplex data
	var varSpeedDuplex;	//String to hold speed and duplex data

	index = getIndexFromSupportCfg(lstLanInterface.value);
	lstDuplexMode.innerHTML = "";
	varSpeedDuplex = PHYSUPPORTCFG_DATA[index].SUPPORTCFG.split(",");
	for (i = 0; i < varSpeedDuplex.length; i++) {
		if (varSpeedDuplex[i].indexOf(" ") != -1) {
			if (lstLinkSpeed.value == varSpeedDuplex[i].split(" ")[0]) {
				varDuplexMode = varSpeedDuplex[i].split(" ")[1].toUpperCase();
				lstDuplexMode.add(new Option(eLang.getString("common", 
					"STR_PHY_DUPLEXMODE_" + varDuplexMode),varDuplexMode),
					isIE?optind++:null);
			}
		}
	}
}

/*
 * It will disable or enable the user controls based on Auto negotiation and
 * Link speed value.
 */
function enableAutoNeg()
{
	rdoAutoNegOn.disabled = false;
	rdoAutoNegOff.disabled = false;
	if (rdoAutoNegOn.checked) {
		lstLinkSpeed.disabled = true;
		lstDuplexMode.disabled = true;
	} else {
		lstLinkSpeed.disabled = false;
		lstDuplexMode.disabled = false;
	}
}