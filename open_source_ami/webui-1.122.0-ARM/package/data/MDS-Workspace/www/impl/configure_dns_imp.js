//;*****************************************************************;
//;*****************************************************************;
//;**                                                             **;
//;**     (C) COPYRIGHT American Megatrends Inc. 2008-2011        **;
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

// File Name  : configure_dns
// Brief      :
// Author Name:

var LANCFG_DATA;
var DNSCFG_DATA;
var timer = 0;
var varDHCP = [4];
var v6Enable = false;

var CONST_IPV4_PRIORITY = 1;
var CONST_IPV6_PRIORITY = 2;

function doInit()
{
	exposeElms([
		"_lstHostSetting",
		"_txtHostName",
		"_rowRegisterBMC0",
		"_tdRegisterBMC0",
		"_chkRegisterBMC0",
		"_rdoRegisterDDNS0",
		"_rdoRegisterDHCP0",
		"_rowRegisterBMC1",
		"_tdRegisterBMC1",
		"_chkRegisterBMC1",
		"_rdoRegisterDDNS1",
		"_rdoRegisterDHCP1",
		"_rowRegisterBMC2",
		"_tdRegisterBMC2",
		"_chkRegisterBMC2",
		"_rdoRegisterDDNS2",
		"_rdoRegisterDHCP2",
		"_lstDomainSetting",
		"_txtDomainName",
		"_lstDNSSetting",
		"_rdoV4Priority",
		"_rdoV6Priority",
		"_txtDNS1",
		"_txtDNS2",
		"_txtDNS3",
		"_btnSave",
		"_btnReset"]);

	if (top.user.isAdmin()) {
		btnSave.onclick = validateDNSCfg;
		btnReset.onclick = reloadDNSCfg;
	} else if (top.user.isOperator()) {
		disableActions();
	} else {
		alert(eLang.getString("common", "STR_PERMISSION_DENIED"));
		location.href = "dashboard.html";
		return;
	}
	lstHostSetting.onchange = enableHost;
	chkRegisterBMC0.onclick = checkRegisterBMC;
	chkRegisterBMC1.onclick = checkRegisterBMC;
	chkRegisterBMC2.onclick = checkRegisterBMC;
	lstDomainSetting.onchange = enableDomain;
	lstDNSSetting.onchange = enableDNS;

	_begin();
}

function _begin()
{
	var index = 0;

	/* Host Configuration */
	lstHostSetting.add(new Option(eLang.getString("common", "STR_MANUAL"), index),
		isIE ? index : null);
	index++;
	lstHostSetting.add(new Option(eLang.getString("common", "STR_AUTO"), index),
		isIE ? index : null);

	/* Register BMC Configuration */
	for (index = 0; index < top.settings.lan.length; index++) {
		$("_rowRegisterBMC" + top.settings.eth[index]).className = "visibleRow";
		$("_tdRegisterBMC" + top.settings.eth[index]).innerHTML = "<strong>" +
			top.settings.ethstr[index] + "</strong>";
	}

	/* Domain Name and DNS Configuration */
	index = 0;
	lstDomainSetting.add(new Option(eLang.getString("common", "STR_MANUAL"),
		eLang.getString("common", "STR_MANUAL")), isIE ? index : null);
	lstDNSSetting.add(new Option(eLang.getString("common", "STR_MANUAL"),
		index), isIE ? index : null);

	getNetworkInfo();
}

function getNetworkInfo()
{
	xmit.get({url:"/rpc/getalllancfg.asp", onrcv:getAllLANCfgRes, status:""});
}

