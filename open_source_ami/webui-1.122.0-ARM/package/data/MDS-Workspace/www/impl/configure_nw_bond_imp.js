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

// File Name  : configure_nw_bond_imp.js
// Brief      : This implementation is to display and configure the network 
// bonding on the BMC.
// Author Name: Arockia Selva Rani. A

var NWBONDCFG_DATA;			//It holds the get RPC virtual media response data
var BOND_SUPPORT;
var bondEnable = 0;

/*
 * This function will be called when its corresponding page gets loaded.
 * It will expose all the user controls and checks for user privilege.
 * Finally it will invoke the begin method. 
 */
function doInit()
{
	exposeElms([
		"_chkEnableBond",
		"_lstBondIfc",
		"_chkAutoConf",
		"_btnSave",
		"_btnReset"
		]);

	if(top.user.isAdmin()) {
	} else if(top.user.isOperator()) {
		disableActions();
	} else {
		alert(eLang.getString("common", "STR_PERMISSION_DENIED"));
		location.href = "dashboard.html";
		return;
	}
	_begin();
}

/*
 * It will fill data for user controls like list box, if any.
 * Also it will invoke the RPC method to get the data for the page.
 */
function _begin()
{
	chkAutoConf.checked = true;
	checkNWBondSupport();
}

/*
 * It will invoke the RPC method to check whether network bonding can be 
 * enabled or not, based on NIC count in the BMC. Once it get response from 
 * RPC, on receive method will be called automatically.
 */
function checkNWBondSupport()
{
	xmit.get({url:"/rpc/checknwbond.asp", onrcv:checkNWBondSupportRes, status:""});
}

/*
 * This is the response function for checkNWBondSupport RPC. 
 * Need to check HAPI_STATUS, intimate end user if it returns non-zero value.
 * If success, display the Network bonding page if it returns true or intimate
 * the end user network bonding cannot be enabled.
 * @param arg object, RPC response data from xmit library
 */
function checkNWBondSupportRes(arg)
{
	var errstr;		//Error string
	if(arg.HAPI_STATUS != 0) {
		errstr = eLang.getString("common", "STR_CONF_NW_BOND_GETVAL");
		errstr += (eLang.getString("common", "STR_IPMI_ERROR") + 
			GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	} else {
		BOND_SUPPORT = WEBVAR_JSONVAR_CHKNWBONDSUPPORT.WEBVAR_STRUCTNAME_CHKNWBONDSUPPORT[0];
		if (BOND_SUPPORT.NIC_COUNT >= 2) {
			if (top.user.isAdmin()) {
				btnSave.onclick = validateNWBondCfg;
				btnReset.onclick = reloadNWBondCfg;
				chkEnableBond.onclick = enableNWBond;
			} else {
				disableActions();
			}
			fillBondInterfaces();
			getNWBondCfg();
		} else {
			alert (eLang.getString("common", "STR_CONF_NW_BOND_NOT_SUPPORT"));
			location.href = "dashboard.html";
		}
	}
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
		reloadNWBondCfg();
	}
}

/*
 * It will load response data from global variable to respective controls in UI.
 */
function reloadNWBondCfg()
{
	chkEnableBond.checked = (NWBONDCFG_DATA.BOND_ENABLE == 1) ? true : false;
	lstBondIfc.value = NWBONDCFG_DATA.BOND_IFC;
	enableNWBond();
	chkAutoConf.checked = true;
}

function validateNWBondCfg()
{
	var autoconf = (chkAutoConf.checked) ? 1 : 0;
	if ((NWBONDCFG_DATA.BOND_ENABLE != bondEnable) || 
		(NWBONDCFG_DATA.BOND_IFC != lstBondIfc.value)) {
		if (NWBONDCFG_DATA.VLAN_ENABLE && !bondEnable) {
			if (confirm(eLang.getString("common",
				"STR_CONF_NW_BOND_CNFM_BOND") + eLang.getString("common",
				"STR_CONF_NW_BOND_CNFM_AUTO" + autoconf))) {
				setNWBondCfg();
			}
		} else {
			if (confirm(eLang.getString("common", "STR_CONF_NW_BOND_CNFM_AUTO" +
				autoconf))) {
				setNWBondCfg();
			}
			
		}
	}
}

/*
 * It will invoke the RPC method to set the network bonding configuration.
 * Once it get response from RPC, on receive method will be called automatically.
 */
function setNWBondCfg()
{
	var req;			//xmit object to send RPC request with parameters
	req = new xmit.getset({url:"/rpc/setnwbondcfg.asp", onrcv:setNWBondCfgRes, 
		status:""});
	req.add("BOND_ENABLE", bondEnable);
	req.add("BOND_IFC", lstBondIfc.value);
	req.add("AUTO_CONF", (chkAutoConf.checked) ? 1 : 0);
	req.send();
	delete req;
}

/*
 * This is the response function for setNWBondCfg RPC. 
 * Need to check HAPI_STATUS, intimate end user if it returns non-zero value.
 * If zero, then setting network bonding configuration is success, intimate 
 * proper message to end user.
 * @param arg object, RPC response data from xmit library
 */
function setNWBondCfgRes(arg)
{
	var errstr;		//Error string
	switch(GET_ERROR_CODE(arg.HAPI_STATUS))
	{
	case 0x80:
	case 0x82:
		alert (eLang.getString("common", "STR_CONF_NW_BOND_SETVAL_" + 
			GET_ERROR_CODE(arg.HAPI_STATUS)));
		break;
	case 0x0:
		btnSave.disabled = true;
		resetNetwork();
		break;
	default:
		errstr = eLang.getString("common", "STR_CONF_NW_BOND_SETVAL");
		errstr += (eLang.getString("common", "STR_IPMI_ERROR") + 
			GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	}
}

/*
 * It will fill available bond interfaces in list box.
 */
 function fillBondInterfaces()
{
	var index;				//loop counter
	lstBondIfc.innerHTML = "";
	for (index = 1; index <= 3; index++) {
		lstBondIfc.add(new Option(eLang.getString("common", 
			"STR_NW_BOND_IFC_" + index), index), isIE?index:null);
	}
}

/*
 * It will disable or enable the user controls based on Enable check box option.
 */
function enableNWBond()
{
	bondEnable = (chkEnableBond.checked) ? 1 : 0;
	if (top.user.isAdmin()) {
		lstBondIfc.disabled = !chkEnableBond.checked;
	}
}