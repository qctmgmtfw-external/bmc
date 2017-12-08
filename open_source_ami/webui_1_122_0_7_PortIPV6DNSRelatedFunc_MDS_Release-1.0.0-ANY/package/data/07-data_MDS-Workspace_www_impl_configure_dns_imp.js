--- .pristine/webui-1.122.0-src/data/MDS-Workspace/www/impl/configure_dns_imp.js Fri Feb  3 16:19:05 2012
+++ source/webui-1.122.0-src/data/MDS-Workspace/www/impl/configure_dns_imp.js Fri Feb  3 19:23:25 2012
@@ -1,7 +1,7 @@
 //;*****************************************************************;
 //;*****************************************************************;
 //;**                                                             **;
-//;**     (C) COPYRIGHT American Megatrends Inc. 2008-2010        **;
+//;**     (C) COPYRIGHT American Megatrends Inc. 2008-2011        **;
 //;**                     ALL RIGHTS RESERVED                     **;
 //;**                                                             **;
 //;**  This computer software, including display screens and      **;
@@ -32,6 +32,10 @@
 var DNSCFG_DATA;
 var timer = 0;
 var varDHCP = [4];
+var v6Enable = false;
+
+var CONST_IPV4_PRIORITY = 1;
+var CONST_IPV6_PRIORITY = 2;
 
 function doInit()
 {
@@ -55,27 +59,22 @@
 		"_rdoRegisterDHCP2",
 		"_lstDomainSetting",
 		"_txtDomainName",
-		"_lstV4DNSSetting",
-		"_txtV4DNS1",
-		"_txtV4DNS2",
-		"_lstV6DNSSetting",
-		"_txtV6DNS1",
-		"_txtV6DNS2",
+		"_lstDNSSetting",
+		"_rdoV4Priority",
+		"_rdoV6Priority",
+		"_txtDNS1",
+		"_txtDNS2",
+		"_txtDNS3",
 		"_btnSave",
 		"_btnReset"]);
 