function getAllLANCfgRes(arg)
{
	switch (arg.HAPI_STATUS) {
	case 0x0:
		LANCFG_DATA = WEBVAR_JSONVAR_GETALLNETWORKCFG.WEBVAR_STRUCTNAME_GETALLNETWORKCFG;
		loadLANInfo();
		getDNSCfg();
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

function getDNSCfg()
{
	xmit.get({url:"/rpc/getdnscfg.asp", onrcv:getDNSCfgRes, status:""});
}

function getDNSCfgRes(arg)
{
	if (arg.HAPI_STATUS != 0) {
		errstr = eLang.getString("common", "STR_CONF_NW_DNSVAL");
		errstr += (eLang.getString("common", "STR_IPMI_ERROR") + 
			GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	} else {
		DNSCFG_DATA = WEBVAR_JSONVAR_GETDNSCFG.WEBVAR_STRUCTNAME_GETDNSCFG[0];
		reloadDNSCfg();
	}
}

function loadLANInfo()
{
	var index = 0;
	var domainstr = "";
	var domainindex = 1;
	var dnsindex = 1;

	varDHCP[0] = false;
	varDHCP[1] = false;
	varDHCP[2] = false;

	for (var i = 0; i<LANCFG_DATA.length; i++) {
		index = getIndexFromLANChannel(LANCFG_DATA[i].channelNum);
		if (getbits(LANCFG_DATA[i].v4IPSource,3,0) ==
			top.CONSTANTS.IPSOURCE_DHCP) {
			varDHCP[top.settings.eth[index]] = true;
			lstDNSSetting.add(new Option(top.settings.ethstr[index],
				top.settings.lan[index]), isIE ? dnsindex++ : null);
			domainstr = top.settings.ethstr[index] + "_v4";
			lstDomainSetting.add(new Option(domainstr, domainstr),
				isIE ? domainindex++ : null);
		}
		if ((getbits(LANCFG_DATA[i].v6IPSource,3,0) ==
			top.CONSTANTS.IPSOURCE_DHCP) && (LANCFG_DATA[i].v6Enable == 1)) {
			domainstr = top.settings.ethstr[index] + "_v6";
			lstDomainSetting.add(new Option(domainstr, domainstr),
				isIE ? domainindex++ : null);
		}
		if (LANCFG_DATA[i].v6Enable == 1) {
			v6Enable = true;
		}
	}
}

function reloadDNSCfg()
{
	/* Host Configuration */
	lstHostSetting.value = DNSCFG_DATA.HOST_CFG;
	txtHostName.value = DNSCFG_DATA.HOST_NAME;
	enableHost();

	/* Register BMC Configuration */
	chkRegisterBMC0.checked = (DNSCFG_DATA.REG_BMC0 == 1) ? true : false;
	enableRegisterBMC(0);
	chkRegisterBMC1.checked = (DNSCFG_DATA.REG_BMC1 == 1) ? true : false;
	enableRegisterBMC(1);
	chkRegisterBMC2.checked = (DNSCFG_DATA.REG_BMC2 == 1) ? true : false;
	enableRegisterBMC(2);

	if (chkRegisterBMC0.checked == true) {
		rdoRegisterDDNS0.checked = (DNSCFG_DATA.REG_DHCP0 == 0) ? true : false;
		rdoRegisterDHCP0.checked = (DNSCFG_DATA.REG_DHCP0 == 1) ? true : false;
	}
	if (chkRegisterBMC1.checked == true) {
		rdoRegisterDDNS1.checked = (DNSCFG_DATA.REG_DHCP1 == 0) ? true : false;
		rdoRegisterDHCP1.checked = (DNSCFG_DATA.REG_DHCP1 == 1) ? true : false;
	}
	if (chkRegisterBMC2.checked == true) {
		rdoRegisterDDNS2.checked = (DNSCFG_DATA.REG_DHCP2 == 0) ? true : false;
		rdoRegisterDHCP2.checked = (DNSCFG_DATA.REG_DHCP2 == 1) ? true : false;
	}

	/* Domain Name Configuration */
	lstDomainSetting.value = DNSCFG_DATA.DOMAIN_CFG;
	txtDomainName.value = DNSCFG_DATA.DOMAIN_NAME;
	enableDomain();

	/* DNS Server Configuration */
	lstDNSSetting.value = DNSCFG_DATA.DNS_CFG;
	enableDNS();
	txtDNS1.value = DNSCFG_DATA.DNS_IP1;
	txtDNS2.value = DNSCFG_DATA.DNS_IP2;
	txtDNS3.value = DNSCFG_DATA.DNS_IP3;
	rdoV4Priority.checked = (DNSCFG_DATA.DNS_PRIORITY == CONST_IPV4_PRIORITY) ?
		true : false;
	rdoV6Priority.checked = (DNSCFG_DATA.DNS_PRIORITY == CONST_IPV6_PRIORITY) ?
		true : false;

	lstHostSetting.focus();
}

function setDNSCfgRes(arg)
{
	btnSave.disabled = false;
	clearTimeout(timer);
	switch(arg.HAPI_STATUS)
	{
		case 0:
			resetWebUI();
		break;

		case 0xDE: 	case 0xDF:
		case 0xE0:	case 0xE1:
		case 0xE2:	case 0xE3:
			errstr =  eLang.getString("common","STR_CONF_NW_SETVAL");
			errstr += (eLang.getString("common","STR_CONF_NW_ERR_" +
				arg.HAPI_STATUS));
			alert(errstr);
		break;

		default:
			errstr =  eLang.getString("common","STR_CONF_NW_SETVAL");
			errstr += (eLang.getString("common","STR_IPMI_ERROR") +
				GET_ERROR_CODE(arg.HAPI_STATUS));
			alert(errstr);
	}
}

function resetWebUI()
{
	btnSave.disabled = true;
	resetNetwork();
}

function Timedout()
{
	alert(eLang.getString("common","STR_CONF_NW_SETERR1"));
	btnSave.disabled = false;
}

function setDNSCfg()
{
	var req;
	var index;
	if (confirm(eLang.getString("common", "STR_CONF_NW_SAVE_CONFIRM"))) {
		btnSave.disabled = true;
		req = new xmit.getset({url:"/rpc/setdnscfg.asp", onrcv:setDNSCfgRes,
			ontimeout:Timedout});

		/* Host Configuration */
		req.add("HOST_CFG", lstHostSetting.value);
		if (lstHostSetting.value == 0) {
			req.add("HOST_NAME", txtHostName.value);
		}

		/* Register BMC Configuration */
		req.add("REG_BMC0", (chkRegisterBMC0.checked == true) ? 1 : 0);
		req.add("REG_BMC1", (chkRegisterBMC1.checked == true) ? 1 : 0);
		req.add("REG_BMC2", (chkRegisterBMC2.checked == true) ? 1 : 0);
		
		req.add("REG_DHCP0", (chkRegisterBMC0.checked &&
			rdoRegisterDHCP0.checked) ? 1 : 0);
		req.add("REG_DHCP1", (chkRegisterBMC1.checked &&
			rdoRegisterDHCP1.checked) ? 1 : 0);
		req.add("REG_DHCP2", (chkRegisterBMC2.checked &&
			rdoRegisterDHCP2.checked) ? 1 : 0);

		/* Domain Name Configuration */
		domaindata = lstDomainSetting.value.split("_");
		if (domaindata.length == 2) {
			req.add("DOMAIN_PRIORITY", domaindata[1]);
			//index = getIndexFromEthString(domaindata[0]);
			req.add("DOMAIN_ETH",top.settings.eth[getIndexFromEthString(domaindata[0])]);
		} else {
			//This variable holds 'Manual' setting for Domain Name
			req.add("DOMAIN_PRIORITY", lstDomainSetting.value);
			req.add("DOMAIN_NAME", txtDomainName.value);
		}

		/* DNS Server Configuration */
		if (lstDNSSetting.value == 0) {		//Manual configuration
			req.add("DNS_DHCP", 0);
			req.add("DNS_IP1",txtDNS1.value);
			req.add("DNS_IP2",txtDNS2.value);
			req.add("DNS_IP3",txtDNS3.value);
		} else {
			req.add("DNS_DHCP", 1);
			req.add("DNS_ETH", 
				top.settings.eth[getIndexFromLANChannel(lstDNSSetting.value)]);
			req.add("DNS_PRIORITY", rdoV4Priority.checked ?
				CONST_IPV4_PRIORITY : CONST_IPV6_PRIORITY);
		}

		req.send();
		delete req;
		//timer = setTimeout("resetWebUI()",2000);
	}
}

function validateDNSCfg()
{
	if (top.user.isAdmin()) {
		if (lstHostSetting.value == 0) {
			if ((!eVal.hostname(txtHostName.value)) ||
				(eVal.isblank(txtHostName.value))) {
				alert (eLang.getString("common", "STR_CONF_DNS_INVALID_HOST"));
				txtHostName.focus();
				return;
			}
		}

		if (lstDomainSetting.value == eLang.getString("common", "STR_MANUAL")) {
			if ((!eVal.domainname(txtDomainName.value, true)) ||
				(eVal.isblank(txtDomainName.value))) {
				alert (eLang.getString("common", "STR_CONF_DNS_INVALID_DOMAIN"));
				txtDomainName.focus();
				return;
			}
		}

		if (lstDNSSetting.value == 0) {
			if ((eVal.isblank(txtDNS1.value)) &&
				(eVal.isblank(txtDNS2.value)) &&
				(eVal.isblank(txtDNS3.value))) {
				alert(eLang.getString("common", "STR_CONF_DNS_BLANK"));
				txtDNS1.focus();
				return false;
			}

			if (!validateServerAddress(txtDNS1)) {
				return false;
			}

			if (!validateServerAddress(txtDNS2)) {
				return false;
			}

			if (!validateServerAddress(txtDNS3)) {
				return false;
			}

			if ((eVal.compareip(txtDNS1.value, txtDNS2.value)) || 
				(eVal.compareip(txtDNS1.value, txtDNS3.value)) || 
				(eVal.compareip(txtDNS2.value, txtDNS3.value))) {
				alert(eLang.getString("common", "STR_CONF_DNS_DIFF"));
				return false;
			}

			if (v6Enable) {
				if ((eVal.comparev6ip(txtDNS1.value, txtDNS2.value)) || 
					(eVal.comparev6ip(txtDNS1.value, txtDNS3.value)) || 
					(eVal.comparev6ip(txtDNS2.value, txtDNS3.value))) {
					alert(eLang.getString("common", "STR_CONF_DNS_DIFF"));
					return false;
				}
			}
		}
		setDNSCfg();
	} else {
		alert(eLang.getString("common", "STR_CONF_ADMIN_PRIV"));
	}
}

function validateServerAddress(txtDNS)
{
	if (!eVal.isblank(txtDNS.value)) {
		if (!eVal.ip(txtDNS.value)) {
			if (v6Enable) {
				if (!eVal.ipv6(txtDNS.value)) {
					alert(eLang.getString("common",
						"STR_CONF_DNS_INVALID_DNS") +
						eLang.getString("common", "STR_HELP_INFO"));
					txtDNS.focus();
					return false;
				}
			} else {
				alert(eLang.getString("common", "STR_CONF_DNS_INVALID_DNS") +
					eLang.getString("common", "STR_CONF_DNS_V6DISABLE"));
				txtDNS.focus();
				return false;
			}
		}
	}
	return true;
}

function enableHost()
{
	txtHostName.disabled = (lstHostSetting.value == 0) ? false : true;
}

function checkRegisterBMC()
{
	if (this.id.indexOf("0") != -1){
		enableRegisterBMC(0);
		if (this.checked == true){
			rdoRegisterDDNS0.checked = true;
		}
	}else if (this.id.indexOf("1") != -1){
		enableRegisterBMC(1);
		if (this.checked == true){
			rdoRegisterDDNS1.checked = true;
		}
	}else if (this.id.indexOf("2") != -1){
		enableRegisterBMC(2);
		if (this.checked == true){
			rdoRegisterDDNS2.checked = true;
		}
	}
}

function enableRegisterBMC(index)
{
	if ($("_chkRegisterBMC" + index).checked == true) {
		$("_rdoRegisterDDNS" + index).disabled = false;
		$("_rdoRegisterDHCP" + index).disabled = !(varDHCP[index]);
	} else {
		$("_rdoRegisterDDNS" + index).disabled = true;
		$("_rdoRegisterDHCP" + index).disabled = true;
		$("_rdoRegisterDDNS" + index).checked = false;
		$("_rdoRegisterDHCP" + index).checked = false;
	}
}

function enableDomain()
{
	txtDomainName.disabled = (lstDomainSetting.value == "Manual") ? false : true;
}

function enableDNS()
{
	var index;
	var opt = (lstDNSSetting.value == 0) ? false : true;

	if (opt) {
		index = getIndexFromLANChannel(lstDNSSetting.value);
		if (LANCFG_DATA[index].lanEnable && 
			(LANCFG_DATA[index].v4IPSource == top.CONSTANTS.IPSOURCE_DHCP)) {
			rdoV4Priority.checked = true;
			rdoV4Priority.disabled = false;
		} else {
			rdoV4Priority.checked = false;
			rdoV4Priority.disabled = true;
		}

		if (LANCFG_DATA[index].lanEnable && LANCFG_DATA[index].v6Enable &&
			(LANCFG_DATA[index].v6IPSource == top.CONSTANTS.IPSOURCE_DHCP)) {
			rdoV6Priority.disabled = false;
		} else {
			rdoV6Priority.checked = false;
			rdoV6Priority.disabled = true;
		}
	} else {
		rdoV4Priority.checked = false;
		rdoV4Priority.disabled = true;
		rdoV6Priority.checked = false;
		rdoV6Priority.disabled = true;
	}
	txtDNS1.disabled = opt;
	txtDNS2.disabled = opt;
	txtDNS3.disabled = opt;
}