-	if(top.user.isAdmin())
-	{
+	if (top.user.isAdmin()) {
 		btnSave.onclick = validateDNSCfg;
 		btnReset.onclick = reloadDNSCfg;
-	}
-	else if(top.user.isOperator())
-	{
+	} else if (top.user.isOperator()) {
 		disableActions();
-	}
-	else
-	{
-		alert(eLang.getString("common","STR_PERMISSION_DENIED"));
+	} else {
+		alert(eLang.getString("common", "STR_PERMISSION_DENIED"));
 		location.href = "dashboard.html";
 		return;
 	}
@@ -84,65 +83,86 @@
 	chkRegisterBMC1.onclick = checkRegisterBMC;
 	chkRegisterBMC2.onclick = checkRegisterBMC;
 	lstDomainSetting.onchange = enableDomain;
-	lstV4DNSSetting.onchange = enableV4DNS;
-	lstV6DNSSetting.onchange = enableV6DNS;
+	lstDNSSetting.onchange = enableDNS;
 
 	_begin();
 }
 
 function _begin()
 {
+	var index = 0;
+
+	/* Host Configuration */
+	lstHostSetting.add(new Option(eLang.getString("common", "STR_MANUAL"), index),
+		isIE ? index : null);
+	index++;
+	lstHostSetting.add(new Option(eLang.getString("common", "STR_AUTO"), index),
+		isIE ? index : null);
+
+	/* Register BMC Configuration */
+	for (index = 0; index < top.settings.lan.length; index++) {
+		$("_rowRegisterBMC" + top.settings.eth[index]).className = "visibleRow";
+		$("_tdRegisterBMC" + top.settings.eth[index]).innerHTML = "<strong>" +
+			top.settings.ethstr[index] + "</strong>";
+	}
+
+	/* Domain Name and DNS Configuration */
+	index = 0;
+	lstDomainSetting.add(new Option(eLang.getString("common", "STR_MANUAL"),
+		eLang.getString("common", "STR_MANUAL")), isIE ? index : null);
+	lstDNSSetting.add(new Option(eLang.getString("common", "STR_MANUAL"),
+		index), isIE ? index : null);
+
 	getNetworkInfo();
 }
 
 function getNetworkInfo()
 {
-	xmit.get({url:"/rpc/getalllancfg.asp",onrcv:getAllLANCfgRes,status:""});
+	xmit.get({url:"/rpc/getalllancfg.asp", onrcv:getAllLANCfgRes, status:""});
 }
 
 function getAllLANCfgRes(arg)
 {
-	switch(arg.HAPI_STATUS)
-	{
-		case 0x0:
-			LANCFG_DATA = WEBVAR_JSONVAR_GETALLNETWORKCFG.WEBVAR_STRUCTNAME_GETALLNETWORKCFG;
-			loadLANInfo();
-			getDNSCfg();
-		break;
-
-		case 0x1D4:
-			alert (eLang.getString("common","STR_PERMISSION_DENIED"));
-			location.href = "dashboard.html";
-		break;
-
-		case 0xE2:	case 0xE3:
-			errstr = eLang.getString("common","STR_CONF_NW_GETVAL");
-			errstr += (eLang.getString("common","STR_CONF_NW_ERR_" + arg.HAPI_STATUS));
-			alert(errstr);
-		break;
-
-		default:
-			errstr = eLang.getString("common","STR_CONF_NW_GETVAL");
-			errstr += (eLang.getString("common","STR_IPMI_ERROR") + GET_ERROR_CODE(arg.HAPI_STATUS));
-			alert(errstr);
+	switch (arg.HAPI_STATUS) {
+	case 0x0:
+		LANCFG_DATA = WEBVAR_JSONVAR_GETALLNETWORKCFG.WEBVAR_STRUCTNAME_GETALLNETWORKCFG;
+		loadLANInfo();
+		getDNSCfg();
+	break;
+
+	case 0x1D4:
+		alert (eLang.getString("common", "STR_PERMISSION_DENIED"));
+		location.href = "dashboard.html";
+	break;
+
+	case 0xE2:	case 0xE3:
+		errstr = eLang.getString("common", "STR_CONF_NW_GETVAL");
+		errstr += (eLang.getString("common", "STR_CONF_NW_ERR_" +
+			arg.HAPI_STATUS));
+		alert(errstr);
+	break;
+
+	default:
+		errstr = eLang.getString("common", "STR_CONF_NW_GETVAL");
+		errstr += (eLang.getString("common", "STR_IPMI_ERROR") +
+			GET_ERROR_CODE(arg.HAPI_STATUS));
+		alert(errstr);
 	}
 }
 
 function getDNSCfg()
 {
-	xmit.get({url:"/rpc/getdnscfg.asp",onrcv:getDNSCfgRes,status:""});
+	xmit.get({url:"/rpc/getdnscfg.asp", onrcv:getDNSCfgRes, status:""});
 }
 
 function getDNSCfgRes(arg)
 {
-	if (arg.HAPI_STATUS != 0)
-	{
-		errstr = eLang.getString("common","STR_CONF_NW_DNSVAL");
-		errstr += (eLang.getString("common","STR_IPMI_ERROR") + GET_ERROR_CODE(arg.HAPI_STATUS));
+	if (arg.HAPI_STATUS != 0) {
+		errstr = eLang.getString("common", "STR_CONF_NW_DNSVAL");
+		errstr += (eLang.getString("common", "STR_IPMI_ERROR") + 
+			GET_ERROR_CODE(arg.HAPI_STATUS));
 		alert(errstr);
-	}
-	else
-	{
+	} else {
 		DNSCFG_DATA = WEBVAR_JSONVAR_GETDNSCFG.WEBVAR_STRUCTNAME_GETDNSCFG[0];
 		reloadDNSCfg();
 	}
@@ -150,105 +170,81 @@
 
 function loadLANInfo()
 {
-	var v6Enable = false;
-	var index, hostindex = 0;
-	var lanInterfaceVal = "";
-	var domainindex = 0, domainstr = "";
-	var v4index = 0, v6index = 0;
-
-	/* Host Configuration */
-	lstHostSetting.add(new Option(eLang.getString("common","STR_MANUAL"),0),isIE?hostindex++:null);
-	lstHostSetting.add(new Option(eLang.getString("common","STR_AUTO"),1),isIE?hostindex++:null);
-
-	/* Register BMC Configuration */
-	for (i = 0; i < top.settings.lan.length; i++)
-	{
-		$("_rowRegisterBMC" + top.settings.eth[i]).className = "visibleRow";
-		$("_tdRegisterBMC" + top.settings.eth[i]).innerHTML = "<strong>" + top.settings.ethstr[i] + "</strong>";
-	}
+	var index = 0;
+	var domainstr = "";
+	var domainindex = 1;
+	var dnsindex = 1;
+
 	varDHCP[0] = false;
 	varDHCP[1] = false;
 	varDHCP[2] = false;
 
-	/* Domain Name and DNS Configuration */
-	lstDomainSetting.add(new Option(eLang.getString("common","STR_MANUAL"),eLang.getString("common","STR_MANUAL")),isIE?domainindex++:null);
-	lstV4DNSSetting.add(new Option(eLang.getString("common","STR_MANUAL"),0),isIE?v4index++:null);
-	lstV6DNSSetting.add(new Option(eLang.getString("common","STR_MANUAL"),0),isIE?v6index++:null);
-
-	for (var i=0; i<LANCFG_DATA.length; i++)
-	{
+	for (var i = 0; i<LANCFG_DATA.length; i++) {
 		index = getIndexFromLANChannel(LANCFG_DATA[i].channelNum);
-		if (getbits(LANCFG_DATA[i].v4IPSource,3,0) == top.CONSTANTS.IPSOURCE_DHCP)	//v4 DNS Source - DHCP
-		{
+		if (getbits(LANCFG_DATA[i].v4IPSource,3,0) ==
+			top.CONSTANTS.IPSOURCE_DHCP) {
 			varDHCP[top.settings.eth[index]] = true;
-			lstV4DNSSetting.add(new Option(top.settings.ethstr[index],top.settings.lan[index]),isIE?v4index++:null);
+			lstDNSSetting.add(new Option(top.settings.ethstr[index],
+				top.settings.lan[index]), isIE ? dnsindex++ : null);
 			domainstr = top.settings.ethstr[index] + "_v4";
-			lstDomainSetting.add(new Option(domainstr,domainstr),isIE?domainindex++:null);
-		}
-		if ((getbits(LANCFG_DATA[i].v6IPSource,3,0) == top.CONSTANTS.IPSOURCE_DHCP) && (LANCFG_DATA[i].v6Enable == 1))	//v6 DNS Source
-		{
-			lstV6DNSSetting.add(new Option(top.settings.ethstr[index],top.settings.lan[index]),isIE?v6index++:null);
+			lstDomainSetting.add(new Option(domainstr, domainstr),
+				isIE ? domainindex++ : null);
+		}
+		if ((getbits(LANCFG_DATA[i].v6IPSource,3,0) ==
+			top.CONSTANTS.IPSOURCE_DHCP) && (LANCFG_DATA[i].v6Enable == 1)) {
 			domainstr = top.settings.ethstr[index] + "_v6";
-			lstDomainSetting.add(new Option(domainstr,domainstr),isIE?domainindex++:null);
-		}
-		if (LANCFG_DATA[i].v6Enable == 1){
+			lstDomainSetting.add(new Option(domainstr, domainstr),
+				isIE ? domainindex++ : null);
+		}
+		if (LANCFG_DATA[i].v6Enable == 1) {
 			v6Enable = true;
 		}
 	}
-	if (!v6Enable)
-	{
-		lstV6DNSSetting.disabled = true;
-		txtV6DNS1.disabled = true;
-		txtV6DNS2.disabled = true;
-	}
 }
 
 function reloadDNSCfg()
 {
 	/* Host Configuration */
-	lstHostSetting.value = DNSCFG_DATA.hostSetting;
-	txtHostName.value = DNSCFG_DATA.hostName;
+	lstHostSetting.value = DNSCFG_DATA.HOST_CFG;
+	txtHostName.value = DNSCFG_DATA.HOST_NAME;
 	enableHost();
 
 	/* Register BMC Configuration */
-	chkRegisterBMC0.checked = (DNSCFG_DATA.registerBMC0 == 1) ? true : false;
+	chkRegisterBMC0.checked = (DNSCFG_DATA.REG_BMC0 == 1) ? true : false;
 	enableRegisterBMC(0);
-	chkRegisterBMC1.checked = (DNSCFG_DATA.registerBMC1 == 1) ? true : false;
+	chkRegisterBMC1.checked = (DNSCFG_DATA.REG_BMC1 == 1) ? true : false;
 	enableRegisterBMC(1);
-	chkRegisterBMC2.checked = (DNSCFG_DATA.registerBMC2 == 1) ? true : false;
+	chkRegisterBMC2.checked = (DNSCFG_DATA.REG_BMC2 == 1) ? true : false;
 	enableRegisterBMC(2);
 
-	if (chkRegisterBMC0.checked == true)
-	{
-		rdoRegisterDDNS0.checked = (DNSCFG_DATA.registerDHCP0 == 0) ? true : false;
-		rdoRegisterDHCP0.checked = (DNSCFG_DATA.registerDHCP0 == 1) ? true : false;
-	}
-	if (chkRegisterBMC1.checked == true)
-	{
-		rdoRegisterDDNS1.checked = (DNSCFG_DATA.registerDHCP1 == 0) ? true : false;
-		rdoRegisterDHCP1.checked = (DNSCFG_DATA.registerDHCP1 == 1) ? true : false;
-	}
-	if (chkRegisterBMC2.checked == true)
-	{
-		rdoRegisterDDNS2.checked = (DNSCFG_DATA.registerDHCP2 == 0) ? true : false;
-		rdoRegisterDHCP2.checked = (DNSCFG_DATA.registerDHCP2 == 1) ? true : false;
+	if (chkRegisterBMC0.checked == true) {
+		rdoRegisterDDNS0.checked = (DNSCFG_DATA.REG_DHCP0 == 0) ? true : false;
+		rdoRegisterDHCP0.checked = (DNSCFG_DATA.REG_DHCP0 == 1) ? true : false;
+	}
+	if (chkRegisterBMC1.checked == true) {
+		rdoRegisterDDNS1.checked = (DNSCFG_DATA.REG_DHCP1 == 0) ? true : false;
+		rdoRegisterDHCP1.checked = (DNSCFG_DATA.REG_DHCP1 == 1) ? true : false;
+	}
+	if (chkRegisterBMC2.checked == true) {
+		rdoRegisterDDNS2.checked = (DNSCFG_DATA.REG_DHCP2 == 0) ? true : false;
+		rdoRegisterDHCP2.checked = (DNSCFG_DATA.REG_DHCP2 == 1) ? true : false;
 	}
 
 	/* Domain Name Configuration */
-	lstDomainSetting.value = DNSCFG_DATA.domainSetting;
-	txtDomainName.value = DNSCFG_DATA.domainName;
+	lstDomainSetting.value = DNSCFG_DATA.DOMAIN_CFG;
+	txtDomainName.value = DNSCFG_DATA.DOMAIN_NAME;
 	enableDomain();
 
 	/* DNS Server Configuration */
-	lstV4DNSSetting.value = DNSCFG_DATA.v4DNSLANChannel;
-	txtV4DNS1.value = DNSCFG_DATA.v4DNS1;
-	txtV4DNS2.value = DNSCFG_DATA.v4DNS2;
-	enableV4DNS();
-
-	lstV6DNSSetting.value = DNSCFG_DATA.v6DNSLANChannel;
-	txtV6DNS1.value = DNSCFG_DATA.v6DNS1;
-	txtV6DNS2.value = DNSCFG_DATA.v6DNS2;
-	enableV6DNS();
+	lstDNSSetting.value = DNSCFG_DATA.DNS_CFG;
+	enableDNS();
+	txtDNS1.value = DNSCFG_DATA.DNS_IP1;
+	txtDNS2.value = DNSCFG_DATA.DNS_IP2;
+	txtDNS3.value = DNSCFG_DATA.DNS_IP3;
+	rdoV4Priority.checked = (DNSCFG_DATA.DNS_PRIORITY == CONST_IPV4_PRIORITY) ?
+		true : false;
+	rdoV6Priority.checked = (DNSCFG_DATA.DNS_PRIORITY == CONST_IPV6_PRIORITY) ?
+		true : false;
 
 	lstHostSetting.focus();
 }
@@ -267,24 +263,23 @@
 		case 0xE0:	case 0xE1:
 		case 0xE2:	case 0xE3:
 			errstr =  eLang.getString("common","STR_CONF_NW_SETVAL");
-			errstr += (eLang.getString("common","STR_CONF_NW_ERR_" + arg.HAPI_STATUS));
+			errstr += (eLang.getString("common","STR_CONF_NW_ERR_" +
+				arg.HAPI_STATUS));
 			alert(errstr);
 		break;
 
 		default:
 			errstr =  eLang.getString("common","STR_CONF_NW_SETVAL");
-			errstr += (eLang.getString("common","STR_IPMI_ERROR") + GET_ERROR_CODE(arg.HAPI_STATUS));
+			errstr += (eLang.getString("common","STR_IPMI_ERROR") +
+				GET_ERROR_CODE(arg.HAPI_STATUS));
 			alert(errstr);
 	}
 }
 
 function resetWebUI()
 {
-	showWait(false);
 	btnSave.disabled = true;
-	prepareDeviceShutdown();
-	var mb = MessageBox(eLang.getString("common","STR_CONF_NW_RESET_TITLE"),p(eLang.getString("common","STR_CONF_NW_RESET_DESC")),[],true);
-	clearCookies();
+	resetNetwork();
 }
 
 function Timedout()
@@ -295,142 +290,148 @@
 
 function setDNSCfg()
 {
-	if(confirm(eLang.getString("common","STR_CONF_NW_SAVE_CONFIRM")))
-	{
+	var req;
+	var index;
+	if (confirm(eLang.getString("common", "STR_CONF_NW_SAVE_CONFIRM"))) {
 		btnSave.disabled = true;
-		var index;
-		var req = new xmit.getset({url:"/rpc/setdnscfg.asp", onrcv:setDNSCfgRes, ontimeout:Timedout});
+		req = new xmit.getset({url:"/rpc/setdnscfg.asp", onrcv:setDNSCfgRes,
+			ontimeout:Timedout});
 
 		/* Host Configuration */
-		req.add("hostSetting", lstHostSetting.value);
-		req.add("hostName", txtHostName.value);
+		req.add("HOST_CFG", lstHostSetting.value);
+		if (lstHostSetting.value == 0) {
+			req.add("HOST_NAME", txtHostName.value);
+		}
 
 		/* Register BMC Configuration */
-		req.add("registerBMC0", (chkRegisterBMC0.checked == true) ? 1 : 0);
-		req.add("registerBMC1", (chkRegisterBMC1.checked == true) ? 1 : 0);
-		req.add("registerBMC2", (chkRegisterBMC2.checked == true) ? 1 : 0);
+		req.add("REG_BMC0", (chkRegisterBMC0.checked == true) ? 1 : 0);
+		req.add("REG_BMC1", (chkRegisterBMC1.checked == true) ? 1 : 0);
+		req.add("REG_BMC2", (chkRegisterBMC2.checked == true) ? 1 : 0);
 		
-		req.add("registerDHCP0", (chkRegisterBMC0.checked && rdoRegisterDHCP0.checked) ? 1 : 0);
-		req.add("registerDHCP1", (chkRegisterBMC1.checked && rdoRegisterDHCP1.checked) ? 1 : 0);
-		req.add("registerDHCP2", (chkRegisterBMC2.checked && rdoRegisterDHCP2.checked) ? 1 : 0);
+		req.add("REG_DHCP0", (chkRegisterBMC0.checked &&
+			rdoRegisterDHCP0.checked) ? 1 : 0);
+		req.add("REG_DHCP1", (chkRegisterBMC1.checked &&
+			rdoRegisterDHCP1.checked) ? 1 : 0);
+		req.add("REG_DHCP2", (chkRegisterBMC2.checked &&
+			rdoRegisterDHCP2.checked) ? 1 : 0);
 
 		/* Domain Name Configuration */
 		domaindata = lstDomainSetting.value.split("_");
-		if (domaindata.length == 2)
-		{
-			req.add("domainV4V6", domaindata[1]);
+		if (domaindata.length == 2) {
+			req.add("DOMAIN_PRIORITY", domaindata[1]);
 			//index = getIndexFromEthString(domaindata[0]);
-			req.add("domainEthIndex",top.settings.eth[getIndexFromEthString(domaindata[0])]);
-		}
-		else
-		{
-			req.add("domainV4V6", lstDomainSetting.value);	//This variable holds 'Manual' setting for Domain Name
-			req.add("domainName", txtDomainName.value);
-		}
-
-		/* IPv4 DNS Server Configuration */
-		if (lstV4DNSSetting.value == 0)		//Manual configuration
-		{
-			req.add("v4DNSDHCPEnable", 0);
-		}else
-		{
-			req.add("v4DNSDHCPEnable", 1);
-			req.add("v4DNSEthIndex", top.settings.eth[getIndexFromLANChannel(lstV4DNSSetting.value)]);
-		}
-		req.add("v4DNS1",txtV4DNS1.value);
-		req.add("v4DNS2",txtV4DNS2.value);
-
-		/* IPv6 DNS Server Configuration */
-		if (lstV6DNSSetting.value == 0)		//Manual configuration
-		{
-			req.add("v6DNSDHCPEnable", 0);
-		}else
-		{
-			req.add("v6DNSDHCPEnable", 1);
-			req.add("v6DNSEthIndex", top.settings.eth[getIndexFromLANChannel(lstV6DNSSetting.value)]);
-		}
-		req.add("v6DNS1",txtV6DNS1.value);
-		req.add("v6DNS2",txtV6DNS2.value);
+			req.add("DOMAIN_ETH",top.settings.eth[getIndexFromEthString(domaindata[0])]);
+		} else {
+			//This variable holds 'Manual' setting for Domain Name
+			req.add("DOMAIN_PRIORITY", lstDomainSetting.value);
+			req.add("DOMAIN_NAME", txtDomainName.value);
+		}
+
+		/* DNS Server Configuration */
+		if (lstDNSSetting.value == 0) {		//Manual configuration
+			req.add("DNS_DHCP", 0);
+			req.add("DNS_IP1",txtDNS1.value);
+			req.add("DNS_IP2",txtDNS2.value);
+			req.add("DNS_IP3",txtDNS3.value);
+		} else {
+			req.add("DNS_DHCP", 1);
+			req.add("DNS_ETH", 
+				top.settings.eth[getIndexFromLANChannel(lstDNSSetting.value)]);
+			req.add("DNS_PRIORITY", rdoV4Priority.checked ?
+				CONST_IPV4_PRIORITY : CONST_IPV6_PRIORITY);
+		}
 
 		req.send();
 		delete req;
-		timer = setTimeout("resetWebUI()",2000);
+		//timer = setTimeout("resetWebUI()",2000);
 	}
 }
 
 function validateDNSCfg()
 {
-	if (top.user.isAdmin())
-	{
-		if (lstHostSetting.value == 0)
-		{
-			if ((!eVal.hostname(txtHostName.value)) || (eVal.isblank(txtHostName.value)))
-			{
-				alert (eLang.getString("common","STR_CONF_DNS_INVALID_HOST"));
+	if (top.user.isAdmin()) {
+		if (lstHostSetting.value == 0) {
+			if ((!eVal.hostname(txtHostName.value)) ||
+				(eVal.isblank(txtHostName.value))) {
+				alert (eLang.getString("common", "STR_CONF_DNS_INVALID_HOST"));
 				txtHostName.focus();
 				return;
 			}
 		}
 
-		if (lstDomainSetting.value == eLang.getString("common","STR_MANUAL"))
-		{
-			if ((!eVal.domainname(txtDomainName.value,true)) || (eVal.isblank(txtDomainName.value)))
-			{
-				alert (eLang.getString("common","STR_CONF_DNS_INVALID_DOMAIN"));
+		if (lstDomainSetting.value == eLang.getString("common", "STR_MANUAL")) {
+			if ((!eVal.domainname(txtDomainName.value, true)) ||
+				(eVal.isblank(txtDomainName.value))) {
+				alert (eLang.getString("common", "STR_CONF_DNS_INVALID_DOMAIN"));
 				txtDomainName.focus();
 				return;
 			}
 		}
 
-		if (lstV4DNSSetting.value == 0)
-		{
-			if ((eVal.isblank(txtV4DNS1.value)) || (!eVal.ip(txtV4DNS1.value,true)))
-			{
-				alert (eLang.getString("common","STR_CONF_DNS_INVALID_V4DNS1"));
-				txtV4DNS1.focus();
-				return;
-			}
-			if((!eVal.isblank(txtV4DNS2.value)) && (!eVal.ip(txtV4DNS2.value,true)))
-			{
-				alert (eLang.getString("common","STR_CONF_DNS_INVALID_V4DNS2"));
-				txtV4DNS2.focus();
-				return;
-			}
-			if (eVal.compareip(txtV4DNS1.value,txtV4DNS2.value))
-			{
-				alert (eLang.getString("common","STR_CONF_DNS_DIFF_DNS"));
-				txtV4DNS2.focus();
-				return;
-			}
-		}
-
-		if (lstV6DNSSetting.value == 0 && !lstV6DNSSetting.disabled)
-		{
-			if ((eVal.isblank(txtV6DNS1.value)) || (!eVal.ipv6(txtV6DNS1.value, true, false)))
-			{
-				alert (eLang.getString("common","STR_CONF_DNS_INVALID_V6DNS1"));
-				txtV6DNS1.focus();
-				return;
-			}
-			if((!eVal.isblank(txtV6DNS2.value)) && (!eVal.ipv6(txtV6DNS2.value, true, true)))
-			{
-				alert (eLang.getString("common","STR_CONF_DNS_INVALID_V6DNS2"));
-				txtV6DNS2.focus();
-				return;
-			}
-			if (eVal.comparev6ip(txtV6DNS1.value,txtV6DNS2.value))
-			{
-				alert (eLang.getString("common","STR_CONF_DNS_DIFF_DNS"));
-				txtV6DNS2.focus();
-				return;
+		if (lstDNSSetting.value == 0) {
+			if ((eVal.isblank(txtDNS1.value)) &&
+				(eVal.isblank(txtDNS2.value)) &&
+				(eVal.isblank(txtDNS3.value))) {
+				alert(eLang.getString("common", "STR_CONF_DNS_BLANK"));
+				txtDNS1.focus();
+				return false;
+			}
+
+			if (!validateServerAddress(txtDNS1)) {
+				return false;
+			}
+
+			if (!validateServerAddress(txtDNS2)) {
+				return false;
+			}
+
+			if (!validateServerAddress(txtDNS3)) {
+				return false;
+			}
+
+			if ((eVal.compareip(txtDNS1.value, txtDNS2.value)) || 
+				(eVal.compareip(txtDNS1.value, txtDNS3.value)) || 
+				(eVal.compareip(txtDNS2.value, txtDNS3.value))) {
+				alert(eLang.getString("common", "STR_CONF_DNS_DIFF"));
+				return false;
+			}
+
+			if (v6Enable) {
+				if ((eVal.comparev6ip(txtDNS1.value, txtDNS2.value)) || 
+					(eVal.comparev6ip(txtDNS1.value, txtDNS3.value)) || 
+					(eVal.comparev6ip(txtDNS2.value, txtDNS3.value))) {
+					alert(eLang.getString("common", "STR_CONF_DNS_DIFF"));
+					return false;
+				}
 			}
 		}
 		setDNSCfg();
-	}
-	else
-	{
-		alert(eLang.getString("common","STR_CONF_ADMIN_PRIV"));
-	}
+	} else {
+		alert(eLang.getString("common", "STR_CONF_ADMIN_PRIV"));
+	}
+}
+
+function validateServerAddress(txtDNS)
+{
+	if (!eVal.isblank(txtDNS.value)) {
+		if (!eVal.ip(txtDNS.value)) {
+			if (v6Enable) {
+				if (!eVal.ipv6(txtDNS.value)) {
+					alert(eLang.getString("common",
+						"STR_CONF_DNS_INVALID_DNS") +
+						eLang.getString("common", "STR_HELP_INFO"));
+					txtDNS.focus();
+					return false;
+				}
+			} else {
+				alert(eLang.getString("common", "STR_CONF_DNS_INVALID_DNS") +
+					eLang.getString("common", "STR_CONF_DNS_V6DISABLE"));
+				txtDNS.focus();
+				return false;
+			}
+		}
+	}
+	return true;
 }
 
 function enableHost()
@@ -460,11 +461,10 @@
 
 function enableRegisterBMC(index)
 {
-	if ($("_chkRegisterBMC" + index).checked == true)
-	{
+	if ($("_chkRegisterBMC" + index).checked == true) {
 		$("_rdoRegisterDDNS" + index).disabled = false;
 		$("_rdoRegisterDHCP" + index).disabled = !(varDHCP[index]);
-	}else{
+	} else {
 		$("_rdoRegisterDDNS" + index).disabled = true;
 		$("_rdoRegisterDHCP" + index).disabled = true;
 		$("_rdoRegisterDDNS" + index).checked = false;
@@ -477,16 +477,36 @@
 	txtDomainName.disabled = (lstDomainSetting.value == "Manual") ? false : true;
 }
 
-function enableV4DNS()
-{
-	var enable = (lstV4DNSSetting.value == 0) ? false : true;
-	txtV4DNS1.disabled = enable;
-	txtV4DNS2.disabled = enable;
-}
-
-function enableV6DNS()
-{
-	var enable = ((lstV6DNSSetting.value == 0) && (!lstV6DNSSetting.disabled)) ? false : true;
-	txtV6DNS1.disabled = enable;
-	txtV6DNS2.disabled = enable;
-}
+function enableDNS()
+{
+	var index;
+	var opt = (lstDNSSetting.value == 0) ? false : true;
+
+	if (opt) {
+		index = getIndexFromLANChannel(lstDNSSetting.value);
+		if (LANCFG_DATA[index].lanEnable && 
+			(LANCFG_DATA[index].v4IPSource == top.CONSTANTS.IPSOURCE_DHCP)) {
+			rdoV4Priority.checked = true;
+			rdoV4Priority.disabled = false;
+		} else {
+			rdoV4Priority.checked = false;
+			rdoV4Priority.disabled = true;
+		}
+
+		if (LANCFG_DATA[index].lanEnable && LANCFG_DATA[index].v6Enable &&
+			(LANCFG_DATA[index].v6IPSource == top.CONSTANTS.IPSOURCE_DHCP)) {
+			rdoV6Priority.disabled = false;
+		} else {
+			rdoV6Priority.checked = false;
+			rdoV6Priority.disabled = true;
+		}
+	} else {
+		rdoV4Priority.checked = false;
+		rdoV4Priority.disabled = true;
+		rdoV6Priority.checked = false;
+		rdoV6Priority.disabled = true;
+	}
+	txtDNS1.disabled = opt;
+	txtDNS2.disabled = opt;
+	txtDNS3.disabled = opt;
+}
